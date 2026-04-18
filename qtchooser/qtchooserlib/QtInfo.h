/**
 * @file QtInfo.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_QTINFO_H
#define QTCHOOSER_QTCHOOSERLIB_QTINFO_H

#include "VersionNumber.h"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace qtchooser {

/**
 * Hold information about a given Qt installation.
 */
class QtInfo
{
public:
    /**
     * Possible errors when inspecting a possible Qt installation.
     */
    enum class Error {
        /** The path does not exist. */
        FileNotFound,
        /** The installation is missing critical files. */
        BadInstall,
        /** The installation returned unusable data about itself. */
        BadData,
        /** An unhandled error occurred (this is probably a bug). */
        Unknown,
    };

    auto operator<=>(const QtInfo &) const = default;
    using GetResult = std::expected<QtInfo, QtInfo::Error>;
    static GetResult getFromPrefix(const std::filesystem::path &path);
    static GetResult getFromQtPaths(const std::filesystem::path &qtpathsPath);
    [[nodiscard]] VersionNumber version() const { return version_; }
    [[nodiscard]] std::filesystem::path prefix() const { return prefix_; }
    [[nodiscard]] const std::vector<std::filesystem::path> &binDirs() const { return binDirs_; }
    [[nodiscard]] std::filesystem::path cmakePackageDir() const { return cmakePackageDir_; }

private:
    VersionNumber version_;
    std::filesystem::path prefix_;
    std::vector<std::filesystem::path> binDirs_;
    std::filesystem::path cmakePackageDir_;
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_QTINFO_H
