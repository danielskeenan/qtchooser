/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#include "ListRunner.h"
#include "qtchooser/qtchooser_config.h"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QTimer>

struct ChooseCliOptions
{
    std::filesystem::path binDir
        = std::filesystem::path(
              QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString())
          / "qtbin";
};

using CliOptions = std::variant<qtchooser::ListCliOptions, ChooseCliOptions>;

CliOptions parseCommandLine(int argc, char *argv[])
{
    argparse::ArgumentParser
        parser(qApp->applicationName().toStdString(), qApp->applicationVersion().toStdString());

    argparse::ArgumentParser cmdList("list");
    cmdList.add_description("List Qt installations.");
    cmdList.add_argument("--path")
        .help("Additional path to search.")
        .nargs(argparse::nargs_pattern::any);
    parser.add_subparser(cmdList);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl << parser << std::endl;
        std::exit(1);
    }

    if (parser.is_subcommand_used(cmdList)) {
        qtchooser::ListCliOptions cliOptions;
        for (const auto &pathStr : cmdList.get<std::vector<std::string>>("--path")) {
            const auto &path = cliOptions.searchPaths.emplace_back(pathStr);
            if (!std::filesystem::is_directory(path)) {
                std::cerr << "Not a directory: " << pathStr << std::endl;
                std::exit(1);
            }
        }

        return cliOptions;
    }

    std::cerr << parser << std::endl;
    std::exit(1);
}

struct Runner
{
    void operator()(const qtchooser::ListCliOptions &cliOptions) const
    {
        QTimer::singleShot(0, [&cliOptions]() {
            auto runner = new qtchooser::ListRunner(cliOptions);
            runner->start();
        });
    }

    void operator()(const ChooseCliOptions &cliOptions) const
    {
        QTimer::singleShot(0, [&cliOptions]() { qApp->quit(); });
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(qtchooser::config::kProjectOrganizationName);
    app.setOrganizationDomain(qtchooser::config::kProjectOrganizationDomain);
    app.setApplicationName(qtchooser::config::kProjectName);
    app.setApplicationVersion(qtchooser::config::kProjectVersion);

    const auto cliOptions = parseCommandLine(argc, argv);
    std::visit(Runner(), cliOptions);

    return app.exec();
}
