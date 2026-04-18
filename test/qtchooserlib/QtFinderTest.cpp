/**
 * @file QtFinderTest.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "qtchooser/qtchooserlib/QtFinder.h"
#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <qtchooser/qtchooser_testinfo.h>

TEST_CASE("QtFinder")
{
    qtchooser::QtFinder finder;
    finder.addSearchPath(qtchooser::test::kHostQtPrefix);
    const auto found = finder.find();

    REQUIRE(found.size() > 0);

    // Check for the Qt this program was compiled with.
    const auto thisQtInfo = qtchooser::QtInfo::getFromPrefix(qtchooser::test::kHostQtPrefix);
    REQUIRE(thisQtInfo.has_value());
    CHECK(std::ranges::find(found, *thisQtInfo) != found.cend());
}
