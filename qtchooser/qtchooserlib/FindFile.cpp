/**
 * @file FindFile.cpp
 *
 * @author Dan Keenan
 * @date 4/6/26
 * @copyright GPL-3.0
 */

#include "FindFile.h"

#include <QStandardPaths>
#include <QStringList>

namespace qtchooser {

std::optional<std::filesystem::path> findFile(
    const std::string &name, const std::filesystem::path &searchDir)
{
    for (const auto &entry : std::filesystem::recursive_directory_iterator(
             searchDir, std::filesystem::directory_options::skip_permission_denied)) {
        if (std::filesystem::is_regular_file(entry.path()) && entry.path().filename() == name) {
            return entry.path();
        }
    }

    return {};
}

std::optional<std::filesystem::path> findProgram(
    const QStringList &names, const std::filesystem::path &prefix)
{
    const QStringList searchPaths{
        QString::fromStdString(prefix / "bin"),
        QString::fromStdString(prefix / "sbin"),
    };
    for (const auto &name : names) {
        const auto found = QStandardPaths::findExecutable(name, searchPaths);
        if (!found.isEmpty()) {
            return found.toStdString();
        }
    }

    return {};
}

} // namespace qtchooser
