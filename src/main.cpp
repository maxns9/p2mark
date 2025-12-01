/*
* Project: p2mark
* File:    main.cpp
* Desc:    Entry point for the application
* Created: 2025-10-07
*/

#include <filesystem>
#include <format>
#include <iostream>
#include <string_view>
#include <format>

#include "argparse.hpp"

#include "ScopedTimer.hpp"
#include "Constants.hpp"
#include "P2Exception.hpp"
#include "Application.hpp"
#include "AppInfo.hpp"

using namespace p2mark;

// Program's command line arguments:
static inline constexpr std::string_view ARG_CONTENTS_PATH {"contents_path"};
static inline constexpr std::string_view ARG_HELP_SHORT    {"-h"};
static inline constexpr std::string_view ARG_HELP_LONG     {"--help"};
static inline constexpr std::string_view ARG_LIST_SHORT    {"-l"};
static inline constexpr std::string_view ARG_LIST_LONG     {"--list"};
static inline constexpr std::string_view ARG_VERSION_SHORT {"-v"};
static inline constexpr std::string_view ARG_VERSION_LONG  {"--version"};

static void SetupArguments(argparse::ArgumentParser& parser) {
    parser.add_description(AppInfo::Description.data());

    parser.add_argument(ARG_CONTENTS_PATH)
        .help("Path to the P2 CONTENTS directory.");

    parser.add_argument(ARG_HELP_SHORT, ARG_HELP_LONG)
        .help("Prints the program\'s help page and exits.")
        .flag()
        .action([&parser](const std::string&) -> void {
            std::cout << parser.help().str();
            std::exit(0);
        });

    parser.add_argument(ARG_LIST_SHORT, ARG_LIST_LONG)
        .help("List the markers, don\'t generate XMPs.")
        .flag();

    parser.add_argument(ARG_VERSION_SHORT, ARG_VERSION_LONG)
        .help("Prints the program\'s version and exits.")
        .flag()
        .action([&parser](const std::string&) -> void {
            std::cout << std::format("{} version: {}\n", AppInfo::Name, AppInfo::Version.ToString());
            std::exit(0);
        });

    parser.add_epilog(AppInfo::Author.data());
}

int main(int argc, char* argv[]) {
    const bool exitOnDefaultArguments {true};
    argparse::ArgumentParser argParser(AppInfo::Name.data(),
                                       AppInfo::Version.ToString(),
                                       argparse::default_arguments::none,
                                       exitOnDefaultArguments);
    SetupArguments(argParser);

    try {
        argParser.parse_args(argc, argv);
    } catch(const std::exception& e) {
        std::cerr << std::format("{}\nType -h or --help to get usage info.\n", e.what());
        return 1;
    }

    AppMode mode {AppMode::MODE_WRITE_MARKERS};
    if(argParser.is_used(ARG_LIST_SHORT)) {
        mode = AppMode::MODE_LIST_MARKERS;
    }

    std::cout << std::format("{} running in {} mode.\n\n",
                             AppInfo::Name, AppModeToString(mode));

    try {
        // SCOPED_TIMER; // Uncomment to time the execution of the program

        Application app(mode, argParser.get<std::string>(ARG_CONTENTS_PATH));
        app.RetrieveClipFiles();
        app.SortClipFiles();
        app.BatchProcessClips();

        return 0;
    } catch(const P2Exception& e) {
        std::cerr << std::format("{}.\n", e.what());
        return 1;
    } catch(const std::filesystem::filesystem_error& e) {
        std::cerr << std::format("Cannot access path: {}.\n",
                                 e.path1().string());
        return 1;
    } catch(const std::exception&) {
        std::cerr << "Unexpected error occured during processing.\n";
        return 1;
    }
}