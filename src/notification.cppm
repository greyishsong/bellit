module;

#include <string>
#include <string_view>
#include <filesystem>
#include <vector>

export module notification;
import process;

using std::string;
using std::string_view;
using std::vector;
namespace fs = std::filesystem;

#ifdef __linux__

void notify_linux(string_view title, string_view message)
{
    vector<string> cmd = {"notify-send", "-a", "Bell It"};
    cmd.emplace_back(title);
    cmd.emplace_back(message);
    TaskResult result = run_task(cmd, fs::current_path(), false);
}

#endif // __linux__

#ifdef _WIN32

void notify_windows(string_view title, string_view message)
{
}

#endif // _WIN32

#ifdef __APPLE__

void notify_macos(string_view title, string_view message)
{
}

#endif // __APPLE__

export void notify_natively(string title, string message)
{
#ifdef __linux__
    notify_linux(title, message);
#elif defined(_WIN32)
    notify_windows(title, message);
#elif defined(__APPLE__)
    notify_macos(title, message);
#else
    throw std::runtime_error("Unsupported platform");
#endif
}
