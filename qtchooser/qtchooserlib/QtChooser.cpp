/**
 * @file QtChooser.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtChooser.h"
#include "EnvironmentInjector.h"
#include <format>

namespace qtchooser {

void QtChooser::choose(const QtInfo &info)
{
    linkBinaries(info.binDir(), binDir_);

    bool envChanged = false;
    envChanged |= setEnv("QT_ROOT_DIR", info.prefix().string());
    envChanged |= setEnv(
        std::format("Qt{}_DIR", info.version().majorVersion()), info.cmakePackageDir().string());
    envChanged |= setEnv("Qt_DIR", info.cmakePackageDir().string());
    if (envChanged) {
        Q_EMIT(envVarsChanged());
    }
}

} // namespace qtchooser
