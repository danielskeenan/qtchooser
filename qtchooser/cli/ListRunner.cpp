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
#include <QCollator>
#include <QCoreApplication>

#include "qtchooser/qtchooserlib/CurrentChosen.h"

namespace qtchooser {

ListRunner::ListRunner(const ListCliOptions &cliOptions, QObject *parent) :
    QObject(parent), finder_(new QtFinder(this))
{
    for (const auto &path : cliOptions.searchPaths) {
        finder_->addSearchPath(path);
    }

    connect(finder_, &QtFinder::found, this, &ListRunner::found);
    connect(finder_, &QtFinder::finished, this, &ListRunner::finished);
}

void ListRunner::start()
{
    std::cout << qPrintable(tr("Searching for Qt installations...")) << std::endl;
    finder_->start();
}

void ListRunner::found(const QtInfo &info)
{
    found_.push_back(info);
}

void ListRunner::finished()
{
    // Sort the list.
    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setIgnorePunctuation(true);
    collator.setNumericMode(true);
    std::ranges::sort(found_, [&collator](const QtInfo &lhs, const QtInfo &rhs) {
        return collator(lhs.name(), rhs.name());
    });

    // Output a table.
    tabulate::Table table;
    // Header
    table.add_row(
        {qPrintable(tr("Version")), qPrintable(tr("Path")), qPrintable(tr("Description"))});
    table.row(0)
        .format()
        .font_align(tabulate::FontAlign::center)
        .color(tabulate::Color::yellow)
        .font_style({tabulate::FontStyle::bold});
    const CurrentChosen currentChosen;
    for (const auto &info : found_) {
        table.add_row(
            {info.version().toString().toStdString(),
             info.prefix().string(),
             info.name().toStdString()});
        if (info.prefix() == currentChosen.prefix()) {
            auto row = table.row(table.size() - 1);
            row.format().color(tabulate::Color::green).font_style({tabulate::FontStyle::italic});
        }
    }

    std::cout << table << std::endl;
    qApp->quit();
}

} // namespace qtchooser
