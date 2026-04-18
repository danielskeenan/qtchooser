/**
 * @file QtFinder_linux.cpp
 *
 * @author Dan Keenan
 * @date 4/6/26
 * @copyright GPL-3.0
 */

#include "QtFinder.h"
#include <boost/process/environment.hpp>

namespace qtchooser {
std::vector<std::filesystem::path> QtFinder::defaultSearchPaths()
{
    return {
        std::filesystem::path("/usr"),
        std::filesystem::path("/usr/local"),
        // Default installation path from official Qt installer.
        boost::process::environment::home() / "Qt",
    };
}
} // namespace qtchooser
