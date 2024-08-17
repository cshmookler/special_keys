// Standard includes
#include <cmath>
#include <filesystem>
#include <fstream>

// Local includes
#include "function.hpp"
#include "audio.hpp"

namespace keys {

bool toggle(Sound_mixer::mode mode) {
    Sound_mixer sound_mixer(mode);
    if (! sound_mixer.good()) {
        return false;
    }

    std::optional<Sound_mixer::status> status = sound_mixer.get_status();
    if (! status.has_value()) {
        return false;
    }

    Sound_mixer::status inverted_status =
      static_cast<Sound_mixer::status>(! static_cast<bool>(status.value()));
    return sound_mixer.set_status(inverted_status);
}

bool volume(Sound_mixer::mode mode, long percent_change) {
    Sound_mixer sound_mixer(mode);
    if (! sound_mixer.good()) {
        return false;
    }

    std::optional<long> volume = sound_mixer.get_volume();
    if (! volume.has_value()) {
        return false;
    }

    return sound_mixer.set_volume(volume.value() + percent_change);
}

bool playback_toggle() {
    return toggle(Sound_mixer::mode::playback);
}

bool playback(long percent_change) {
    return volume(Sound_mixer::mode::playback, percent_change);
}

bool capture_toggle() {
    return toggle(Sound_mixer::mode::capture);
}

bool capture(long percent_change) {
    return volume(Sound_mixer::mode::capture, percent_change);
}

[[nodiscard]] std::optional<long> read_long(const std::filesystem::path& path) {
    std::ifstream file{ path };
    std::string first_line;
    if (! std::getline(file, first_line).good()) {
        return std::nullopt;
    }
    try {
        return std::stol(first_line);
    } catch (const std::invalid_argument& error) {
        return std::nullopt;
    }
}

bool write_long(const std::filesystem::path& path, long value) {
    std::ofstream file{ path };
    file << value;
    return file.good();
}

bool backlight(long percent_change) {
    // documentation for /sys/class/backlight/:
    // https://github.com/torvalds/linux/blob/master/include/linux/backlight.h
    // https://docs.kernel.org/gpu/backlight.html

    const char* const devices_path = "/sys/class/backlight/";
    const char* const device_brightness_filename = "brightness";
    const char* const device_max_brightness_filename = "max_brightness";

    std::optional<std::filesystem::path> backlight_path;

    for (const std::filesystem::directory_entry& device :
      std::filesystem::directory_iterator(devices_path)) {
        if (! std::filesystem::exists(
              device.path() / device_brightness_filename)) {
            continue;
        }
        if (! std::filesystem::exists(
              device.path() / device_max_brightness_filename)) {
            continue;
        }

        backlight_path = device.path();
        break;
    }

    if (! backlight_path.has_value()) {
        return false;
    }

    std::filesystem::path device_brightness_path =
      backlight_path.value() / device_brightness_filename;
    std::filesystem::path device_max_brightness_path =
      backlight_path.value() / device_max_brightness_filename;

    std::optional<long> brightness = read_long(device_brightness_path);
    if (! brightness.has_value()) {
        return false;
    }

    std::optional<long> max_brightness = read_long(device_max_brightness_path);
    if (! max_brightness.has_value()) {
        return false;
    }

    double raw_change = static_cast<double>(percent_change)
      * static_cast<double>(max_brightness.value()) / 1e2F;

    return write_long(device_brightness_path,
      brightness.value() + static_cast<long>(std::round(raw_change)));
}

} // namespace keys
