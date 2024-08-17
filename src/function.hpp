#pragma once

/**
 * @file function.hpp
 * @author Caden Shmookler (cshmookler@gmail.com)
 * @brief Functions for common special (multimedia) keys.
 * @date 2024-08-16
 */

namespace keys {

bool playback_toggle();

bool playback(long percent_change);

bool capture_toggle();

bool capture(long percent_change);

bool backlight(long percent_change);

} // namespace keys
