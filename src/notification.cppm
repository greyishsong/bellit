module;

#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <format>

export module notification;
import process;

using std::format;
using std::string;
using std::string_view;
using std::vector;
namespace fs = std::filesystem;
using namespace std::string_view_literals;

export enum class NotificationType
{
    Info,
    Warn
};

constexpr string_view get_icon_name(NotificationType type)
{
#ifdef __linux__
    return ""sv;
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

void notify_linux(string_view title, string_view message)
{
    vector<string> cmd = {"notify-send", "-a", "Bell It"};
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
        "$balmsg.BalloonTipTitle = \\\"{}\\\";"
        "$balmsg.BalloonTipText = \\\"{}\\\";"
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
    notify_linux(title, message);
#elif defined(_WIN32)
    notify_windows(title, message, type);
#elif defined(__APPLE__)
    notify_macos(title, message);
#else
    throw std::runtime_error("Unsupported platform");
#endif
}
