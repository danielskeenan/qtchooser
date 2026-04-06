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

QFuture<QtInfo::GetResult> QtInfo::get(const std::filesystem::path &path)
{
    return QtConcurrent::run([path]() -> QtInfo::GetResult {
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
        const auto qtdiagPath = findQtDiag(info.prefix_);
        if (!qtdiagPath) {
            SPDLOG_ERROR("Could not find qtdiag.");
            return std::unexpected(Error::BadInstall);
        }
        const auto qtpathsPath = findQtPaths(info.prefix_);
        if (!qtpathsPath) {
            SPDLOG_ERROR("Cound not find qtpaths.");
            return std::unexpected(Error::BadInstall);
        }

        // Ask about this installation.
        QProcess qtdiag;
        qtdiag.start(QString::fromStdString(qtdiagPath->string()));
        qtdiag.waitForFinished();
        if (qtdiag.exitStatus() != QProcess::NormalExit || qtdiag.exitCode() != 0) {
            SPDLOG_ERROR("qtdiag exited with code {}", qtdiag.exitCode());
            return std::unexpected(Error::BadInstall);
        }
        qtdiag.setReadChannel(QProcess::StandardOutput);
        info.name_ = QString::fromUtf8(qtdiag.readLine()).trimmed();

        QProcess qtpaths;
        qtpaths.start(
            QString::fromStdString(qtpathsPath->string()), {"--query", "--query-format", "json"});
        qtpaths.waitForFinished();
        if (qtpaths.exitStatus() != QProcess::NormalExit || qtpaths.exitCode() != 0) {
            SPDLOG_ERROR("qtpaths exited with code {}", qtpaths.exitCode());
            return std::unexpected(Error::BadInstall);
        }
        qtpaths.setReadChannel(QProcess::StandardOutput);
        const auto qtquery = QJsonDocument::fromJson(qtpaths.readAllStandardOutput());
        info.version_ = QVersionNumber::fromString(qtquery["QT_VERSION"].toStringView());
        if (info.version_.isNull()) {
            SPDLOG_ERROR("Unexpected version number");
            return std::unexpected(Error::BadData);
        }
        info.binDir_ = qtquery["QT_INSTALL_BINS"].toString().toStdString();
        const std::filesystem::path libDir(qtquery["QT_INSTALL_LIBS"].toString().toStdString());
        const auto cmakePackageFile
            = findFile(std::format("Qt{}Config.cmake", info.version_.majorVersion()), libDir);
        if (!cmakePackageFile) {
            SPDLOG_ERROR("Could not find CMake package file.");
            return std::unexpected(Error::BadInstall);
        }
        info.cmakePackageDir_ = cmakePackageFile->parent_path();

        return info;
    });
}

} // namespace qtchooser
