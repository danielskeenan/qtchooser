/**
 * @file EnvironmentInjector.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <spdlog/spdlog.h>
#include <QFileInfo>
#include <QSettings>

namespace qtchooser {

static const auto kSettingLastBinaries = "lib/lastBinaries";

void linkBinaries(const std::filesystem::path &from, const std::filesystem::path &to)
{
    // Remove old links.
    QSettings settings;
    auto lastBinaries = settings.value(kSettingLastBinaries).toStringList();
    for (const auto &name : lastBinaries) {
        const auto binPath = to / name.toStdString();
        if (std::filesystem::is_regular_file(binPath) || std::filesystem::is_symlink(binPath)) {
            try {
                std::filesystem::remove(binPath);
                SPDLOG_INFO("Removed old {}", binPath.string());
            } catch (const std::filesystem::filesystem_error &e) {
                SPDLOG_ERROR("Error removing {}: {}", binPath.string(), e.what());
            }
        }
    }

    // Create new links.
    lastBinaries.clear();
    for (const auto &entry : std::filesystem::directory_iterator(from)) {
        const auto path = std::filesystem::canonical(entry.path());
        QFileInfo fileInfo(path);
        if (fileInfo.isExecutable()) {
            const auto toName = entry.path().filename();
            const auto toPath = to / toName;
            try {
                std::filesystem::create_symlink(path, toPath);
                lastBinaries.push_back(QString::fromStdString(toName.string()));
            } catch (const std::filesystem::filesystem_error &e) {
                SPDLOG_ERROR("Error creating link {} to {}", toPath.string(), path.string());
            }
        }
    }
    settings.setValue(kSettingLastBinaries, lastBinaries);
}

} // namespace qtchooser
