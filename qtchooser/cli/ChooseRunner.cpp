/**
 * @file ChooseRunner.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "ChooseRunner.h"
#include <iostream>

namespace qtchooser {

ChooseRunner::ChooseRunner(const ChooseCliOptions &cliOptions) : prefix_(cliOptions.prefix) {}

void ChooseRunner::run()
{
    std::cout << "Choosing Qt at " << prefix_.string() << std::endl;
    const auto info = QtInfo::get(prefix_);

    if (!info.has_value()) {
        std::cerr << "Error while learning about Qt installation:" << std::endl
                  <<
            [&]() {
                switch (info.error()) {
                case QtInfo::Error::FileNotFound:
                    return "File or directory not found.";
                case QtInfo::Error::BadInstall:
                    return "Not a Qt installation or the installation is corrupt.";
                case QtInfo::Error::BadData:
                    return "The Qt installation responded in an unexpected way. This version "
                           "may not be supported.";
                case QtInfo::Error::Unknown:
                    return "An unknown error occured.";
                }
                throw std::logic_error("Unhandled finder error");
            }();
        return;
    }
    if (chooser_.choose(*info)) {
        std::cout
            << "An environment variable has changed. You may need to log out and back in for this "
               "change to take effect in all programs."
            << std::endl;
    }
}

} // namespace qtchooser
