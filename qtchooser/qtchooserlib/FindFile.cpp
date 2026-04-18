/**
 * @file FindFile.cpp
 *
 * @author Dan Keenan
 * @date 4/6/26
 * @copyright GPL-3.0
 */

#include "FindFile.h"
#include <boost/process/environment.hpp>

namespace qtchooser {

std::optional<std::filesystem::path> findFile(
    const std::string &name, const std::filesystem::path &searchDir)
{
    for (const auto &entry : std::filesystem::recursive_directory_iterator(
             searchDir, std::filesystem::directory_options::skip_permission_denied)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().filename() == name) {
            return entry.path();
        }
    }

    return {};
}

std::optional<std::filesystem::path> findProgram(
    const std::vector<std::string> &names,
    const std::filesystem::path &prefix,
    const std::vector<std::filesystem::path> &extraPaths)
{
    // Setup search environment.
    boost::process::environment::value pathVar((prefix / "bin").string());
    for (const auto &extraPath : extraPaths) {
        pathVar.push_back(extraPath.string());
    }
    std::unordered_map<boost::process::environment::key, boost::process::environment::value> env{
        {"PATH", pathVar}};

    for (const auto &name : names) {
        auto found = boost::process::environment::find_executable(name, env);
        if (!found.empty()) {
            return found;
        }
    }

    return {};
}

} // namespace qtchooser
