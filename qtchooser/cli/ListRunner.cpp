/**
 * @file ListRunner.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "ListRunner.h"
#include <iostream>
#include <tabulate/table.hpp>

#include "qtchooser/qtchooserlib/CurrentChosen.h"

namespace qtchooser {

ListRunner::ListRunner(const ListCliOptions &cliOptions)
{
    for (const auto &path : cliOptions.searchPaths) {
        finder_.addSearchPath(path);
    }
}

void ListRunner::run()
{
    std::cout << "Searching for Qt installations..." << std::endl;
    const auto found = finder_.find();

    // Output a table.
    tabulate::Table table;
    // Header
    table.add_row({"Active", "Version", "Path"});
    table.row(0)
        .format()
        .font_align(tabulate::FontAlign::center)
        .color(tabulate::Color::yellow)
        .font_style({tabulate::FontStyle::bold});
    const CurrentChosen currentChosen;
    for (const auto &info : found) {
        const auto isCurrent = info.prefix() == currentChosen.prefix();
        table.add_row({(isCurrent ? "Active" : ""), info.version().string(), info.prefix().string()});
        if (isCurrent) {
            auto &row = table.row(table.size() - 1);
            row.format().color(tabulate::Color::green).font_style({tabulate::FontStyle::italic});
            row.cell(0).format().font_align(tabulate::FontAlign::center);
        }
    }

    std::cout << table << std::endl;
}

} // namespace qtchooser
