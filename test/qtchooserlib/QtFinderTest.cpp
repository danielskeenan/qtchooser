/**
 * @file QtFinderTest.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "qtchooser/qtchooserlib/QtFinder.h"
#include <catch2/catch_test_macros.hpp>
#include <QSignalSpy>
#include <qtchooser/qtchooser_testinfo.h>

TEST_CASE("QtFinder")
{
    qtchooser::QtFinder finder;
    QSignalSpy finderSpy(&finder, &qtchooser::QtFinder::found);
    finder.start();

    // This must finish within 10 seconds.
    REQUIRE(finder.wait(10000));
    REQUIRE(finderSpy.size() > 0);

    // Check for the Qt this program was compiled with.
    const auto thisQtInfo = qtchooser::QtInfo::get(qtchooser::test::kHostQtPrefix).result();
    REQUIRE(thisQtInfo.has_value());
    CHECK(std::ranges::any_of(finderSpy, [&thisQtInfo](const QVariantList &args) {
        return args.front().value<qtchooser::QtInfo>() == thisQtInfo;
    }));
}
