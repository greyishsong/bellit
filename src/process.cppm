module;

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <numeric>

#include <process.hpp>

export module process;

using std::function;
using std::size_t;
using std::string;
using std::vector;
namespace fs = std::filesystem;

/**
 * Execution result of a sub process.
 */
export struct TaskResult
{
    int    return_code;
    /** Content of stdout. Could be empty if the output is not captured. */
    string stdout_content;
    /** Content of stderr. Could be empty if the output is not captured. */
    string stderr_content;
};

const TinyProcessLib::Config TASK_CONFIG = {
    .show_window = TinyProcessLib::Config::ShowWindow::hide
};

/**
 * \brief Create a subprocess to execute a task.
 *
 * In this project, `run_task` is not a general tool but only used in 2 cases:
 * - Call the native notification tool to send a message.
 * - Execute the command the user wants to execute.
 *
 * All environment variables will be inherited from the user's shell, the only
 * configuration we provide is cwd and whether to capture output.
 *
 * \param command The command to run and its arguments.
 * \param cwd The working directory of the command.
 * \param capture_output Whether to capture stdout and stderr of the subprocess.
 *      If set to `false`, the subprocess will directly output to the terminal.
 * \returns Result of the command.
 */
export TaskResult run_task(vector<string> command, const fs::path& cwd, bool capture_output = false)
{
    // Store output into a vector because read_stdout/read_stderr could be called multiple times.
    vector<string> stdout_slices, stderr_slices;
    auto           read_stdout = [&stdout_slices](const char* bytes, size_t n) -> void {
        stdout_slices.emplace_back(bytes, n);
    };
    auto read_stderr = [&stderr_slices](const char* bytes, size_t n) -> void {
        stderr_slices.emplace_back(bytes, n);
    };
    function<void(const char*, size_t)> read_stdout_func = nullptr;
    function<void(const char*, size_t)> read_stderr_func = nullptr;
    if (capture_output) {
        read_stdout_func = read_stdout;
        read_stderr_func = read_stderr;
    }
    // Start a sub-process here.
    TinyProcessLib::Process process(
        command, cwd.generic_string(), read_stdout_func, read_stderr_func, false, TASK_CONFIG
    );

    TaskResult result;
    result.return_code = process.get_exit_status();
    // If stdout and stderr are captured, join the slices to one string.
    if (capture_output) {
        auto accumulate_length = [](const size_t& s, const string& str) -> size_t {
            return s + str.size();
        };
        auto join_slices = [accumulate_length](const vector<string>& slices) -> string {
            const size_t total_length =
                std::accumulate(slices.begin(), slices.end(), 0, accumulate_length);
            string content;
            content.reserve(total_length);
            for (const string& slice: slices) {
                std::ranges::copy(slice.begin(), slice.end(), std::back_inserter(content));
            }
            return content;
        };

        result.stdout_content = join_slices(stdout_slices);
        result.stderr_content = join_slices(stderr_slices);
    }

    return result;
}
