/**
 * @file EnvironmentInjector_win.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <Windows.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <QProcessEnvironment>
#include <QSettings>

namespace qtchooser {

constexpr auto kEnvPath = "Path";
constexpr auto kEnvPathSeparator = ';';

QSettings userEnvironment()
{
    return QSettings(R"(HKEY_CURRENT_USER\Environment)", QSettings::NativeFormat);
}

bool setEnv(const QString &var, const QString &val)
{
    auto settings = userEnvironment();
    const auto oldValue = settings.value(var);
    if (oldValue == val) {
        return false;
    }

    settings.setValue(var, val);
    settings.sync();
    // https://learn.microsoft.com/en-us/windows/win32/procthread/environment-variables
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) std::addressof("Environment"));

    return true;
}

QStringList getPath()
{
    const auto env = QProcessEnvironment::systemEnvironment();
    const auto envPath = env.value(kEnvPath);
    return envPath.split(kEnvPathSeparator, Qt::SkipEmptyParts);
}

QStringList getUserPath()
{
    auto settings = userEnvironment();
    return settings.value(kEnvPath).toString().split(kEnvPathSeparator, Qt::SkipEmptyParts);
}

bool setUserPath(const QStringList &path)
{
    return setEnv(kEnvPath, path.join(kEnvPathSeparator));
}

bool addToPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    if (userPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    userPath.push_back(QString::fromStdString(path.string()));
    return setUserPath(userPath);
}

bool removeFromPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    if (!userPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    userPath.removeAll(QString::fromStdString(path.string()));
    return setUserPath(userPath);
}

} // namespace qtchooser
