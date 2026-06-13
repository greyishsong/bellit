#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <filesystem>

#include <cxxopts.hpp>

import process;
import notification;

using std::string;
using std::string_view;
using std::vector;
namespace fs = std::filesystem;

constexpr string_view examples = "Examples:\n"
"To notify instantly:\n"
"    bellit -t \"Your title\" -m \"Your message\"\n"
"To notify after a long running task has been accomplished:\n"
"    bellit -t \"Compression\" -m \"Gzip package created.\" -- tar -zcvf large-file.tar.gz /path/to/large/directory\n"
"Warn when the task fails:\n"
"    bellit -t \"Ping\" -m \"Connected\" -w \"Disconnected\" -- ping -c 1 192.168.1.1";

int main(int argc, char* argv[])
{
    cxxopts::Options options("bellit", "Notify the user instantly or after a task has been accomplished.");
    // clang-format off
    options.add_options()
    (
        "t,title",
        "Title of the notification",
        cxxopts::value<string>()
    )
    (
        "m,message",
        "Message to send",
        cxxopts::value<string>()
    )
    (
        "w,warning",
        "Message to send when the task fails. If not set, message (-m/--message) will be used.",
        cxxopts::value<string>()->default_value("")
    )
    (
        "h,help",
        "Print usage",
        cxxopts::value<bool>()
    )
    (
        "command",
        "Command to run and its arguments. When not passed, a notification will be sent directly",
        cxxopts::value<vector<string>>()->default_value("")
    );
    // clang-format on
    options.positional_help("[-- COMMAND]...");
    options.set_width(80);
    options.parse_positional("command");

    auto args = options.parse(argc, argv);
    if (args["help"].as<bool>()) {
        std::cout << options.help() << std::endl;;
        std::cout << examples << std::endl;
        return 0;
    }

    const string title = args["title"].as<string>();
    const string message = args["message"].as<string>();
    const string raw_warning = args["warning"].as<string>();
    const string& warning = raw_warning.empty()? message : raw_warning;
    const vector<string> command = args["command"].as<vector<string>>();
    if (command.size() > 0) {
        TaskResult result = run_task(command, fs::current_path(), false);
        if (result.return_code == 0) {
            notify_natively(title, message, NotificationType::Info);
        } else {
            notify_natively(title, warning, NotificationType::Warn);
        }
    } else {
        if (!args["warning"].as<string>().empty()) {
            std::cerr << "Error: Warning message (-w/--warning) must be passed together with a command" << std::endl;
            return 1;
        }
        notify_natively(title, message, NotificationType::Info);
    }

    return 0;
}
