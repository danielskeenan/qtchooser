/**
 * @file EnvironmentInjector_none.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <spdlog/spdlog.h>

namespace qtchooser {

bool setEnv(const QString &var, const QString &val)
{
    SPDLOG_WARN("Could not persist env var {}={}", var, val);
}

QStringList getPath()
{
    SPDLOG_WARN("Unknown PATH on this platform.");
    return {};
}

bool addToPath(const std::filesystem::path &path)
{
    SPDLOG_WARN("Unknown PATH on this platform.");
    return false;
}

bool removeFromPath(const std::filesystem::path &path)
{
    SPDLOG_WARN("Unknown PATH on this platform.");
    return false;
}

} // namespace qtchooser
