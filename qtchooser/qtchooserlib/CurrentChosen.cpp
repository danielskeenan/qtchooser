/**
 * @file CurrentChosen.cpp
 *
 * @author Dan Keenan
 * @date 4/6/2026
 * @copyright GPL-3.0
 */

#include "CurrentChosen.h"
#include <QSettings>

namespace qtchooser {

constexpr auto kSettingCurrentPrefix = "lib/currentPrefix";
constexpr auto kSettingCurrentBinDir = "lib/currentBinDir";

CurrentChosen::CurrentChosen()
{
    QSettings settings;
    prefix_ = settings.value(kSettingCurrentPrefix).toString().toStdString();
    for (const auto &binDir : settings.value(kSettingCurrentBinDir).toStringList()) {
        binDirs_.emplace_back(binDir.toStdString());
    }
}

void CurrentChosen::setInfo(const QtInfo &info)
{
    prefix_ = info.prefix();
    binDirs_ = info.binDirs();

    QSettings settings;
    settings.setValue(kSettingCurrentPrefix, QString::fromStdString(prefix_.string()));
    QStringList binDirList;
    for (const auto &binDir : binDirs_) {
        binDirList.emplace_back(QString::fromStdString(binDir.string()));
    }
    settings.setValue(kSettingCurrentBinDir, binDirList);
}

bool CurrentChosen::isValid() const
{
    return !(prefix_.empty() || binDirs_.empty());
}

} // namespace qtchooser
