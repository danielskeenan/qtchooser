/**
 * @file EnvironmentInjector.cpp
 *
 * @author Dan Keenan
 * @date 4/6/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"

namespace qtchooser {

void EnvironmentInjector::setEnv(const QString &var, const QString &val)
{
    env_[var] = val;
}

void EnvironmentInjector::addToPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    userPath.push_back(QString::fromStdString(path.string()));
    setUserPath(userPath);
}

void EnvironmentInjector::removeFromPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    userPath.removeAll(QString::fromStdString(path.string()));
    setUserPath(userPath);
}

}
