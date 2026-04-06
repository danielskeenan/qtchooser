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

    CurrentChosen currentChosen;
    if (currentChosen.isValid()) {
        envChanged |= removeFromPath(currentChosen.binDir());
    }
    envChanged |= setEnv("QT_ROOT_DIR", QString::fromStdString(info.prefix().string()));
    envChanged |= setEnv(
        QString("Qt%1_DIR").arg(info.version().majorVersion()),
        QString::fromStdString(info.cmakePackageDir().string()));
    envChanged |= setEnv("Qt_DIR", QString::fromStdString(info.cmakePackageDir().string()));
    envChanged |= addToPath(info.binDir());
    if (envChanged) {
        Q_EMIT(envVarsChanged());
    }
    currentChosen.setInfo(info);
}

} // namespace qtchooser
