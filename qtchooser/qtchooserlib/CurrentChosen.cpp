/**
 * @file CurrentChosen.cpp
 *
 * @author Dan Keenan
 * @date 4/6/2026
 * @copyright GPL-3.0
 */

#include "CurrentChosen.h"
#include <boost/process/environment.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace qtchooser {

constexpr auto kSettingPrefix = "currentPrefix";
constexpr auto kSettingBinDirs = "currentBinDirs";

std::filesystem::path currentChosenPath()
{
    return boost::process::environment::home() / ".qtchooser";
}

CurrentChosen::CurrentChosen()
{
    using nlohmann::json;

    std::ifstream settingsFile(currentChosenPath());
    if (!settingsFile.is_open()) {
        // No settings stored. Try to guess from system environment.
        std::array<std::string, 3> qtpathsNames{"qtpaths6", "qtpaths", "qtpaths5"};
        for (auto &name : qtpathsNames) {
#ifdef OS_WINDOWS
            name.append(".exe");
#endif
            const auto qtpaths = boost::process::environment::find_executable(name);
            if (!qtpaths.empty()) {
                // Found qtpaths in PATH; ask it about installation.
                const auto info = QtInfo::getFromQtPaths(qtpaths);
                if (info.has_value()) {
                    setInfo(*info);
                }
            }
        }
        return;
    }

    try {
        const auto settings = json::parse(settingsFile);

        if (settings.contains(kSettingPrefix) && settings[kSettingPrefix].is_string()) {
            prefix_ = settings[kSettingPrefix].get<std::string>();
        }

        if (settings.contains(kSettingBinDirs) && settings[kSettingBinDirs].is_array()) {
            for (const auto &binDir : settings[kSettingBinDirs]) {
                if (binDir.is_string()) {
                    binDirs_.emplace_back(binDir.get<std::string>());
                }
            }
        }
    } catch (const json::parse_error &e) {
        SPDLOG_ERROR("Error parsing settings file: {}", e.what());
        return;
    } catch (const json::exception &e) {
        SPDLOG_ERROR("Error loading settings file: {}", e.what());
        return;
    }
}

void CurrentChosen::setInfo(const QtInfo &info)
{
    using nlohmann::json;

    prefix_ = info.prefix();
    binDirs_ = info.binDirs();

    json settings;

    settings[kSettingPrefix] = prefix_.string();

    std::vector<std::string> binDirs;
    for (const auto &binDir : binDirs_) {
        binDirs.emplace_back(binDir.string());
    }
    settings[kSettingBinDirs] = binDirs;

    std::ofstream settingsFile(currentChosenPath());
    if (!settingsFile.is_open()) {
        SPDLOG_ERROR("Could not save current chosen to disk");
        return;
    }
    settingsFile << settings;
}

bool CurrentChosen::isValid() const
{
    return !(prefix_.empty() || binDirs_.empty());
}

} // namespace qtchooser
