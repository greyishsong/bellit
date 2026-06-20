module;

#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <format>
#include <iostream>
#include <fstream>

export module notification;
import process;
import environment;

using std::format;
using std::string;
using std::string_view;
using std::vector;
namespace fs = std::filesystem;
using namespace std::string_view_literals;

// -------------------- Native notification implementations --------------------

export enum class NotificationType
{
    Info,
    Warn
};

string_view get_icon_name(NotificationType type)
{
#ifdef __linux__
    switch (type) {
    case NotificationType::Info: return "dialog-information"sv;
    case NotificationType::Warn: return "dialog-warning"sv;
    }
#elif defined(_WIN32)
    switch (type) {
    case NotificationType::Info: return "Info"sv;
    case NotificationType::Warn: return "Warning"sv;
    default: return "Info"sv;
    }
#elif defined(__APPLE__)
    return ""sv;
#endif
}

#ifdef __linux__

void notify_linux(string_view title, string_view message, NotificationType type)
{
    vector<string>    cmd       = {"notify-send", "--app-name", "Bell It"};
    const string_view icon_name = get_icon_name(type);
    if (!icon_name.empty()) {
        cmd.emplace_back("--icon");
        cmd.emplace_back(icon_name);
    }
    cmd.emplace_back(title);
    cmd.emplace_back(message);
    run_task(cmd, fs::current_path(), false);
}

#endif // __linux__

#ifdef _WIN32

void notify_windows(
    string_view title, string_view message, NotificationType type = NotificationType::Info
)
{
    // Create a toast notification through PowerShell
    const string_view icon_name = get_icon_name(type);
    const string      script    = format(
        "Add-Type -AssemblyName System.Windows.Forms;"
        "$global:balmsg = New-Object System.Windows.Forms.NotifyIcon;"
        "$balmsg.BalloonTipTitle = \"{}\";"
        "$balmsg.BalloonTipText = \"{}\";"
        "$path = (Get-Process -id $pid).Path;"
        "$balmsg.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($path);"
        "$balmsg.BalloonTipIcon = [System.Windows.Forms.ToolTipIcon]::{};"
        "$balmsg.Visible = $true;"
        "$balmsg.ShowBalloonTip(20000)",
        title, message, icon_name
    );
    const vector<string> cmd = {
        "powershell.exe", "-NoProfile", "-ExecutionPolicy", "ByPass", script
    };
    run_task(cmd, fs::current_path(), false);
}

#endif // _WIN32

#ifdef __APPLE__

void notify_macos(string_view title, string_view message)
{
    const string script = format("display notification \"{}\" with title \"{}\"", message, title);
    const vector<string> cmd = {"osascript", "-e", script};
    run_task(cmd, fs::current_path(), false);
}

#endif // __APPLE__

export void notify_natively(
    string_view title, string_view message,
    [[maybe_unused]] NotificationType type = NotificationType::Info
)
{
#ifdef __linux__
    notify_linux(title, message, type);
#elif defined(_WIN32)
    notify_windows(title, message, type);
#elif defined(__APPLE__)
    notify_macos(title, message);
#else
    throw std::runtime_error("Unsupported platform");
#endif
}

// -------------------- OSC notification implementations --------------------

export void output_to_tty(string_view content)
{
    using std::ofstream;
#ifdef _WIN32
    ofstream tty("CONOUT$", std::ios::out);
#else
    ofstream tty("/dev/tty", std::ios::out);
#endif
    tty << content;
}

export string osc_9_control_seq(string_view title)
{
    return format("\x1b]9;{}\x07", title);
}

export string osc_777_control_seq(string_view title, string_view message)
{
    return format("\x1b]777;notify;{};{}\x07", title, message);
}

/*!
 * \brief Wrap a control sequence with DCS (required by tmux).
 * \param control_seq The raw control sequence **not** ending with ST (ESC \).
 * \returns Wrapped sequence which will be parsed by tmux.
 */
export string wrap_with_dcs(string_view control_seq)
{
    return format("\x1bPtmux;\x1b{}\x1b\\", control_seq);
}
