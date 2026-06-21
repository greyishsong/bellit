# README

**Bell It** is a simple tool for sending native desktop notification from terminal. Its name is inspired by the ASCII character `BEL` (`0x07`), which is heritage inherited from teletypes.

The main motivation of this project is to provide better notification for backend, long running tasks in terminal, especially the coding agents (Claude Code, Codex, OpenCode, etc). Even if some desktop application like Cursor or Codex Desktop have already integrate notification mechanism, I still prefer to developing in terminal. Bell It is designed for such situations. It is also an attempt for me to learn C++ 20's `module` feature.

## Usage

### CLI

Bell It is primarily a command line tool accepting Unix & GNU style command line options. The basic usage is to send a notification instantly:
```shell
bellit -t "Your title" -m "Your message"
# equal as
bellit --title "Your title" --message "Your message"
```

Its also possible to send a notification after a task has been accomplished:
```shell
bellit -t "Compression" -m "Gzip package created." -- tar -zcvf large-file.tar.gz /path/to/large/directory
bellit -t "Project Summary" -m "OpenCode has summarized the project" -- opencode run "Summarize this project, introduce its usage and architecture for me. Write the introduction to intro.md"
```

If your task may fail, its also possible to specify a warning message to inform the failure:
```shell
bellit -t "Ping" -m "Connected" -w "Disconnected" -- ping -c 1 192.168.1.1
```

### Skill for Agents

Agents can also use `bellit` through a skill. To install the skill for your agent, install the executable `bellit` and then download the `skill/bellit` directory into your agent's skill directory.

## Installation

All packages are available from GitHub Release, some are available through package manager.

### Linux

Debian 13+ and Ubuntu 24+ are supported. For Debian/Ubuntu users, install the `deb` package with `dpkg`.

For Arch/Manjaro users, install the `pkg.tar.zst` package manually with `pacman`, or install the AUR package `bellit-bin`.

### Windows

Install with `winget`:
```shell
winget install --id greyishsong.bellit
```

Or download the `bellit-windows-x64.zip`, extract `bellit.exe` to any directory in your `PATH`.

### macOS

Install the homebrew package `bellit`:
```shell
brew tap greyishsong/bellit-homebrew
brew install bellit
```

## Development

For better support to C++ module, this project requires
- CMake 4.1+
- Ninja
- Clang 18+ (LLVM/Clang, not AppleClang/Clang-cl)

Because different compilers use different Binary Module Interface (BMI) formats, it is recommended to use clangd-based LSP plugins in your IDE/editor. It is possible to build this project with another toolchain, but I cannot provide any guarantee.

To build `bellit`:
```shell
cmake --preset default
cmake --build --preset default
```

Then you will have an executable `build/bellit` not depending on any 3rd-party dynamic library. Install it to anywhere you prefer.

## Limitations

### SSH/TMUX Session Passthrough

Bell It is able to detect SSH/TMUX session (through environment variables) and switch to OSC control sequence. The control sequence will be passed through SSH/TMUX session, then trigger a notification on your local computer. But OSC 777 sequence only supports specifying title and message, you will not see info/warning icon as you run `bellit` locally.

### Windows Terminal

Windows Terminal does not support OSC 777 notifications. When you run an SSH client in Windows Terminal, and run `bellit` on the remote machine, you will not receive any notification.

### macOS

On macOS, Bell It uses Apple Script `display notification` to send notifications. `display notification` does not support specifying icon, so the icon in notification may be the default icon or application icon of your terminal emulator.
