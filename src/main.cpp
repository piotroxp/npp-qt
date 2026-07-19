// main.cpp - Application entry point for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFileOpenEvent>
#include <QTimer>
#include <QThread>
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
// Headless environment detection
// Runs a minimal Qt lifecycle probe BEFORE QApplication construction.
// QCoreApplication is safe to construct without a display — QGuiApplication
// (via QApplication) is what requires a platform plugin. We attempt to create
// an offscreen surface; if that throws, no display is available and we force
// the offscreen platform so QApplication's event-dispatcher init won't abort.
// ============================================================================
static void detectHeadlessEnvironment(int argc, char* argv[]) {
    // Skip if the user has already specified a platform.
    if (!qgetenv("QT_QPA_PLATFORM").isEmpty()) {
        return;
    }

    // Use DISPLAY/WAYLAND_DISPLAY as a proxy for display availability.
    // This avoids creating QOffscreenSurface (which crashes when no screen exists).
    // QCoreApplication requires a display to exist, so we can't probe with Qt.
    bool displayAvailable = (!qgetenv("DISPLAY").isEmpty()
                            || !qgetenv("WAYLAND_DISPLAY").isEmpty());

    if (!displayAvailable) {
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));
        qWarning() << "[Notepad--] No display detected (no DISPLAY/WAYLAND_DISPLAY)."
                      " Running in headless mode.";
    }
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
                // Guard: initialize() may not have run yet (event can fire during startup).
                // isInitialized() is safe to call — it only checks _state, not _fileManager.
                if (Application::instance().isInitialized()) {
                    Application::instance().openFile(fe->file().toStdString());
                } else {
                    qWarning() << "[App] QFileOpenEvent received before initialize, dropping:"
                               << fe->file();
                }
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

    // Probe for headless environment and set QT_QPA_PLATFORM=offscreen
    // before QApplication is constructed.
    detectHeadlessEnvironment(argc, argv);

    // Set application identity
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName("Agent Army");
    QCoreApplication::setOrganizationDomain("agent-army.example.com");

    // Qt styles
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    NotepadApp app(argc, argv);

    // Set up signal handlers that exit the Qt event loop
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // In headless/offscreen mode the window won't close to quit, so use a
    // periodic timer to poll signal flags and exit app.exec() cleanly.
    QTimer signalTimer;
    QObject::connect(&signalTimer, &QTimer::timeout, [&app, &signalTimer]() {
        if (g_sigintReceived || g_sigtermReceived) {
            signalTimer.stop();
            std::fflush(stderr);
            std::_Exit(EXIT_SUCCESS);
        }
    });
    signalTimer.start(500);  // Check every 500ms

    // In headless/offscreen mode with no file arguments, auto-exit after
    // initialisation.  This prevents the app from hanging forever when
    // invoked as a headless file processor (e.g. `npp-qt --no-plugins file.txt`).
    // A long timeout is used to give the window time to initialise before exiting.
    // When file arguments are provided, we let the event loop run normally
    // (the app will exit when all files are processed and the window is closed,
    // or timeout kills it).
    // Check: if QT_QPA_PLATFORM=offscreen, or if no display is available.
    bool isOffscreen = (qgetenv("QT_QPA_PLATFORM") == "offscreen")
                           || (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")
                               && qgetenv("DISPLAY").isEmpty()
                               && qgetenv("WAYLAND_DISPLAY").isEmpty());
    if (args.files.empty() && isOffscreen) {
        qDebug() << "[Notepad--] Headless mode: auto-exiting after 5 seconds";
        QTimer::singleShot(5000, &app, [&app]() {
            qDebug() << "[Notepad--] Headless timeout — exiting";
            // Use std::_Exit() to terminate immediately, bypassing Qt's shutdown.
            // This avoids QApplication::closeAllWindows() → closeEvent being called
            // on a partially-destroyed MainWindow (_tabWidget freed by deleteChildren),
            // which causes heap-use-after-free.  The GoToLineDialog double-destroy
            // is guarded by its own static guard, so std::_Exit() is safe here.
            std::fflush(stderr);
            std::_Exit(EXIT_SUCCESS);
        });
    }

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

    // Initialize
    if (!application.initialize()) {
        QMessageBox::critical(nullptr, "Startup Error",
            "Failed to initialize application.\n" + application.lastError());
        return 1;
    }

    // Load session if specified (must be after initialize() — needs _sessionManager and _fileManager)
    if (!args.sessionToLoad.empty()) {
        application.loadSession(args.sessionToLoad);
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
