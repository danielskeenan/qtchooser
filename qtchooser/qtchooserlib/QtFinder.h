/**
 * @file QtFinder.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_QTFINDER_H
#define QTCHOOSER_QTCHOOSERLIB_QTFINDER_H

#include "QtInfo.h"
#include <filesystem>

namespace qtchooser {

/**
 * Find Qt installations on this system.
 *
 * Platform-specific search paths are used. Additional paths can be added with addSearchPaths().
 */
class QtFinder
{
public:
    explicit QtFinder();

    /**
     * Add a search path in addition to the platform-specific paths.
     *
     * @note This function is not thread-safe. Do not call this function after calling start().
     *
     * @param path Path to add.
     */
    void addSearchPath(const std::filesystem::path &path);

    /**
     * Find Qt installations.
     * @return
     */
    std::vector<QtInfo> find();

private:
    std::vector<std::filesystem::path> searchPaths_;

    static std::vector<std::filesystem::path> defaultSearchPaths();

    /**
     * Could this path contain a Qt installation?
     *
     * @param path Path to check.
     * @return `FALSE` if this path definitely does not contain a Qt installation, `TRUE` if further
     * checks are necessary.
     */
    static bool isPossibleQtDir(const std::filesystem::path &path);
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_QTFINDER_H
