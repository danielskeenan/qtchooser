/**
 * @file QtFinder_none.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtFinder.h"

namespace qtchooser {
std::vector<std::filesystem::path> QtFinder::defaultSearchPaths()
{
    return {};
}
}
