/**
 * @file EnvironmentInjector_linux.cpp
 *
 * @author Dan Keenan
 * @date 4/6/26
 * @copyright GPL-3.0
 */

#include "EnvironmentInjector.h"
#include <spdlog/spdlog.h>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QSaveFile>

namespace qtchooser {

constexpr auto kProfileMarker = "# Added by qtchooser";
constexpr auto kEnvPath = "PATH";
constexpr auto kEnvPathRef = ":${PATH}";
constexpr auto kEnvPathSeparator = ':';
const QRegularExpression kReExport(R"/(^s*export (w+)="?(.+?)"?$)/");

QFile getBashProfile()
{
    return QFile(QDir::home().absoluteFilePath(".bash_profile"));
}

EnvironmentInjector::Environment getUserEnvironment()
{
    auto bashProfileFile = getBashProfile();
    if (!bashProfileFile.open(QFile::ReadOnly)) {
        SPDLOG_WARN("Could not open .bash_profile");
        return {};
    }

    QTextStream bashProfile(&bashProfileFile);
    EnvironmentInjector::Environment env;
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
        env.insert(var, val);
    }

    return env;
}

EnvironmentInjector::EnvironmentInjector() : env_(getUserEnvironment()) {}

bool EnvironmentInjector::setEnv(const QString &var, const QString &val)
{
    if (env_[var] == val) {
        return false;
    }
    env_[var] = val;
    return true;
}

QStringList EnvironmentInjector::getUserPath()
{
    return env_[kEnvPath].split(kEnvPathSeparator, Qt::SkipEmptyParts);
}

bool EnvironmentInjector::setUserPath(const QStringList &path)
{
    return setEnv(kEnvPath, path.join(kEnvPathSeparator));
}

bool EnvironmentInjector::addToPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    if (userPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    userPath.push_back(QString::fromStdString(path.string()));
    return setUserPath(userPath);
}

bool EnvironmentInjector::removeFromPath(const std::filesystem::path &path)
{
    auto userPath = getUserPath();
    if (!userPath.contains(QString::fromStdString(path.string()))) {
        return false;
    }

    userPath.removeAll(QString::fromStdString(path.string()));
    return setUserPath(userPath);
}

void EnvironmentInjector::commit()
{
    auto bashProfileFile = getBashProfile();
    if (!bashProfileFile.open(QFile::ReadOnly)) {
        SPDLOG_WARN("Could not open .bash_profile.");
        return;
    }
    QSaveFile newBashProfileFile(bashProfileFile.fileName());
    if (!newBashProfileFile.open(QFile::WriteOnly)) {
        SPDLOG_ERROR("Could not open temp .bash_profile for writing.");
        return;
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
    for (const auto &[var, val] : env_.asKeyValueRange()) {
        auto exportVal = val;
        if (var == kEnvPath) {
            // Put our PATH before others.
            exportVal.append(kEnvPathRef);
        }
        newBashProfile << "export " << var << "=\"" << exportVal << "\"\n";
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
}

} // namespace qtchooser
