/**
 * @file QtInfo.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_QTINFO_H
#define QTCHOOSER_QTCHOOSERLIB_QTINFO_H

#include <filesystem>
#include <QString>
#include <QVersionNumber>
#include <expected>
#include <QFuture>

namespace qtchooser {

/**
 * Hold information about a given Qt installation.
 */
class QtInfo {
public:
    /**
     * Possible errors when inspecting a possible Qt installation.
     */
    enum class Error
    {
        /** The path does not exist. */
        FileNotFound,
        /** The installation is missing critical files. */
        BadInstall,
        /** The installation returned unusable data about itself. */
        BadData,
        /** An unhandled error occurred (this is probably a bug). */
        Unknown,
    };

    static QFuture<std::expected<QtInfo, QtInfo::Error>> get(const std::filesystem::path& path);
    [[nodiscard]] QString name() const { return name_; }
    [[nodiscard]] QVersionNumber version() const { return version_; }
    [[nodiscard]] std::filesystem::path prefix() const { return prefix_; }

private:
    QString name_;
    QVersionNumber version_;
    std::filesystem::path prefix_;
};

} // qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_QTINFO_H
