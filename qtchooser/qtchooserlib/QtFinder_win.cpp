/**
 * @file QtFinder_win.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "QtFinder.h"
#include <QDir>

namespace qtchooser {
std::vector<std::filesystem::path> QtFinder::defaultSearchPaths()
{
    return {
        std::filesystem::path(QDir::rootPath().toStdString()) / "Qt"
    };
}
}
