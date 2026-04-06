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

namespace qtchooser {

ListRunner::ListRunner(const ListCliOptions &cliOptions, QObject *parent) :
    QObject(parent), finder_(new QtFinder(this))
{
    connect(finder_, &QtFinder::found, this, &ListRunner::found);
    connect(finder_, &QtFinder::finished, this, &ListRunner::finished);
    connect(finder_, &QtFinder::finished, finder_, &QtFinder::deleteLater);
}

void ListRunner::start()
{
    std::cout << "Searching for Qt installations..." << std::endl;
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
    table.add_row({"Version", "Path", "Description"});
    table.row(0)
        .format()
        .font_align(tabulate::FontAlign::center)
        .color(tabulate::Color::yellow)
        .font_style({tabulate::FontStyle::bold});
    for (const auto &info : found_) {
        table.add_row(
            {info.version().toString().toStdString(),
             info.prefix().string(),
             info.name().toStdString()});
    }

    std::cout << table << std::endl;
    qApp->quit();
}

} // namespace qtchooser
