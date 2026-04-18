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
    const auto prefix = std::filesystem::canonical(qtchooser::test::kHostQtPrefix);
    const auto version = QLibraryInfo::version();

    const auto info = qtchooser::QtInfo::get(prefix);
    if (!info.has_value()) {
        FAIL(std::format("Error {}", static_cast<int>(info.error())));
    }

    CHECK(info->prefix() == prefix);
    CHECK(info->version().string() == version.toString().toStdString());
    std::vector expectedBinDirs{qtchooser::test::kHostQtBinDir, qtchooser::test::kHostQtLibExecDir};
    // Dedupe.
    std::ranges::sort(expectedBinDirs);
    expectedBinDirs.erase(std::ranges::unique(expectedBinDirs).begin(), expectedBinDirs.end());
    CHECK(info->binDirs() == expectedBinDirs);
    CHECK(info->cmakePackageDir() == qtchooser::test::kHostQtCMakeDir);
}
