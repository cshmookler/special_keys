# **special_keys**

Provides functions for common special (multimedia) keys. Communicates with [status_bar](https://github.com/cshmookler/status_bar) for instant visual feedback.

## Build from Source

### 1.&nbsp; Install a C++ compiler, Git, Meson, GoogleTest, [cpp_result](https://github.com/cshmookler/cpp_result), [status_bar](https://github.com/cshmookler/status_bar), and [system_state](https://github.com/cshmookler/system_state).

#### Linux (MOOS):

```bash
sudo pacman -S base-devel git meson gtest moos-cpp-result moos-status-bar moos-system-state
```

### 2.&nbsp; Clone this project.

```
git clone https://github.com/cshmookler/special_keys.git
cd special_keys
```

### 3.&nbsp; Build this project from source.

```
meson setup build
cd build
ninja
```

### 4.&nbsp; (Optional) Install this project globally.

```
meson install
```

## **TODO**

- [X] playback volume mute
- [X] playback volume up/down
- [X] capture volume mute
- [X] capture volume up/down
- [X] backlight up/down
- [X] communicate with status_bar
- [ ] add more to the help message
- [X] improve error reporting and logging
- [ ] add more documentation
- [ ] add tests
