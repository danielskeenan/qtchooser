/**
 * @file ChooseRunner.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_CLI_CHOOSERUNNER_H
#define QTCHOOSER_CLI_CHOOSERUNNER_H

#include <qtchooser/qtchooserlib/QtChooser.h>

namespace qtchooser {

struct ChooseCliOptions
{
    std::filesystem::path prefix;
};

/**
 * Choose a specific Qt installation.
 */
class ChooseRunner
{
public:
    explicit ChooseRunner(const ChooseCliOptions &cliOptions);

    void run();

private:
    std::filesystem::path prefix_;
    QtChooser chooser_;
};

} // namespace qtchooser

#endif //QTCHOOSER_CLI_CHOOSERUNNER_H
