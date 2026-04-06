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
    envChanged |= setEnv("QT_ROOT_DIR", QString::fromStdString(info.prefix().string()));
    envChanged |= setEnv(
        QString("Qt%1_DIR").arg(info.version().majorVersion()),
        QString::fromStdString(info.cmakePackageDir().string()));
    envChanged |= setEnv("Qt_DIR", QString::fromStdString(info.cmakePackageDir().string()));
    if (envChanged) {
        Q_EMIT(envVarsChanged());
    }
}

} // namespace qtchooser
