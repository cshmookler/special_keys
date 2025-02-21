// Standard includes
#include <iostream>
#include <string>

// External includes
#include <argparse/argparse.hpp>
#include <cpp_result/all.hpp>
#include <status_bar/notify.hpp>
#include <system_state/system_state.hpp>

// Local includes
#include "version.hpp"

res::result_t toggle_playback() {
    auto sound_mixer = syst::get_sound_mixer();
    if (sound_mixer.has_error()) {
        return RES_TRACE(sound_mixer.error());
    }

    for (auto& control : sound_mixer->get_controls()) {
        if (! control.has_playback_status()) {
            continue;
        }

        auto result = control.toggle_playback_status();
        if (result.failure()) {
            return RES_TRACE(result.error());
        }
    }

    if (! sbar::notify(sbar::field::volume)) {
        return RES_NEW_ERROR(
          "Failed to notify status_bar that the volume has been changed");
    }

    return res::success;
}

res::result_t set_playback_volume(double volume) {
    auto sound_mixer = syst::get_sound_mixer();
    if (sound_mixer.has_error()) {
        return RES_TRACE(sound_mixer.error());
    }

    for (auto& control : sound_mixer->get_controls()) {
        if (! control.has_playback_volume()) {
            continue;
        }

        auto result = control.set_playback_volume_all_relative(volume);
        if (result.failure()) {
            return RES_TRACE(result.error());
        }
    }

    if (! sbar::notify(sbar::field::volume)) {
        return RES_NEW_ERROR(
          "Failed to notify status_bar that the volume has been changed");
    }

    return res::success;
}

res::result_t toggle_capture() {
    auto sound_mixer = syst::get_sound_mixer();
    if (sound_mixer.has_error()) {
        return RES_TRACE(sound_mixer.error());
    }

    for (auto& control : sound_mixer->get_controls()) {
        if (! control.has_capture_status()) {
            continue;
        }

        auto result = control.toggle_capture_status();
        if (result.failure()) {
            return RES_TRACE(result.error());
        }
    }

    if (! sbar::notify(sbar::field::volume)) {
        return RES_NEW_ERROR(
          "Failed to notify status_bar that the volume has been changed");
    }

    return res::success;
}

res::result_t set_capture_volume(double volume) {
    auto sound_mixer = syst::get_sound_mixer();
    if (sound_mixer.has_error()) {
        return RES_TRACE(sound_mixer.error());
    }

    for (auto& control : sound_mixer->get_controls()) {
        if (! control.has_capture_volume()) {
            continue;
        }

        auto result = control.set_capture_volume_all_relative(volume);
        if (result.failure()) {
            return RES_TRACE(result.error());
        }
    }

    if (! sbar::notify(sbar::field::volume)) {
        return RES_NEW_ERROR(
          "Failed to notify status_bar that the volume has been changed");
    }

    return res::success;
}

res::result_t set_backlight_brightness(double brightness) {
    auto backlights = syst::get_backlights();
    if (backlights.has_error()) {
        return RES_TRACE(backlights.error());
    }

    for (auto& backlight : backlights.value()) {
        auto result = backlight.set_brightness_relative(brightness);
        if (result.failure()) {
            return RES_TRACE(result.error());
        }
    }

    if (! sbar::notify(sbar::field::backlight)) {
        return RES_NEW_ERROR("Failed to notify status_bar that the backlight "
                             "brightness has been changed");
    }

    return res::success;
}

int main(int argc, char** argv) {
    // Setup the argument parser
    argparse::ArgumentParser program{ "special_keys",
        keys::get_runtime_version(),
        argparse::default_arguments::all,
        true };

    program.add_description(
      "Provides functions for common special (multimedia) keys. Communicates "
      "with status_bar (https://github.com/cshmookler/status_bar) for instant "
      "visual feedback.");

    const char* const playback = "playback";
    const char* const capture = "capture";
    const char* const backlight = "backlight";

    program.add_argument("function")
      .required()
      .choices(playback, capture, backlight);

    program.add_argument("param");

    // Parse arguments
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << "\n\n";
        std::cerr << program;
        return 1;
    }

    // Interpret arguments
    std::string function = program.get("function");
    std::string param = program.get("param");
    try {
        if (function == playback) {
            if (param == "toggle") {
                auto result = toggle_playback();
                if (result.failure()) {
                    std::cerr << result.error().string() << '\n';
                    return 1;
                }
                return 0;
            }

            auto result = set_playback_volume(std::stod(param));
            if (result.failure()) {
                std::cerr << result.error().string() << '\n';
                return 1;
            }
            return 0;
        }
        if (function == capture) {
            if (param == "toggle") {
                auto result = toggle_capture();
                if (result.failure()) {
                    std::cerr << result.error().string() << '\n';
                    return 1;
                }
                return 0;
            }

            auto result = set_capture_volume(std::stod(param));
            if (result.failure()) {
                std::cerr << result.error().string() << '\n';
                return 1;
            }
            return 0;
        }
        if (function == backlight) {
            auto result = set_backlight_brightness(std::stod(param));
            if (result.failure()) {
                std::cerr << result.error().string() << '\n';
                return 1;
            }
            return 0;
        }
    } catch (const std::invalid_argument& error) {
        std::cerr << "std::invalid_argument: " << error.what() << '\n';
        return 1;
    }

    return 1;
}
