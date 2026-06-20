#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <filesystem>

#include <cxxopts.hpp>

import process;
import notification;
import environment;

using std::string;
using std::string_view;
using std::vector;
namespace fs = std::filesystem;

// clang-format off
constexpr string_view examples = "Examples:\n"
"To notify instantly:\n"
"    bellit -t \"Your title\" -m \"Your message\"\n"
"Send a warning message instead of general notification:\n"
"    bellit -t \"Warning\" -w \"Something went wrong\"\n"
"To notify after a long running task has been accomplished:\n"
"    bellit -t \"Compression\" -m \"Gzip package created.\" -- tar -zcvf large-file.tar.gz /path/to/large/directory\n"
"Warn when the task fails:\n"
"    bellit -t \"Ping\" -m \"Connected\" -w \"Disconnected\" -- ping -c 1 192.168.1.1";
// clang-format on

/**
 * \brief Check session environment to ensure the notification will be sent to
 *      local machine.
 */
void notify(string_view title, string_view message, NotificationType type)
{
    if (is_ssh()) {
        // OSC control sequence can be passed through SSH.
        const string raw_control_seq = osc_777_control_seq(title, message); 
        if (is_tmux()) {
            const string wrapped_seq = wrap_with_dcs(raw_control_seq);
            output_to_tty(wrapped_seq);
        } else {
            output_to_tty(raw_control_seq);
        }
    } else {
        // Use native notification if called locally.
        notify_natively(title, message, type);
    }
}

int run_task_and_notify(
    string_view title, string_view message, string_view warning,
    const vector<string>& command
)
{
    if (command.size() > 0) {
        // A command is passed, run it before notification.
        TaskResult result = run_task(command, fs::current_path(), false);
        if (result.return_code == 0) {
            notify(title, message, NotificationType::Info);
        } else {
            // If the warning is empty, use the message as a fallback.
            if (warning.empty()) {
                warning = message;
            }
            notify(title, warning, NotificationType::Warn);
        }
    } else {
        // No command passed, notify directly.
        if (!warning.empty()) {
            notify(title, warning, NotificationType::Warn);
        } else {
            notify(title, message, NotificationType::Info);
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    cxxopts::Options options(
        "bellit", "Notify the user instantly or after a task has been accomplished."
    );
    // clang-format off
    options.add_options()
    (
        "t,title",
        "Title of the notification",
        cxxopts::value<string>(),
        "TITLE"
    )
    (
        "m,message",
        "Message to send",
        cxxopts::value<string>()->default_value(""),
        "MESSAGE"
    )
    (
        "w,warning",
        "Message to send when the task fails. If not set, MESSAGE (specified by -m/--message) will be used.",
        cxxopts::value<string>()->default_value(""),
        "WARNING"
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

    cxxopts::ParseResult args;
    bool                 show_help;
    int                  return_code = 0;
    auto                 show_error  = [](const char* error_message) -> void {
        std::cerr << "Error: " << error_message << std::endl;
        std::cerr << "See the following help message for correct usage:\n" << std::endl;
    };
    try {
        args      = options.parse(argc, argv);
        show_help = args.contains("help");
    } catch (const cxxopts::exceptions::exception& e) {
        show_help = true;
        show_error(e.what());
        return_code = 1;
    }

    if (!show_help) {
        try {
            const string         title       = args["title"].as<string>();
            const string         message     = args["message"].as<string>();
            const string         warning = args["warning"].as<string>();
            const vector<string> command     = args["command"].as<vector<string>>();
            return_code           = run_task_and_notify(title, message, warning, command);
        } catch (const cxxopts::exceptions::exception& e) {
            show_help = true;
            show_error(e.what());
            return_code = 1;
        }
    }

    if (show_help) {
        std::cout << options.help() << std::endl;
        std::cout << examples << std::endl;
    }

    return return_code;
}
