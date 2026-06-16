module;

#include <cstdlib>

export module environment;

using std::getenv;

export bool is_ssh()
{
    const char* ssh_tty = getenv("SSH_TTY");
    return ssh_tty != nullptr;
}

export bool is_tmux()
{
    const char* tmux = getenv("TMUX");
    return tmux != nullptr;
}
