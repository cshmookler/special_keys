// Local includes
#include "audio.hpp"

namespace keys {

Sound_mixer::Sound_mixer(Sound_mixer::mode selem_mode) {
    if (handle_error_(
          "snd_mixer_open", snd_mixer_open, &this->mixer_, mixer_mode)) {
        return;
    }
    if (handle_error_(
          "snd_mixer_attach", snd_mixer_attach, this->mixer_, default_card)) {
        return;
    }
    if (handle_error_("snd_mixer_selem_register",
          snd_mixer_selem_register,
          this->mixer_,
          nullptr,
          nullptr)) {
        return;
    }
    if (handle_error_("snd_mixer_load", snd_mixer_load, this->mixer_)) {
        return;
    }

    snd_mixer_selem_id_t* selem = nullptr;
    if (handle_error_(
          "snd_mixer_selem_id_malloc", snd_mixer_selem_id_malloc, &selem)) {
        return;
    }

    const char* selem_name = nullptr;
    int selem_index = 0;
    if (selem_mode == mode::playback) {
        selem_name = playback_name;
        selem_index = playback_index;
        this->get_switch_func_name_ = "snd_mixer_selem_get_playback_switch";
        this->set_switch_func_name_ = "snd_mixer_selem_set_playback_switch_all";
        this->get_volume_range_func_name_ =
          "snd_mixer_selem_get_playback_volume_range";
        this->get_volume_func_name_ = "snd_mixer_selem_get_playback_volume";
        this->set_volume_func_name_ = "snd_mixer_selem_set_playback_volume_all";
        this->get_switch_func_ = snd_mixer_selem_get_playback_switch;
        this->set_switch_func_ = snd_mixer_selem_set_playback_switch_all;
        this->get_volume_range_func_ =
          snd_mixer_selem_get_playback_volume_range;
        this->get_volume_func_ = snd_mixer_selem_get_playback_volume;
        this->set_volume_func_ = snd_mixer_selem_set_playback_volume_all;
    } else { // mode::capture
        selem_name = capture_name;
        selem_index = capture_index;
        this->get_switch_func_name_ = "snd_mixer_selem_get_capture_switch";
        this->set_switch_func_name_ = "snd_mixer_selem_set_capture_switch_all";
        this->get_volume_range_func_name_ =
          "snd_mixer_selem_get_capture_volume_range";
        this->get_volume_func_name_ = "snd_mixer_selem_get_capture_volume";
        this->set_volume_func_name_ = "snd_mixer_selem_set_capture_volume_all";
        this->get_switch_func_ = snd_mixer_selem_get_capture_switch;
        this->set_switch_func_ = snd_mixer_selem_set_capture_switch_all;
        this->get_volume_range_func_ = snd_mixer_selem_get_capture_volume_range;
        this->get_volume_func_ = snd_mixer_selem_get_capture_volume;
        this->set_volume_func_ = snd_mixer_selem_set_capture_volume_all;
    }

    snd_mixer_selem_id_set_name(selem, selem_name);
    snd_mixer_selem_id_set_index(selem, selem_index);

    this->mixer_elem_ = snd_mixer_find_selem(this->mixer_, selem);
    if (this->mixer_elem_ == nullptr) {
        std::cerr << "snd_mixer_find_selem(): failed to find the simple "
                     "mixer element handle\n";
        return;
    }

    this->good_ = true;
}

Sound_mixer::Sound_mixer(Sound_mixer&& sound_mixer) noexcept
: get_switch_func_name_(sound_mixer.get_switch_func_name_)
, set_switch_func_name_(sound_mixer.set_switch_func_name_)
, get_volume_range_func_name_(sound_mixer.get_volume_range_func_name_)
, get_volume_func_name_(sound_mixer.get_volume_func_name_)
, set_volume_func_name_(sound_mixer.set_volume_func_name_)
, get_switch_func_(std::move(sound_mixer.get_switch_func_))
, set_switch_func_(std::move(sound_mixer.set_switch_func_))
, get_volume_range_func_(std::move(sound_mixer.get_volume_range_func_))
, get_volume_func_(std::move(sound_mixer.get_volume_func_))
, set_volume_func_(std::move(sound_mixer.set_volume_func_))
, mixer_(sound_mixer.mixer_)
, good_(sound_mixer.good_) {
    sound_mixer.mixer_ = nullptr;
    sound_mixer.good_ = false;
}

Sound_mixer::~Sound_mixer() {
    if (this->mixer_ != nullptr) {
        snd_mixer_close(this->mixer_);
        // do nothing if the mixer fails to close.
    }
}

std::optional<Sound_mixer::status> Sound_mixer::get_status() {
    if (! this->good()) {
        return std::nullopt;
    }

    int front_left_state = -1;
    if (handle_error_(this->get_switch_func_name_,
          this->get_switch_func_,
          this->mixer_elem_,
          snd_mixer_selem_channel_id_t::SND_MIXER_SCHN_FRONT_LEFT,
          &front_left_state)) {
        return std::nullopt;
    }

    return static_cast<status>(front_left_state);
}

bool Sound_mixer::set_status(Sound_mixer::status new_status) {
    if (! this->good()) {
        return false;
    }

    if (handle_error_(this->set_switch_func_name_,
          this->set_switch_func_,
          this->mixer_elem_,
          static_cast<int>(new_status))) {
        return false;
    }

    return true;
}

std::optional<long> Sound_mixer::get_volume() {
    if (! this->good()) {
        return std::nullopt;
    }

    long min_volume = -1;
    long max_volume = -1;
    if (handle_error_(this->get_volume_range_func_name_,
          this->get_volume_range_func_,
          this->mixer_elem_,
          &min_volume,
          &max_volume)) {
        return std::nullopt;
    }

    long front_left_volume = -1;
    if (handle_error_(this->get_volume_func_name_,
          this->get_volume_func_,
          this->mixer_elem_,
          snd_mixer_selem_channel_id_t::SND_MIXER_SCHN_FRONT_LEFT,
          &front_left_volume)) {
        return std::nullopt;
    }

    return { get_percent_(min_volume, max_volume, front_left_volume) };
}

bool Sound_mixer::set_volume(long new_volume) {
    if (! this->good()) {
        return false;
    }

    long min_volume = -1;
    long max_volume = -1;
    if (handle_error_(this->get_volume_range_func_name_,
          this->get_volume_range_func_,
          this->mixer_elem_,
          &min_volume,
          &max_volume)) {
        return false;
    }

    return ! handle_error_(this->set_volume_func_name_,
      this->set_volume_func_,
      this->mixer_elem_,
      get_raw_(min_volume, max_volume, new_volume));
}

} // namespace keys
