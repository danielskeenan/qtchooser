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
    auto env = EnvironmentInjector::get();

    CurrentChosen currentChosen;
    if (currentChosen.isValid()) {
        for (const auto &binDir : currentChosen.binDirs()) {
            env->removeFromPath(binDir);
        }
    }
    env->setEnv("QT_ROOT_DIR", info.prefix().string());
    env->setEnv(
        std::format("Qt{}_DIR", info.version().majorVersion()), info.cmakePackageDir().string());
    env->setEnv("Qt_DIR", info.cmakePackageDir().string());
    for (const auto &binDir : info.binDirs()) {
        env->addToPath(binDir);
    }
    if (env->commit()) {
        Q_EMIT(envVarsChanged());
    }
    currentChosen.setInfo(info);
}

} // namespace qtchooser
