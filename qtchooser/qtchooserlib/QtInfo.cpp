/**
 * @file QtInfo.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtInfo.h"
#include "FindFile.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/read.hpp>
#include <boost/process.hpp>
#include <spdlog/spdlog.h>

namespace qtchooser {

std::string qtQuery(const std::filesystem::path &qtpathsPath, const std::string &var)
{
    namespace proc = boost::process;
    namespace asio = boost::asio;

    asio::io_context ctx;
    asio::readable_pipe qtpathsOutP{ctx};
    asio::readable_pipe qtpathsErrP{ctx};

    proc::process qtpaths{
        ctx, qtpathsPath, {"--query", var}, proc::process_stdio{nullptr, qtpathsOutP, qtpathsErrP}};

    std::string qtpathsOut;
    std::string qtpathsErr;
    boost::system::error_code qtpathsOutEc;
    boost::system::error_code qtpathsErrEc;
    asio::read(qtpathsOutP, asio::dynamic_buffer(qtpathsOut), qtpathsOutEc);
    if (qtpathsOutEc != asio::error::eof) {
        SPDLOG_ERROR("Error reading qtpaths stdout");
        return {};
    }
    asio::read(qtpathsErrP, asio::dynamic_buffer(qtpathsErr), qtpathsErrEc);
    if (qtpathsErrEc != asio::error::eof) {
        SPDLOG_ERROR("Error reading qtpaths stderr");
        return {};
    }

    qtpaths.wait();
    boost::algorithm::trim(qtpathsOut);
    boost::algorithm::trim(qtpathsErr);
    if (qtpaths.exit_code() != 0) {
        SPDLOG_ERROR(
            "qtpaths exited with code {}\n{}\n{}", qtpaths.exit_code(), qtpathsOut, qtpathsErr);
        return {};
    }
    return qtpathsOut;
}

QtInfo::GetResult QtInfo::get(const std::filesystem::path &path)
{
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
    if (versionStr.empty()) {
        return std::unexpected(Error::BadData);
    }
    info.version_ = VersionNumber(versionStr);
    if (info.version_.empty()) {
        SPDLOG_ERROR("Unexpected version number");
        return std::unexpected(Error::BadData);
    }
    // Bin directory.
    const auto binDirStr = qtQuery(*qtpathsPath, "QT_HOST_BINS");
    if (binDirStr.empty()) {
        return std::unexpected(Error::BadData);
    }
    auto binDirPath = info.binDirs_.emplace_back(binDirStr);
    binDirPath.make_preferred();
    // Libexec directory.
    const auto libexecDirStr = qtQuery(*qtpathsPath, "QT_HOST_LIBEXECS");
    if (libexecDirStr.empty()) {
        return std::unexpected(Error::BadData);
    }
    auto libExecDirPath = info.binDirs_.emplace_back(libexecDirStr);
    libExecDirPath.make_preferred();
    // Lib directory.
    const auto libDirStr = qtQuery(*qtpathsPath, "QT_INSTALL_LIBS");
    if (libDirStr.empty()) {
        return std::unexpected(Error::BadData);
    }
    const std::filesystem::path libDir(libDirStr);
    const auto cmakePackageFile
        = findFile(std::format("Qt{}Config.cmake", info.version_.major()), libDir);
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

    return info;
}

} // namespace qtchooser
