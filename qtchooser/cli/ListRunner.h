/**
 * @file ListRunner.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_CLI_LISTRUNNER_H
#define QTCHOOSER_CLI_LISTRUNNER_H

#include <QPointer>
#include <qtchooser/qtchooserlib/QtFinder.h>

namespace qtchooser {

struct ListCliOptions
{
    std::vector<std::filesystem::path> searchPaths;
};

/**
 * List Qt installations.
 */
class ListRunner : public QObject
{
    Q_OBJECT
public:
    explicit ListRunner(const ListCliOptions &cliOptions, QObject *parent = nullptr);

    void start();

private:
    QPointer<QtFinder> finder_;
    std::vector<QtInfo> found_;

private Q_SLOTS:
    void found(const QtInfo &info);
    void finished();
};

} // namespace qtchooser

#endif //QTCHOOSER_CLI_LISTRUNNER_H
