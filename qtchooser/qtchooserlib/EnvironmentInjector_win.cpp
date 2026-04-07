/**
 * @file EnvironmentInjector_win.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <Windows.h>
#include <QSettings>

namespace qtchooser {

constexpr auto kEnvRegKey = R"(HKEY_CURRENT_USER\Environment)";
constexpr auto kEnvPath = "Path";
constexpr auto kEnvPathSeparator = ';';

EnvironmentInjector::Environment getUserEnvironment()
{
    QSettings settings(kEnvRegKey, QSettings::NativeFormat);
    EnvironmentInjector::Environment env;
    for (const auto &var : settings.allKeys()) {
        env[var] = settings.value(var).toString();
    }

    return env;
}

EnvironmentInjector::EnvironmentInjector() : originalEnv_(getUserEnvironment()), env_(originalEnv_)
{}

bool EnvironmentInjector::commit()
{
    //TODO: Use native registry functions, using QSettings clobbers the registry!!!!!!!
    QSettings settings(kEnvRegKey, QSettings::NativeFormat);
    settings.clear();
    for (const auto &[var, val] : env_.asKeyValueRange()) {
        settings.setValue(var, val);
    }
    settings.sync();

    const bool changed = originalEnv_ != env_;
    originalEnv_ = env_;
    // https://learn.microsoft.com/en-us/windows/win32/procthread/environment-variables
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) std::addressof("Environment"));
    return changed;
}

QStringList EnvironmentInjector::getUserPath()
{
    return env_[kEnvPath].split(kEnvPathSeparator, Qt::SkipEmptyParts);
}

void EnvironmentInjector::setUserPath(const QStringList &path)
{
    setEnv(kEnvPath, path.join(kEnvPathSeparator));
}

} // namespace qtchooser
