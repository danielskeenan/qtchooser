/**
 * @file EnvironmentInjectorLinux.cpp
 *
 * @author Dan Keenan
 * @date 4/12/26
 * @copyright GPL-3.0
 */

#include "EnvironmentInjectorLinux.h"
#include <spdlog/spdlog.h>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSaveFile>

namespace qtchooser {
constexpr auto kProfileMarker = "# Added by qtchooser";
constexpr auto kEnvPath = "PATH";
constexpr auto kEnvPathRef = ":${PATH}";
constexpr auto kEnvPathSeparator = ':';
const QRegularExpression kReExport(R"/(^\s*export (\w+)="?(.+?)"?$)/");

QFile getBashProfile()
{
    return QFile(QDir::home().absoluteFilePath(".bash_profile"));
}

EnvironmentInjectorLinux::EnvironmentInjectorLinux() : EnvironmentInjector()
{
    auto bashProfileFile = getBashProfile();
    if (!bashProfileFile.open(QFile::ReadOnly)) {
        SPDLOG_WARN("Could not open .bash_profile");
        return;
    }

    QTextStream bashProfile(&bashProfileFile);
    QString line;

    // Seek to our section of the file.
    while (bashProfile.readLineInto(&line)) {
        if (line == kProfileMarker) {
            break;
        }
    }

    // Parse the exports.
    while (bashProfile.readLineInto(&line)) {
        if (line.trimmed().isEmpty()) {
            // End of our section.
            break;
        }
        const auto match = kReExport.match(line);
        if (!match.hasMatch()) {
            break;
        }
        const auto var = match.captured(1);
        auto val = match.captured(2);
        if (var == kEnvPath) {
            // Remove reference to existing PATH var.
            val.replace(kEnvPathRef, "");
        }
        env_[var.toStdString()] = val.toStdString();
    }
    originalEnv_ = env_;
}

void EnvironmentInjectorLinux::setEnv(const std::string &var, const std::string &val)
{
    env_[var] = val;
}

bool EnvironmentInjectorLinux::commit()
{
    auto bashProfileFile = getBashProfile();
    if (!bashProfileFile.open(QFile::ReadOnly)) {
        SPDLOG_WARN("Could not open .bash_profile.");
        return false;
    }
    QSaveFile newBashProfileFile(bashProfileFile.fileName());
    if (!newBashProfileFile.open(QFile::WriteOnly)) {
        SPDLOG_ERROR("Could not open temp .bash_profile for writing.");
        return false;
    }
    QTextStream bashProfile(&bashProfileFile);
    QTextStream newBashProfile(&newBashProfileFile);
    QString line;

    // Seek to our section of the file.
    while (bashProfile.readLineInto(&line)) {
        if (line == kProfileMarker) {
            break;
        }
        newBashProfile << line << '\n';
    }

    // Write our exports.
    newBashProfile << kProfileMarker << '\n';
    for (const auto &[var, val] : env_) {
        auto exportVal = val;
        if (var == kEnvPath) {
            // Put our PATH before others.
            exportVal.append(kEnvPathRef);
        }
        newBashProfile << "export " << QString::fromStdString(var) << "=\""
                       << QString::fromStdString(exportVal) << "\"\n";
    }

    // Seek past original exports.
    while (bashProfile.readLineInto(&line)) {
        if (line.trimmed().isEmpty()) {
            // End of our section.
            newBashProfile << line << '\n';
            break;
        }
    }

    // Write remainder of file.
    while (bashProfile.readLineInto(&line)) {
        newBashProfile << line << '\n';
    }

    newBashProfile.flush();
    bashProfileFile.close();
    newBashProfileFile.commit();
    const bool changed = originalEnv_ != env_;
    originalEnv_ = env_;
    return changed;
}

std::vector<std::filesystem::path> EnvironmentInjectorLinux::getUserPath()
{
    std::vector<std::filesystem::path> userPath;

    if (env_.contains(kEnvPath)) {
        for (const auto &part : std::views::split(env_[kEnvPath], kEnvPathSeparator)) {
            userPath.emplace_back(std::string_view(part));
        }
    }

    return userPath;
}

void EnvironmentInjectorLinux::setUserPath(const std::vector<std::filesystem::path> &userPath)
{
    std::vector<std::string> pathStrs;
    for (const auto &path : userPath) {
        pathStrs.emplace_back(path.string());
    }
    env_[kEnvPath]
        = std::string(std::from_range, std::views::join_with(pathStrs, kEnvPathSeparator));
}
} // namespace qtchooser