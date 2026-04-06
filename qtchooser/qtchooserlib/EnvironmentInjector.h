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
#include <qcontainerfwd.h>

namespace qtchooser {

/**
 * Create symlinks of all executables from @p from in the directory @p to.
 *
 * All links created from previous calls to this function will be removed first.
 *
 * @param from
 * @param to
 */
void linkBinaries(const std::filesystem::path &from, const std::filesystem::path &to);

/**
 * Set the persistent environment variable @p var to @p val.
 *
 * @param var
 * @param val
 * @return `TRUE` if the value has changed from its previous value.
 */
bool setEnv(const QString &var, const QString &val);

/**
 * Get the current system PATH.
 * @return
 */
QStringList getPath();

/**
 * Add @p path to the system PATH variable.
 * @param path
 * @return `TRUE` if the value has changed from its previous value.
 */
bool addToPath(const std::filesystem::path& path);

/**
 * Remove @p path from the system PATH variable.
 * @param path
 * @return `TRUE` if the value has changed from its previous value.
 */
bool removeFromPath(const std::filesystem::path& path);

} // namespace qtchooser

#endif //QTCHOOSER_ENVIRONMENTINJECTOR_H
