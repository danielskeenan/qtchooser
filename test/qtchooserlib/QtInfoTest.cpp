/**
 * @file QtInfoTest.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include <catch2/catch_test_macros.hpp>
#include <qtchooser/qtchooserlib/QtInfo.h>
#include <QLibraryInfo>
#include "qtchooser/qtchooser_testinfo.h"

TEST_CASE("Inspect Qt")
{
    const auto prefix = std::filesystem::canonical(qtchooser::test::kHostQtPath);
    const auto version = QLibraryInfo::version();

    auto future = qtchooser::QtInfo::get(prefix);
    future.waitForFinished();
    const auto info = future.takeResult();
    if (!info.has_value()) {
        FAIL(std::format("Error {}", static_cast<int>(info.error())));
    }

    CAPTURE(info->name().toStdString());
    CAPTURE(QLibraryInfo::build());
    CHECK(info->name().startsWith(QLibraryInfo::build()));
    CHECK(info->prefix() == prefix);
    CHECK(info->version().toString().toStdString() == version.toString().toStdString());
}
