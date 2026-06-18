module;

#include <cstdlib>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

export module environment;

using std::getenv;
using std::size_t;
using std::string;
using std::string_view;
using std::vector;
using namespace std::string_view_literals;

/**
 * Detect SSH session through `SSH_TTY` environment variable.
 */
export bool is_ssh()
{
    const char* ssh_tty = getenv("SSH_TTY");
    return ssh_tty != nullptr;
}

/**
 * Detect tmux session through `TMUX` environment variable.
 */
export bool is_tmux()
{
    const char* tmux = getenv("TMUX");
    return tmux != nullptr;
}

#ifdef __linux__
/**
 * \brief Supported Linux desktop environments.
 *
 * Supported desktop environments are listed in the same order as FreeDesktop spec:
 * https://specifications.freedesktop.org/menu/latest/onlyshowin-registry.html
 *
 * Each enum value (except `Unknown`) is named as the "OnlyShownIn Value" in the
 * spec.
 */
export enum class DesktopEnvironment
{
    GNOME,
    KDE,
    XFCE,
    Cinnamon,
    Unknown
};

/**
 * Get the current desktop environment according to FreeDesktop standardized
 * environment variable `XDG_CURRENT_DESKTOP`.
 * \returns The current desktop environment.
 */
export DesktopEnvironment get_desktop_environment()
{
    const char* xdg_current_desktop = getenv("XDG_CURRENT_DESKTOP");
    if (xdg_current_desktop == nullptr) {
        return DesktopEnvironment::Unknown;
    }
    // The environment variable XDG_CURRENT_DESKTOP is a colon separated list
    // containing current desktop identifies.
    // See: https://wiki.archlinux.org/title/Environment_variables#Desktop_environment_detection
    const string   current_desktops(xdg_current_desktop);
    vector<string> id_list;
    size_t         start = 0;
    for (size_t i = 0; i < current_desktops.size(); ++i) {
        if (current_desktops[i] == ':') {
            id_list.emplace_back(current_desktops.substr(start, i - start));
            start = i + 1;
        }
    }
    id_list.emplace_back(current_desktops.substr(start));
    auto contain_id = [&id_list](string_view id) -> bool {
        return std::find(id_list.begin(), id_list.end(), id) != id_list.end();
    };
    if (contain_id("GNOME")) {
        return DesktopEnvironment::GNOME;
    } else if (contain_id("KDE")) {
        return DesktopEnvironment::KDE;
    } else if (contain_id("XFCE")) {
        return DesktopEnvironment::XFCE;
    } else if (contain_id("Cinnamon")) {
        return DesktopEnvironment::Cinnamon;
    }
    return DesktopEnvironment::Unknown;
}
#endif
