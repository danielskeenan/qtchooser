/**
 * @file QtInfo.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_QTINFO_H
#define QTCHOOSER_QTCHOOSERLIB_QTINFO_H

#include <expected>
#include <filesystem>
#include <QFuture>
#include <QString>
#include <QVersionNumber>

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

    explicit QtInfo();
    auto operator<=>(const QtInfo &) const = default;
    using GetResult = std::expected<QtInfo, QtInfo::Error>;
    static QFuture<GetResult> get(const std::filesystem::path &path);
    [[nodiscard]] QString name() const { return name_; }
    [[nodiscard]] QVersionNumber version() const { return version_; }
    [[nodiscard]] std::filesystem::path prefix() const { return prefix_; }
    [[nodiscard]] std::filesystem::path binDir() const { return binDir_; }
    [[nodiscard]] std::filesystem::path cmakePackageDir() const { return cmakePackageDir_; }

private:
    QVersionNumber version_;
    QString name_;
    std::filesystem::path prefix_;
    std::filesystem::path binDir_;
    std::filesystem::path cmakePackageDir_;
};

} // namespace qtchooser

Q_DECLARE_METATYPE(qtchooser::QtInfo)

#endif //QTCHOOSER_QTCHOOSERLIB_QTINFO_H
