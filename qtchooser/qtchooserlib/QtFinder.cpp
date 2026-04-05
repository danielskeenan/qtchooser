/**
 * @file QtFinder.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtFinder.h"
#include <regex>
#include <spdlog/spdlog.h>
#include <QDir>
#include <QStandardPaths>
#include <queue>

namespace qtchooser {

QtFinder::QtFinder(QObject *parent) : QThread(parent), searchPaths_(defaultSearchPaths()) {}

void QtFinder::run()
{
    const std::regex reVersionNumber(R"(^[\d.]+$)");
    std::queue searchPaths(searchPaths_.cbegin(), searchPaths_.cend());

    while (!searchPaths.empty()) {
        const auto searchPath = searchPaths.front();
        searchPaths.pop();

        // POSIX-style directory.
        if (isPossibleQtDir(searchPath)) {
            auto info = QtInfo::get(searchPath);
            info.waitForFinished();
            const auto result = info.takeResult();
            if (result.has_value()) {
                SPDLOG_DEBUG("Found {}", result->name().toStdString());
                Q_EMIT(found(*result));
            }
        }

        // Qt Installer-style directory. The official Qt Installer and aqt install Qt as <version>/<arch>/<bin/lib...>
        if (std::filesystem::is_directory(searchPath)) {
            for (const auto &versionEntry : std::filesystem::directory_iterator(searchPath)) {
                if (versionEntry.is_directory()
                    && std::regex_match(versionEntry.path().stem().string(), reVersionNumber)) {
                    for (const auto &archEntry : std::filesystem::directory_iterator(versionEntry)) {
                        if (archEntry.is_directory()) {
                            searchPaths.push(archEntry.path());
                        }
                    }
                }
            }
        }
    }
}

void QtFinder::addSearchPath(const std::filesystem::path &path)
{
    searchPaths_.emplace_back(path);
}

bool QtFinder::isPossibleQtDir(const std::filesystem::path &path)
{
    // Look for qtpaths as a sign that this might have a Qt installation.
    return std::filesystem::is_directory(path / "bin")
           && !QStandardPaths::findExecutable(
                   "qtpaths", {QString::fromStdString((path / "bin").string())})
                   .isEmpty();
}

} // namespace qtchooser
