/**
 * @file QtChooser.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtChooser.h"
#include "CurrentChosen.h"
#include "EnvironmentInjector.h"
#include <QStandardPaths>

namespace qtchooser {

void QtChooser::choose(const QtInfo &info)
{
    bool envChanged = false;
    EnvironmentInjector env;

    CurrentChosen currentChosen;
    if (currentChosen.isValid()) {
        envChanged |= env.removeFromPath(currentChosen.binDir());
    }
    envChanged |= env.setEnv("QT_ROOT_DIR", QString::fromStdString(info.prefix().string()));
    envChanged |= env.setEnv(
        QString("Qt%1_DIR").arg(info.version().majorVersion()),
        QString::fromStdString(info.cmakePackageDir().string()));
    envChanged |= env.setEnv("Qt_DIR", QString::fromStdString(info.cmakePackageDir().string()));
    envChanged |= env.addToPath(info.binDir());
    env.commit();
    if (envChanged) {
        Q_EMIT(envVarsChanged());
    }
    currentChosen.setInfo(info);
}

} // namespace qtchooser
