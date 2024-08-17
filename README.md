# **special_keys**

Provides functions for common special (multimedia) keys. Communicates with [status_bar](https://github.com/cshmookler/status_bar) for instant visual feedback.

## Build from Source

### 1.&nbsp; Install a C++ compiler, Meson, GoogleTest (optional), argparse, and ALSA libraries.

#### Linux (Arch):

```bash
sudo pacman -S base-devel meson gtest argparse alsa-lib 
```

### 2.&nbsp; Clone this project.

This project can be downloaded online [here](https://github.com/cshmookler/special_keys).

Alternatively, if you have [Git](https://git-scm.com/downloads/) installed, open a shell and enter the commands below.  This project will be downloaded to the current working directory.

```bash
git clone https://github.com/cshmookler/special_keys.git
cd special_keys
```

### 3.&nbsp; Build this project from source.

```bash
meson setup build
cd build
ninja
```

## **TODO**

- [X] playback volume mute
- [X] playback volume up/down
- [X] capture volume mute
- [X] capture volume up/down
- [X] backlight up/down
