/**
 * @file EnvironmentInjector.cpp
 *
 * @author Dan Keenan
 * @date 4/6/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <QtGlobal>
#ifdef Q_OS_WIN
#include "EnvironmentInjectorWin.h"
#elifdef Q_OS_LINUX
#include "EnvironmentInjectorLinux.h"
#endif

namespace qtchooser {

std::unique_ptr<EnvironmentInjector> EnvironmentInjector::get()
{
#ifdef Q_OS_WIN
    return std::make_unique<EnvironmentInjectorWin>();
#elifdef Q_OS_LINUX
    return std::make_unique<EnvironmentInjectorLinux>();
#else
#error "Unsupported platform"
#endif
}

void EnvironmentInjector::addToPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    if (!std::ranges::contains(userPath, path)) {
        userPath.push_back(path);
        setUserPath(userPath);
    }
}

void EnvironmentInjector::removeFromPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    std::erase(userPath, path);
    setUserPath(userPath);
}

} // namespace qtchooser
