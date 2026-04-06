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

EnvironmentInjector::EnvironmentInjector() {}

bool EnvironmentInjector::commit()
{
    SPDLOG_WARN("This platform does not support environment injection.");
    return false;
}

QStringList EnvironmentInjector::getUserPath()
{
    SPDLOG_WARN("This platform does not support environment injection.");
    return {};
}

void EnvironmentInjector::setUserPath(const QStringList &path)
{
    SPDLOG_WARN("This platform does not support environment injection.");
}

} // namespace qtchooser
