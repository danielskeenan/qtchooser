/**
 * @file QtInfo.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtInfo.h"
#include "FindFile.h"
#include <spdlog/spdlog.h>
#include <QFuture>
#include <QJsonDocument>
#include <QJsonValue>
#include <QProcess>
#include <QStandardPaths>
#include <QtConcurrentRun>

namespace qtchooser {

QtInfo::QtInfo()
{
    qRegisterMetaType<QtInfo>();
}

QString qtQuery(const std::filesystem::path &qtpathsPath, const QString &var)
{
    QProcess qtpaths;
    qtpaths.start(QString::fromStdString(qtpathsPath.string()), {"--query", var});
    qtpaths.waitForFinished();
    if (qtpaths.exitStatus() != QProcess::NormalExit || qtpaths.exitCode() != 0) {
        SPDLOG_ERROR(
            "qtpaths exited with code {}\n{}\n{}",
            qtpaths.exitCode(),
            qtpaths.readAllStandardOutput().toStdString(),
            qtpaths.readAllStandardError().toStdString());
        return {};
    }
    return qtpaths.readAllStandardOutput().trimmed();
}

QFuture<QtInfo::GetResult> QtInfo::get(const std::filesystem::path &path)
{
    return QtConcurrent::run([path]() -> QtInfo::GetResult {
        SPDLOG_DEBUG("Inspecting {}", path.string());
        QtInfo info;

        try {
            info.prefix_ = std::filesystem::absolute(path);
            info.prefix_.make_preferred();
            if (!std::filesystem::is_directory(info.prefix_)) {
                SPDLOG_DEBUG("Not a directory.");
                return std::unexpected(Error::FileNotFound);
            }
        } catch (const std::filesystem::filesystem_error &e) {
            SPDLOG_ERROR("Filesystem error: {}", e.what());
            return std::unexpected(Error::FileNotFound);
        }

        // Find qtpaths.
        const auto qtpathsPath = findQtPaths(info.prefix_);
        if (!qtpathsPath) {
            SPDLOG_ERROR("Cound not find qtpaths.");
            return std::unexpected(Error::BadInstall);
        }

        // Learn installation paths.
        // Version.
        const auto versionStr = qtQuery(*qtpathsPath, "QT_VERSION");
        if (versionStr.isEmpty()) {
            return std::unexpected(Error::BadData);
        }
        info.version_ = QVersionNumber::fromString(versionStr);
        if (info.version_.isNull()) {
            SPDLOG_ERROR("Unexpected version number");
            return std::unexpected(Error::BadData);
        }
        // Bin directory.
        const auto binDirStr = qtQuery(*qtpathsPath, "QT_INSTALL_BINS");
        if (binDirStr.isEmpty()) {
            return std::unexpected(Error::BadData);
        }
        auto binDirPath = info.binDirs_.emplace_back(binDirStr.toStdString());
        binDirPath.make_preferred();
        // Libexec directory.
        const auto libexecDirStr = qtQuery(*qtpathsPath, "QT_INSTALL_LIBEXECS");
        if (libexecDirStr.isEmpty()) {
            return std::unexpected(Error::BadData);
        }
        auto libExecDirPath = info.binDirs_.emplace_back(libexecDirStr.toStdString());
        libExecDirPath.make_preferred();
        // Lib directory.
        const auto libDirStr = qtQuery(*qtpathsPath, "QT_INSTALL_LIBS");
        if (libDirStr.isEmpty()) {
            return std::unexpected(Error::BadData);
        }
        const std::filesystem::path libDir(libDirStr.toStdString());
        const auto cmakePackageFile
            = findFile(std::format("Qt{}Config.cmake", info.version_.majorVersion()), libDir);
        if (!cmakePackageFile) {
            SPDLOG_ERROR("Could not find CMake package file.");
            return std::unexpected(Error::BadInstall);
        }
        info.cmakePackageDir_ = cmakePackageFile->parent_path();
        info.cmakePackageDir_.make_preferred();
        // Dedupe.
        std::ranges::sort(info.binDirs_);
        info.binDirs_.erase(std::ranges::unique(info.binDirs_).begin(), info.binDirs_.end());

        // Find qtdiag.
        const auto qtdiagPath = findQtDiag(info.prefix_, info.binDirs_);
        if (!qtdiagPath) {
            SPDLOG_ERROR("Could not find qtdiag.");
            return std::unexpected(Error::BadInstall);
        }

        // Get further information.
        QProcess qtdiag;
        qtdiag.start(QString::fromStdString(qtdiagPath->string()), {"--platform", "minimal"});
        qtdiag.waitForFinished();
        if (qtdiag.exitStatus() != QProcess::NormalExit || qtdiag.exitCode() != 0) {
            SPDLOG_ERROR(
                "qtdiag exited with code {}:\n{}\n{}",
                qtdiag.exitCode(),
                qtdiag.readAllStandardOutput().toStdString(),
                qtdiag.readAllStandardError().toStdString());
            return std::unexpected(Error::BadInstall);
        }
        qtdiag.setReadChannel(QProcess::StandardOutput);
        info.name_ = QString::fromUtf8(qtdiag.readLine()).trimmed();

        return info;
    });
}

} // namespace qtchooser
