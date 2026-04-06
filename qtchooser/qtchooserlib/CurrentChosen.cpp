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
    binDir_ = settings.value(kSettingCurrentBinDir).toString().toStdString();
}

void CurrentChosen::setInfo(const QtInfo &info)
{
    prefix_ = info.prefix();
    binDir_ = info.binDir();

    QSettings settings;
    settings.setValue(kSettingCurrentPrefix, QString::fromStdString(prefix_.string()));
    settings.setValue(kSettingCurrentBinDir, QString::fromStdString(binDir_.string()));
}

bool CurrentChosen::isValid() const
{
    return !(prefix_.empty() || binDir_.empty());
}

} // namespace qtchooser
