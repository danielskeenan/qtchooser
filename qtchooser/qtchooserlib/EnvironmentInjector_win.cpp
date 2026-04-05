/**
 * @file EnvironmentInjector_win.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
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

bool setEnv(const std::string &var, const std::string &val)
{
    auto settings = userEnvironment();
    const auto oldValue = settings.value(var);
    const auto newValue = QString::fromStdString(val);
    settings.setValue(var, newValue);
    return oldValue != newValue;
}

QStringList getPath()
{
    const auto env = QProcessEnvironment::systemEnvironment();
    const auto envPath = env.value(kEnvPath);
    return envPath.split(kEnvPathSeparator, Qt::SkipEmptyParts);
}

bool addToPath(const std::filesystem::path &path)
{
    const auto currentPath = getPath();
    if (currentPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    auto settings = userEnvironment();
    auto userPath = settings.value(kEnvPath).toString().split(kEnvPathSeparator, Qt::SkipEmptyParts);
    userPath.push_back(QString::fromStdString(path.string()));
    settings.setValue(kEnvPath, userPath.join(kEnvPathSeparator));

    return true;
}

bool removeFromPath(const std::filesystem::path &path)
{
    const auto currentPath = getPath();
    if (!currentPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    // Can't remove if it's in the system path, so need to determine that first.
    auto settings = userEnvironment();
    auto userPath = settings.value(kEnvPath).toString().split(kEnvPathSeparator, Qt::SkipEmptyParts);
    if (!userPath.contains(QString::fromStdString(path.string()))) {
        SPDLOG_WARN("Cannot remove {} from the system path.", path.string());
        return false;
    }

    userPath.removeAll(QString::fromStdString(path.string()));
    settings.setValue(kEnvPath, userPath.join(kEnvPathSeparator));
    return true;
}

} // namespace qtchooser
