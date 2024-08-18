// Standard includes
#include <iostream>
#include <string>

// External includes
#include <argparse/argparse.hpp>

// Local includes
#include "function.hpp"
#include "version.hpp"

const char* const playback = "playback";
const char* const capture = "capture";
const char* const backlight = "backlight";

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
                return static_cast<int>(! keys::playback_toggle());
            }
            return static_cast<int>(! keys::playback(std::stol(param)));
        }
        if (function == capture) {
            if (param == "toggle") {
                return static_cast<int>(! keys::capture_toggle());
            }
            return static_cast<int>(! keys::capture(std::stol(param)));
        }
        if (function == backlight) {
            return static_cast<int>(! keys::backlight(std::stol(param)));
        }
    } catch (const std::invalid_argument& error) {
        return 1;
    }

    return 1;
}
