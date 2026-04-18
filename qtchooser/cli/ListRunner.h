/**
 * @file ListRunner.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_CLI_LISTRUNNER_H
#define QTCHOOSER_CLI_LISTRUNNER_H

#include <qtchooser/qtchooserlib/QtFinder.h>

namespace qtchooser {

struct ListCliOptions
{
    std::vector<std::filesystem::path> searchPaths;
};

/**
 * List Qt installations.
 */
class ListRunner
{
public:
    explicit ListRunner(const ListCliOptions &cliOptions);

    void run();

private:
    QtFinder finder_;
};

} // namespace qtchooser

#endif //QTCHOOSER_CLI_LISTRUNNER_H
