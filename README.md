# README

**Bell It** is a simple tool for sending native desktop notification from terminal. Its name is inspired by the ASCII character `BEL` (`0x07`), which is heritage inherited from teletypes.

The main motivation of this project is to provide better notification for backend, long running tasks in terminal, especially the coding agents (Claude Code, Codex, OpenCode, etc). Even if some desktop application like Cursor or Codex Desktop have already integrate notification mechanism, I still prefer to developing in terminal. Bell It is designed for such situations. It is also an attempt for me to learn C++ 20's `module` feature.

## Usage

### CLI

Bell It is primarily a command line tool accepting Unix & GNU style command line options. The basic usage is to send a notification instantly:
```shell
bellit -t "Your title" -m "Your message"
# or
bellit --title "Your title" --message "Your message"
```

Its also possible to send a notification after a task has been accomplished:
```shell
bellit -t "Compression" -m "Gzip package created." -- tar -zcvf large-file.tar.gz /path/to/large/directory
# or
bellit -t "Project Summary" -m "OpenCode has summarized the project" -- opencode run "Summarize this project, introduce its usage and architecture for me. Write the introduction to intro.md"
```

If your task may fail, its also possible to specify a warning message to inform the failure:
```shell
bellit -t "Ping" -m "Connected" -w "Disconnected" -- ping -c 1 192.168.1.1
```

### TODO: Skill for Agents

> Coming soon.

Agents running in terminal can also learn to use `bellit` as a skill.

## Development

For better support to C++ module, this project requires
- CMake 4.1+
- Ninja
- Clang++

Because different compilers use different Binary Module Interface (BMI) formats, it is recommended to use clangd-based LSP plugins in your IDE/editor. It may be possible to build this project with another toolchain, but I cannot provide any guarantee.

To build `bellit`:
```shell
cmake --preset default
cmake --build --preset default
```

Then you will have an executable `build/bellit` not depending on any 3rd-party dynamic library. Install it to anywhere you prefer.
