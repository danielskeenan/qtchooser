/**
 * @file QtFinder_win.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtFinder.h"

namespace qtchooser {
std::vector<std::filesystem::path> QtFinder::defaultSearchPaths()
{
    return {
        // Default installation path from official Qt installer.
        std::filesystem::path(std::getenv("WINDIR")).root_path() / "Qt"};
}
} // namespace qtchooser
