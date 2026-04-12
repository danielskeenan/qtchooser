/**
 * @file EnvironmentInjectorWin.cpp
 *
 * @author Dan Keenan
 * @date 4/12/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjectorWin.h"
#include <optional>
#include <ranges>
#include <spdlog/spdlog.h>
#include <windows.h>
#include <winreg/WinReg.hpp>
#include <winrt/base.h>

namespace qtchooser {

constexpr auto kRegEnvKey = L"Environment";
constexpr auto kEnvPath = "Path";
constexpr auto kWEnvPathSeparator = L';';
constexpr auto kEnvPathSeparator = ';';

class RegEnvVar
{
public:
    explicit RegEnvVar(winreg::RegKey &regEnv, const std::string &var) :
        regEnv_(regEnv), var_(winrt::to_hstring(var))
    {
        try {
            if (regEnv.ContainsValue(var_)) {
                // Environment vars should only be strings, so only handle REG_SZ and REG_EXPAND_SZ
                varType_ = regEnv.QueryValueType(var_);
            }
        } catch (const winreg::RegException &e) {
            SPDLOG_ERROR("Failed to get environment ({}) {}", e.what(), var);
        }
    }

    /** Get the current value of the variable. */
    std::optional<std::string> get()
    {
        try {
            if (regEnv_.ContainsValue(var_)) {
                switch (varType_) {
                case REG_SZ:
                    return winrt::to_string(regEnv_.GetStringValue(var_));
                case REG_EXPAND_SZ:
                    return winrt::to_string(regEnv_.GetExpandStringValue(var_));
                default:
                    SPDLOG_CRITICAL("Unexpected registry type: {}", varType_);
                    return {};
                }
            }
        } catch (const winreg::RegException &e) {
            SPDLOG_ERROR("Failed to get environment ({}) {}", e.what(), winrt::to_string(var_));
        }

        return {};
    }

    /**
     * Get the current value of the variable.
     *
     * @return TRUE if the value changed.
     */
    bool set(const std::string &val)
    {
        const std::wstring newVal(winrt::to_hstring(val));
        try {
            const auto oldVal = get();
            if (varType_ == REG_SZ) {
                regEnv_.SetStringValue(var_, newVal);
            } else if (varType_ == REG_EXPAND_SZ) {
                regEnv_.SetExpandStringValue(var_, newVal);
            } else {
                SPDLOG_CRITICAL("Unexpected registry type: {}", varType_);
            }
            return oldVal != val;
        } catch (const winreg::RegException &e) {
            SPDLOG_ERROR(
                "Failed to set environment ({}) {}={}", e.what(), winrt::to_string(var_), val);
        }
        return false;
    }

private:
    winreg::RegKey &regEnv_;
    std::wstring var_;
    // Default to unexpended string.
    DWORD varType_ = REG_SZ;
};

winreg::RegKey getRegEnv()
{
    return winreg::RegKey(HKEY_CURRENT_USER, kRegEnvKey);
}

void EnvironmentInjectorWin::setEnv(const std::string &var, const std::string &val)
{
    env_[var] = val;
}

bool EnvironmentInjectorWin::commit()
{
    bool changed = false;
    auto regEnv = getRegEnv();
    for (const auto &[var, val] : env_) {
        RegEnvVar regVar(regEnv, var);
        changed |= regVar.set(val);
    }
    if (changed) {
        // https://learn.microsoft.com/en-us/windows/win32/procthread/environment-variables
        PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, LPARAM(&kRegEnvKey));
    }
    return changed;
}

std::vector<std::filesystem::path> EnvironmentInjectorWin::getUserPath()
{
    std::vector<std::filesystem::path> userPath;

    if (!env_.contains(kEnvPath)) {
        // Load from the registry.
        auto regEnv = getRegEnv();
        RegEnvVar regPath(regEnv, kEnvPath);
        const auto regPathVal = regPath.get();
        if (!regPathVal.has_value()) {
            return userPath;
        }
        env_.emplace(kEnvPath, *regPathVal);
    }

    const std::wstring userPathStr(winrt::to_hstring(env_.at(kEnvPath)));
    for (const auto &part : std::views::split(userPathStr, kWEnvPathSeparator)) {
        const std::wstring_view str(part);
        if (str.empty()) {
            continue;
        }
        auto path = userPath.emplace_back(winrt::to_string(str));
        path.make_preferred();
    }

    return userPath;
}

void EnvironmentInjectorWin::setUserPath(const std::vector<std::filesystem::path> &userPath)
{
    std::vector<std::string> pathStrs;
    for (const auto &path : userPath) {
        auto preferredPath = path;
        preferredPath.make_preferred();
        pathStrs.emplace_back(preferredPath.string());
    }
    env_[kEnvPath]
        = std::string(std::from_range, std::views::join_with(pathStrs, kEnvPathSeparator));
}

} // namespace qtchooser