/**
 * @file ChooseRunner.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "ChooseRunner.h"
#include <iostream>
#include <QCoreApplication>
#include <QFutureWatcher>

namespace qtchooser {

ChooseRunner::ChooseRunner(const ChooseCliOptions &cliOptions, QObject *parent) :
    QObject(parent), prefix_(cliOptions.prefix), chooser_(new QtChooser(this)),
    watcher_(new QFutureWatcher<QtInfo::GetResult>(this))
{
    connect(chooser_, &QtChooser::envVarsChanged, this, &ChooseRunner::envVarsChanged);
    connect(watcher_, &QFutureWatcher<QtInfo::GetResult>::finished, this, &ChooseRunner::infoReady);
}

void ChooseRunner::start()
{
    std::cout << qPrintable(tr("Choosing Qt at %1").arg(QString::fromStdString(prefix_.string())))
              << std::endl;
    watcher_->setFuture(QtInfo::get(prefix_));
}

void ChooseRunner::envVarsChanged()
{
    std::cout << qPrintable(
        tr("An environment variable has changed. You may need to log out and back in for this "
           "change to take effect in all programs."))
              << std::endl;
}

void ChooseRunner::infoReady()
{
    const auto info = watcher_->result();
    if (!info.has_value()) {
        std::cerr << qPrintable(tr("Error while learning about Qt installation:")) << std::endl
                  << qPrintable([&]() {
                         switch (info.error()) {
                         case QtInfo::Error::FileNotFound:
                             return tr("File or directory not found.");
                         case QtInfo::Error::BadInstall:
                             return tr("Not a Qt installation or the installation is corrupt.");
                         case QtInfo::Error::BadData:
                             return tr(
                                 "The Qt installation responded in an unexpected way. This version "
                                 "may not be supported.");
                         case QtInfo::Error::Unknown:
                             return tr("An unknown error occured.");
                         }
                         Q_UNREACHABLE();
                     }());
        qApp->exit(1);
        return;
    }

    std::cout << qPrintable(info->name()) << std::endl;
    chooser_->choose(*info);
    qApp->quit();
}

} // namespace qtchooser
