/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include <catch2/catch_session.hpp>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    // Parts of Qt assume there is an active QCoreApplication instance.
    QCoreApplication app(argc, argv);

    return Catch::Session().run(argc, argv);
}
