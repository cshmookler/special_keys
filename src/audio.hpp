#pragma once

/**
 * @file audio.hpp
 * @author Caden Shmookler (cshmookler@gmail.com)
 * @brief Utilities for manipulating audio settings.
 * @date 2024-08-16
 */

// Standard includes
#include <cmath>
#include <functional>
#include <optional>
#include <string_view>
#include <iostream>
#include <type_traits>

// External includes
#include <alsa/asoundlib.h>

namespace keys {

class Sound_mixer {
  public:
    enum class mode {
        playback,
        capture,
    };

    enum class status : int {
        off = 0,
        on = 1,
    };

  private:
    static constexpr const char* default_card = "default";

    static constexpr const char* playback_name = "Master";
    static constexpr int playback_index = 0;

    static constexpr const char* capture_name = "Capture";
    static constexpr int capture_index = 0;

    static constexpr int mixer_mode = 0;

    using get_switch_func =
      std::function<int(snd_mixer_elem_t*, snd_mixer_selem_channel_id_t, int*)>;
    using set_switch_func = std::function<int(snd_mixer_elem_t*, int)>;
    using get_volume_range_func =
      std::function<int(snd_mixer_elem_t*, long*, long*)>;
    using get_volume_func = std::function<int(
      snd_mixer_elem_t*, snd_mixer_selem_channel_id_t, long*)>;
    using set_volume_func = std::function<int(snd_mixer_elem_t*, long)>;

    const char* get_switch_func_name_;
    const char* set_switch_func_name_;
    const char* get_volume_range_func_name_;
    const char* get_volume_func_name_;
    const char* set_volume_func_name_;

    get_switch_func get_switch_func_;
    set_switch_func set_switch_func_;
    get_volume_range_func get_volume_range_func_;
    get_volume_func get_volume_func_;
    set_volume_func set_volume_func_;

    snd_mixer_t* mixer_ = nullptr;
    snd_mixer_elem_t* mixer_elem_ = nullptr;

    bool good_ = false;

    template<typename F, typename... A>
    [[nodiscard]] static bool handle_error_(
      const std::string_view& function_name, F function, A... args) {
        static_assert(std::is_invocable_v<F, A...>);

        int errnum = function(args...);
        if (errnum == 0) {
            return false;
        }

        std::cerr << function_name << "(): " << snd_strerror(errnum) << '\n';

        return true;
    }

    [[nodiscard]] static inline long get_percent_(
      long min, long max, long raw) {
        double ratio = static_cast<double>(raw - min)
          / static_cast<double>(max - min) * 1e2F;
        return static_cast<long>(std::round(ratio));
    }

    [[nodiscard]] static inline long get_raw_(
      long min, long max, long percent) {
        double ratio =
          static_cast<double>(percent) * static_cast<double>(max - min) / 1e2F;
        return min + static_cast<long>(std::round(ratio));
    }

  public:
    Sound_mixer(mode selem_mode);
    Sound_mixer(const Sound_mixer&) = delete;
    Sound_mixer(Sound_mixer&& sound_mixer) noexcept;
    Sound_mixer& operator=(const Sound_mixer&) = delete;
    Sound_mixer& operator=(Sound_mixer&& sound_mixer) noexcept = default;
    ~Sound_mixer();

    [[nodiscard]] inline bool good() const {
        return this->good_;
    }

    [[nodiscard]] std::optional<status> get_status();

    bool set_status(status new_status);

    [[nodiscard]] std::optional<long> get_volume();

    bool set_volume(long new_volume);
};

} // namespace keys
