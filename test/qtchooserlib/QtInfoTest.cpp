/**
 * @file QtInfoTest.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "qtchooser/qtchooser_testinfo.h"
#include <catch2/catch_test_macros.hpp>
#include <QLibraryInfo>
#include <qtchooser/qtchooserlib/QtInfo.h>

TEST_CASE("QtInfo")
{
    const auto prefix = std::filesystem::canonical(qtchooser::test::kHostQtPath);
    const auto version = QLibraryInfo::version();

    auto future = qtchooser::QtInfo::get(prefix);
    future.waitForFinished();
    const auto info = future.takeResult();
    if (!info.has_value()) {
        FAIL(std::format("Error {}", static_cast<int>(info.error())));
    }

    CHECK(info->name().startsWith(QString("Qt %1").arg(QT_VERSION_STR)));
    CHECK(info->prefix() == prefix);
    CHECK(info->version().toString().toStdString() == version.toString().toStdString());
}
