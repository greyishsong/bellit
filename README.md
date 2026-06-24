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

Bell It can be installed through some package managers. If the package manager you use is not supported, you can just build it from source and place `bellit`/`bellit.exe` to any directory in your `PATH`.

### Linux

Packages are published in GitHub Release.

Debian 13+ and Ubuntu 24+ are supported. For Debian/Ubuntu users, install the `deb` package with `dpkg`.

For Arch/Manjaro users, install the `pkg.tar.zst` package manually with `pacman`, or install the AUR package `bellit`.

### Windows

Install with `winget`:
```shell
winget install --id greyishsong.bellit
```

### macOS

Install the homebrew package `bellit`:
```shell
brew tap greyishsong/bellit
brew trust greyishsong/bellit
brew install bellit
```

It's recommended to install `vjeantet/alerter` for better experience:
```shell
brew install vjeantet/tap/alerter
```

### Build from Source

For better support to C++ module, this project requires
- CMake 4.1+
- Ninja
- Clang 18+ (LLVM/Clang, not AppleClang/Clang-cl)

Because different compilers use different Binary Module Interface (BMI) formats, it is recommended to use clangd-based LSP plugins in your IDE/editor. It is possible to build this project with another toolchain, but I cannot provide any guarantee.

To build `bellit`:
```shell
cmake --preset default -DCMAKE_BUILD_TYPE=Release # For developing, you can also build Debug here
cmake --build --preset default
```

Then you will have an executable `build/bellit`.

## Limitations

### SSH Session

Bell It is able to detect SSH session (through environment variables) and switch to OSC control sequence. The control sequence will be passed through SSH session, then trigger a notification on your local computer. Even if you run `bellit` in a tmux/zellij session in your SSH server, OSC control sequence can be passed back to your machine.

However, the cost of simplicity is limited SSH session support:

- OSC 777 control sequence does not support icons in notification.
- For multi-hop SSH connection (i.e. using one ore more bastion hosts), tmux/zellij support is limited to the last hop.

### Windows Terminal

Windows Terminal does not support OSC 777 notifications. When you run an SSH client in Windows Terminal, and run `bellit` on the remote machine, you will not receive any notification.

### macOS

On macOS, Bell It uses Apple Script `display notification` to send notifications. `display notification` does not support specifying icon, so the icon in notification may be the default icon or application icon of your terminal emulator.
