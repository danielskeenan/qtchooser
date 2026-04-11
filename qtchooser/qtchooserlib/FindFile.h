/**
 * @file FindFile.h
 *
 * @author Dan Keenan
 * @date 4/6/26
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_FINDFILE_H
#define QTCHOOSER_QTCHOOSERLIB_FINDFILE_H

#include <filesystem>
#include <optional>
#include <QStringList>

namespace qtchooser {

/**
 * Search @p searchDir recursively for a file.
 *
 * @param name
 * @param searchDir
 * @return Path to the first instance of the file, or std::nullopt if not found.
 */
std::optional<std::filesystem::path> findFile(
    const std::string &name, const std::filesystem::path &searchDir);

/**
 * Find an executable inside @p prefix.
 *
 * Searches inside @p prefix in the same manner as CMake's find_program.
 *
 * @param names A list of possible names for the program.
 * @param prefix Prefix to search inside.
 * @param extraPaths Additional paths to search. These paths will be searched before the default paths.
 * @return Path to the program, or std::nullopt if not found.
 *
 * @see https://cmake.org/cmake/help/latest/command/find_program.html
 */
std::optional<std::filesystem::path> findProgram(
    const QStringList &names,
    const std::filesystem::path &prefix,
    const std::vector<std::filesystem::path> &extraPaths);
inline std::optional<std::filesystem::path> findProgram(
    const QStringList &names, const std::filesystem::path &prefix)
{
    return findProgram(names, prefix, {});
}

inline std::optional<std::filesystem::path> findQtPaths(const std::filesystem::path &prefix)
{
    return findProgram({"qtpaths6", "qtpaths", "qtpaths5"}, prefix);
}

inline std::optional<std::filesystem::path> findQtDiag(
    const std::filesystem::path &prefix, const std::vector<std::filesystem::path> &binDirs)
{
    return findProgram({"qtdiag6", "qtpaths", "qtdiag5"}, prefix, binDirs);
}

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_FINDFILE_H
