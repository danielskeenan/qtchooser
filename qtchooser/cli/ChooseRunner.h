/**
 * @file ChooseRunner.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_CLI_CHOOSERUNNER_H
#define QTCHOOSER_CLI_CHOOSERUNNER_H

#include <QPointer>
#include <qtchooser/qtchooserlib/QtChooser.h>

namespace qtchooser {

struct ChooseCliOptions
{
    std::filesystem::path prefix;
};

/**
 * Choose a specific Qt installation.
 */
class ChooseRunner : public QObject
{
    Q_OBJECT
public:
    explicit ChooseRunner(const ChooseCliOptions& cliOptions, QObject *parent = nullptr);

    void start();

private:
    std::filesystem::path prefix_;
    QPointer<QtChooser> chooser_;
    QPointer<QFutureWatcher<QtInfo::GetResult>> watcher_;

private Q_SLOTS:
    void envVarsChanged();
    void infoReady();
};

} // namespace qtchooser

#endif //QTCHOOSER_CLI_CHOOSERUNNER_H
