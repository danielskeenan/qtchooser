/**
 * @file EnvironmentInjector.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_ENVIRONMENTINJECTOR_H
#define QTCHOOSER_ENVIRONMENTINJECTOR_H

#include <filesystem>
#include <QHash>
#include <QStringList>

namespace qtchooser {

/**
 * Persist env vars in a platform-specific way.
 */
class EnvironmentInjector
{
public:
    using Environment = QHash<QString, QString>;

    explicit EnvironmentInjector();

    /**
     * Set the persistent environment variable @p var to @p val.
     *
     * @param var
     * @param val
     * @return `TRUE` if the value has changed from its previous value.
     */
    bool setEnv(const QString &var, const QString &val);

    /**
     * Add @p path to the system PATH variable.
     * @param path
     * @return `TRUE` if the value has changed from its previous value.
     */
    bool addToPath(const std::filesystem::path &path);

    /**
     * Remove @p path from the system PATH variable.
     * @param path
     * @return `TRUE` if the value has changed from its previous value.
     */
    bool removeFromPath(const std::filesystem::path &path);

    /**
     * Save the changes to the system.
     */
    void commit();

private:
    Environment env_;

    QStringList getUserPath();
    bool setUserPath(const QStringList &path);
};

} // namespace qtchooser

#endif //QTCHOOSER_ENVIRONMENTINJECTOR_H
