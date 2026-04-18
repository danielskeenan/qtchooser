/**
 * @file EnvironmentInjectorLinux.cpp
 *
 * @author Dan Keenan
 * @date 4/12/26
 * @copyright GPL-3.0
 */

#include "EnvironmentInjectorLinux.h"
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/process/environment.hpp>
#include <fstream>
#include <ranges>
#include <regex>
#include <spdlog/spdlog.h>
#include <sstream>
#include <qtchooser/qtchooser_config.h>

namespace qtchooser {

constexpr auto kProfileMarker = "# Added by qtchooser";
constexpr auto kEnvPathSeparator = ':';
constexpr auto kEnvPath = "PATH";
static const std::regex kReExport(R"/(^\s*export (\w+)="?(.+?)"?$)/");
static const std::regex kRePathRef(R"/(:\${PATH}|\${PATH}:)/");

std::filesystem::path bashProfilePath()
{
    return boost::process::environment::home() / ".bash_profile";
}

EnvironmentInjectorLinux::EnvironmentInjectorLinux() : EnvironmentInjector()
{
    std::ifstream bashProfile(bashProfilePath());
    if (!bashProfile.is_open()) {
        SPDLOG_WARN("Could not open .bash_profile");
        return;
    }

    std::string line;

    // Seek to our section of the file.
    while (std::getline(bashProfile, line)) {
        if (line == kProfileMarker) {
            break;
        }
    }

    // Parse the exports.
    while (std::getline(bashProfile, line)) {
        boost::algorithm::trim(line);
        if (line.empty()) {
            // End of our section.
            break;
        }
        std::smatch match;
        if (!std::regex_match(line, match, kReExport)) {
            break;
        }
        const auto var = match.str(1);
        auto val = match.str(2);
        if (var == kEnvPath) {
            // Remove reference to existing PATH var.
            val = std::regex_replace(val, kRePathRef, "");
        }
        env_[var] = val;
    }
    originalEnv_ = env_;
}

void EnvironmentInjectorLinux::setEnv(const std::string &var, const std::string &val)
{
    env_[var] = val;
}

/**
 * Helper to run cleanup tasks when the object goes out of scope.
 *
 * Tasks are run in reverse order from when they are added.
 */
class Cleaner
{
public:
    using Task = std::function<void()>;

    ~Cleaner()
    {
        for (const auto &task : tasks_ | std::views::reverse) {
            task();
        }
    }

    void addTask(const Task &task) { tasks_.push_back(task); }

private:
    std::vector<Task> tasks_;
};

bool EnvironmentInjectorLinux::commit()
{
    if (originalEnv_ == env_) {
        return false;
    }

    Cleaner cleaner;

    std::ifstream bashProfile(bashProfilePath());
    if (!bashProfile.is_open()) {
        SPDLOG_WARN("Could not open .bash_profile.");
        return false;
    }

    const auto newBashProfilePath = std::filesystem::temp_directory_path() / config::kProjectName
                                    / bashProfilePath().filename();
    if (!std::filesystem::is_directory(newBashProfilePath.parent_path())) {
        std::filesystem::create_directories(newBashProfilePath.parent_path());
        cleaner.addTask(
            [&newBashProfilePath]() { std::filesystem::remove(newBashProfilePath.parent_path()); });
    }
    std::ofstream newBashProfile(newBashProfilePath);
    cleaner.addTask([&newBashProfilePath]() { std::filesystem::remove(newBashProfilePath); });
    if (!newBashProfile.is_open()) {
        SPDLOG_ERROR("Could not open temp .bash_profile for writing.");
        return false;
    }

    std::string line;

    // Seek to our section of the file.
    while (std::getline(bashProfile, line)) {
        if (boost::algorithm::trim_copy(line) == kProfileMarker) {
            break;
        }
        newBashProfile << line << '\n';
    }

    // Write our exports.
    newBashProfile << kProfileMarker << '\n';
    for (const auto &[var, val] : env_) {
        auto exportVal = val;
        if (var == kEnvPath) {
            // Put our PATH before others.
            exportVal.append(std::format("{}${{{}}}", kEnvPathSeparator, kEnvPath));
        }
        newBashProfile << "export " << var << "=\"" << exportVal << "\"\n";
    }

    // Seek past original exports.
    while (std::getline(bashProfile, line)) {
        if (boost::algorithm::trim_copy(line).empty()) {
            // End of our section.
            newBashProfile << line << '\n';
            break;
        }
    }

    // Write remainder of file.
    while (std::getline(bashProfile, line)) {
        newBashProfile << line << '\n';
    }

    newBashProfile.flush();
    bashProfile.close();
    std::filesystem::copy_file(
        newBashProfilePath, bashProfilePath(), std::filesystem::copy_options::overwrite_existing);
    originalEnv_ = env_;

    return true;
}

std::vector<std::filesystem::path> EnvironmentInjectorLinux::getUserPath()
{
    std::vector<std::filesystem::path> userPath;

    if (env_.contains(kEnvPath)) {
        for (const auto &part : std::views::split(env_[kEnvPath], kEnvPathSeparator)) {
            userPath.emplace_back(std::string_view(part));
        }
    }

    return userPath;
}

void EnvironmentInjectorLinux::setUserPath(const std::vector<std::filesystem::path> &userPath)
{
    std::stringstream ss;
    for (auto it = userPath.cbegin(); it != userPath.cend(); ++it) {
        ss << it->string();
        if (it + 1 != userPath.cend()) {
            ss << kEnvPathSeparator;
        }
    }
    env_[kEnvPath] = ss.str();
}
} // namespace qtchooser
