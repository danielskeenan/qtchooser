/**
 * @file QtInfo.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtInfo.h"
#include <spdlog/spdlog.h>
#include <QFuture>
#include <QProcess>
#include <QStandardPaths>
#include <QtConcurrentRun>

namespace qtchooser {

QtInfo::QtInfo()
{
    qRegisterMetaType<QtInfo>();
}

/**
 * Similar to CMake's find_file.
 *
 * Find the first instance of a filename in searchDir.
 * @param name
 * @param searchDir
 * @return
 */
std::optional<std::filesystem::path> findFile(
    const std::string &name, const std::filesystem::path &searchDir)
{
    for (const auto &entry : std::filesystem::recursive_directory_iterator(
             searchDir, std::filesystem::directory_options::skip_permission_denied)) {
        if (std::filesystem::is_regular_file(std::filesystem::canonical(entry.path()))
            && entry.path().filename() == name) {
            return entry.path();
        }
    }

    return {};
}

QFuture<std::expected<QtInfo, QtInfo::Error>> QtInfo::get(const std::filesystem::path &path)
{
    return QtConcurrent::run([path]() -> std::expected<QtInfo, QtInfo::Error> {
        SPDLOG_DEBUG("Inspecting {}", path.string());
        QtInfo info;

        try {
            info.prefix_ = std::filesystem::absolute(path);
            if (!std::filesystem::is_directory(info.prefix_)) {
                SPDLOG_DEBUG("Not a directory.");
                return std::unexpected(Error::FileNotFound);
            }
        } catch (const std::filesystem::filesystem_error &e) {
            SPDLOG_ERROR("Filesystem error: {}", e.what());
            return std::unexpected(Error::FileNotFound);
        }
        info.binDir_ = info.prefix_ / "bin";

        // Find qtdiag and qtpaths.
        const QStringList qtBinDirs{QString::fromStdString((info.binDir_).string())};
        const auto qtdiagPath = QStandardPaths::findExecutable("qtdiag", qtBinDirs);
        if (qtdiagPath.isEmpty()) {
            SPDLOG_ERROR("Could not find qtdiag.");
            return std::unexpected(Error::BadInstall);
        }
        const auto qtpathsPath = QStandardPaths::findExecutable("qtpaths", qtBinDirs);
        if (qtpathsPath.isEmpty()) {
            SPDLOG_ERROR("Cound not find qtpaths.");
            return std::unexpected(Error::BadInstall);
        }

        // Ask about this installation.
        QProcess qtdiag;
        qtdiag.start(qtdiagPath);
        qtdiag.waitForFinished();
        if (qtdiag.exitStatus() != QProcess::NormalExit || qtdiag.exitCode() != 0) {
            SPDLOG_ERROR("qtdiag exited with code {}", qtdiag.exitCode());
            return std::unexpected(Error::BadInstall);
        }
        qtdiag.setReadChannel(QProcess::StandardOutput);
        info.name_ = QString::fromUtf8(qtdiag.readLine()).trimmed();

        QProcess qtpaths;
        qtpaths.start(qtpathsPath, {"--qt-version"});
        qtpaths.waitForFinished();
        if (qtpaths.exitStatus() != QProcess::NormalExit || qtpaths.exitCode() != 0) {
            SPDLOG_ERROR("qtpaths exited with code {}", qtpaths.exitCode());
            return std::unexpected(Error::BadInstall);
        }
        qtpaths.setReadChannel(QProcess::StandardOutput);
        info.version_ = QVersionNumber::fromString(QString::fromUtf8(qtpaths.readLine()).trimmed());
        if (info.version_.isNull()) {
            SPDLOG_ERROR("Unexpected version number");
            return std::unexpected(Error::BadData);
        }
        const auto cmakePackageFile
            = findFile(std::format("Qt{}Config.cmake", info.version_.majorVersion()), info.prefix_);
        if (!cmakePackageFile) {
            SPDLOG_ERROR("Could not find CMake package file.");
            return std::unexpected(Error::BadInstall);
        }
        info.cmakePackageDir_ = cmakePackageFile->parent_path();

        return info;
    });
}

} // namespace qtchooser
