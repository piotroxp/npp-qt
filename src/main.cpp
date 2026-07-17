// main.cpp - Application entry point for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFileOpenEvent>
#include <csignal>
#include <iostream>
#include <chrono>

#include "core/Application.h"
#include "common/Util.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define main main // prevent Qt's qMain override
#endif

// ============================================================================
// Signal handlers
// ============================================================================
static std::atomic<bool> g_sigintReceived{false};
static std::atomic<bool> g_sigtermReceived{false};

extern "C" void signalHandler(int sig) {
    if (sig == SIGINT) g_sigintReceived = true;
    if (sig == SIGTERM) g_sigtermReceived = true;
}

// ============================================================================
// QApplication subclass to handle file open events (macOS)
// ============================================================================
class NotepadApp : public QApplication {
public:
    NotepadApp(int& argc, char** argv) : QApplication(argc, argv) {}

    bool event(QEvent* event) override {
        if (event->type() == QEvent::FileOpen) {
            auto* fe = dynamic_cast<QFileOpenEvent*>(event);
            if (fe) {
                Application::instance().openFile(fe->file().toStdString());
            }
        }
        return QApplication::event(event);
    }
};

// ============================================================================
// Parse command line arguments
// ============================================================================
struct CommandLineArgs {
    std::vector<std::string> files;
    std::string configDir;
    std::string pluginDir;
    bool readOnly = false;
    bool newInstance = false;
    bool showHelp = false;
    bool noPlugins = false;
    std::string sessionToLoad;
    std::string profile;  // e.g. "dark", "light"
};

CommandLineArgs parseArgs(int argc, char* argv[]) {
    CommandLineArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help" || arg == "/?") {
            args.showHelp = true;
        } else if (arg == "-r" || arg == "--read-only") {
            args.readOnly = true;
        } else if (arg == "-n" || arg == "--new-instance") {
            args.newInstance = true;
        } else if (arg == "--no-plugins") {
            args.noPlugins = true;
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) args.configDir = argv[++i];
        } else if (arg == "-p" || arg == "--plugin-dir") {
            if (i + 1 < argc) args.pluginDir = argv[++i];
        } else if (arg == "-s" || arg == "--session") {
            if (i + 1 < argc) args.sessionToLoad = argv[++i];
        } else if (arg == "--profile") {
            if (i + 1 < argc) args.profile = argv[++i];
        } else if (!arg.empty() && arg[0] != '-') {
            args.files.push_back(arg);
        }
    }
    return args;
}

void printHelp(const char* programName) {
    std::cout << APP_NAME << " v" << APP_VERSION << " - A Qt-based text editor\n\n"
              << "Usage: " << programName << " [options] [files...]\n\n"
              << "Options:\n"
              << "  -h, --help         Show this help message\n"
              << "  -r, --read-only    Open files in read-only mode\n"
              << "  -n, --new-instance Always open a new instance\n"
              << "  -c, --config DIR   Use DIR as configuration directory\n"
              << "  -p, --plugin-dir DIR  Set plugin directory\n"
              << "  -s, --session FILE Load session from FILE\n"
              << "  --profile NAME     Use color theme profile (dark/light)\n";
}

// ============================================================================
// Global exception handler
// ============================================================================
void setupExceptionHandler() {
    std::set_terminate([]() {
        std::cerr << "FATAL: Unhandled exception\n";
        logError("Unhandled exception - application terminating");
        std::abort();
    });
}

// ============================================================================
// Main
// ============================================================================
int main(int argc, char* argv[]) {
    using namespace std::chrono;
    auto startTime = steady_clock::now();

    setupExceptionHandler();
    CommandLineArgs args = parseArgs(argc, argv);

    if (args.showHelp) {
        printHelp(argv[0]);
        return 0;
    }

    // Set application identity
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName("Agent Army");
    QCoreApplication::setOrganizationDomain("agent-army.example.com");

    // Qt styles
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    NotepadApp app(argc, argv);

    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Initialize application
    auto& application = Application::instance();
    if (!args.configDir.empty()) {
        application.setConfigDirectory(args.configDir);
    }
    if (!args.pluginDir.empty()) {
        application.setPluginDirectory(args.pluginDir);
    }
    if (!args.profile.empty()) {
        application.setThemeProfile(args.profile);
    }
    if (args.noPlugins) {
        application.setSkipPlugins(true);
    }

    // Load session if specified
    if (!args.sessionToLoad.empty()) {
        application.loadSession(args.sessionToLoad);
    }

    // Initialize
    if (!application.initialize()) {
        QMessageBox::critical(nullptr, "Startup Error",
            "Failed to initialize application.\n" + application.lastError());
        return 1;
    }

    // Open files from command line
    for (const auto& filePath : args.files) {
        application.openFile(filePath, args.readOnly);
    }

    // If no files, create empty document
    if (args.files.empty() && args.sessionToLoad.empty()) {
        application.newFile();
    }

    // Show main window
    // NOTE: showMainWindow() is already called inside initialize().
    // Calling it here again causes a double-show / double-exec crash.

    // Main event loop
    int result = app.exec();

    // Cleanup
    application.shutdown();

    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - startTime).count();
    logInfo(StringHelper::format("Application shutdown after %lld ms", (long long)elapsed));

    return result;
}
