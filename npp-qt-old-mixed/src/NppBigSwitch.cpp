// NppBigSwitch.cpp — Qt6 implementation of Notepad++ command dispatcher
#include "NppBigSwitch.h"
#include "NppCommands.h"
#include "Notepad_plus.h"
#include "NppDarkMode.h"
#include "MISC/Common/Common.h"

#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QProcess>
#include <QWidget>
#include <QWindow>

// ============================================================================
// Construction
// ============================================================================
NppBigSwitch::NppBigSwitch(QObject* parent)
    : QObject(parent)
    , _currentDpi(96)
{
    registerDefaultHandlers();
}

NppBigSwitch::~NppBigSwitch() = default;

// ============================================================================
// Event Filter — routes Qt events to handlers
// In npp-qt, most menu commands are wired via QAction::triggered() signals
// directly to NppCommands slots. This filter handles custom events and
// system-level events that need special routing.
// ============================================================================
bool NppBigSwitch::eventFilter(QObject* watched, QEvent* event)
{
    if (!watched || !event)
        return false;

    QEvent::Type type = event->type();

    // Handle custom Npp events
    if (type >= QEvent::User && type < QEvent::MaxUser) {
        // Check if it's one of our custom Npp events
        int nppId = static_cast<int>(type) - QEvent::User;
        if (nppId >= 0 && nppId < 0x10000) {
            // Menu command range
            if (nppId >= MenuCommandBase && nppId < MenuCommandMax) {
                handleCommand(nppId);
                return true;
            }
            // Custom event
            run_dlgProc(nppId, event);
            return true;
        }
    }

    // DPI changed — forwarded from QGuiApplication
    if (type == QEvent::ThemeChange || type == QEvent::ApplicationFontChange) {
        int newDpi = qApp->devicePixelRatio() * 96;
        if (newDpi != _currentDpi) {
            _currentDpi = newDpi;
            handleDpiChanged(newDpi, QRect());
        }
    }

    // Language changed
    if (type == QEvent::LocaleChange) {
        handleSettingChange("Locale");
    }

    return false;  // Don't filter — let the event propagate
}

// ============================================================================
// Custom event handling — replaces Win32 RegisterWindowMessage dispatch
// ============================================================================
intptr_t NppBigSwitch::run_dlgProc(int eventId, QEvent* event)
{
    // Menu command range — dispatch to NppCommands
    if (eventId >= MenuCommandBase && eventId < MenuCommandMax) {
        handleCommand(eventId);
        return 0;
    }

    // Handle specific custom events
    switch (eventId) {
        case TaskbarButtonCreated:
            emit taskbarButtonCreated();
            break;

        case RefreshDarkMode:
            refreshDarkMode();
            break;

        case ReloadScrollToEnd:
            refreshDarkMode();  // No-op in Qt6 (scrollbar handled by Scintilla)
            break;

        case FilePrint:
            // File → Print — the printer dialog is handled by QAction
            // connected directly to NppCommands::filePrint()
            break;

        case DoOpen: {
            // Inter-process file open via command line args
            // Handled in main.cpp on startup
            break;
        }

        default:
            break;
    }

    emit nppEventReceived(eventId);
    return 0;
}

// ============================================================================
// Command dispatch — replaces switch(id) { case IDM_*: ... }
// ============================================================================
void NppBigSwitch::registerCommand(int commandId, const std::function<void()>& handler)
{
    _commandHandlers.insert(commandId, handler);
}

void NppBigSwitch::unregisterCommand(int commandId)
{
    _commandHandlers.remove(commandId);
}

void NppBigSwitch::registerDefaultHandlers()
{
    // Default handlers are registered by NppCommands::initMenu()
    // via registerCommand() calls for each menu action.
    // This method can set up any global/default handlers here.
}

bool NppBigSwitch::handleCommand(int commandId)
{
    auto it = _commandHandlers.find(commandId);
    if (it != _commandHandlers.end()) {
        it.value()();  // Execute the handler
        emit commandDispatched(commandId);
        return true;
    }

    // Unknown command — log but don't crash
    qWarning("NppBigSwitch: unhandled command ID 0x%04X", commandId);
    return false;
}

// ============================================================================
// WM_SIZE handler — resize notification
// ============================================================================
bool NppBigSwitch::handleSize(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
    // Handled directly by Qt's QResizeEvent on the MainWindow
    // No need for special handling here
    return false;
}

// ============================================================================
// WM_MOVE handler — position notification
// ============================================================================
bool NppBigSwitch::handleMove(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    // Handled directly by Qt's QMoveEvent on the MainWindow
    return false;
}

// ============================================================================
// WM_DRAWITEM — owner-drawn controls (toolbar, menus)
// Qt handles this via QStyle::drawControl + QStyleOption
// ============================================================================
bool NppBigSwitch::handleDrawItem(QEvent* event)
{
    Q_UNUSED(event);
    // Owner-drawn controls are handled by Qt's style system
    // No special handling needed in Qt6
    return false;
}

// ============================================================================
// WM_DPICHANGED — DPI scaling change
// ============================================================================
bool NppBigSwitch::handleDpiChanged(int newDpi, const QRect& suggestedRect)
{
    _currentDpi = newDpi;
    emit dpiChanged(newDpi);

    // Notify the main window to resize/repaint
    if (QWidget* mainWin = qApp->activeWindow()) {
        if (!suggestedRect.isNull()) {
            mainWin->setGeometry(suggestedRect);
        }
        mainWin->update();
    }
    return true;
}

// ============================================================================
// WM_COPYDATA — inter-process file open (another instance)
// Qt equivalent: QLocalServer or QSharedMemory
// ============================================================================
bool NppBigSwitch::handleCopyData(QEvent* event)
{
    Q_UNUSED(event);
    // Inter-process file open is handled by the main() single-instance check
    // (QLocalServer listening for connections)
    return false;
}

// ============================================================================
// WM_SETTINGCHANGE — system settings changed (locale, theme, etc.)
// ============================================================================
bool NppBigSwitch::handleSettingChange(const QString& settingName)
{
    if (settingName == "Locale" || settingName.isEmpty()) {
        // Language changed — reload translations
        // NppParameters::resetalien() handles this
        qApp->setFont(qApp->font());  // Force UI refresh
    }
    if (settingName.contains("Environment", Qt::CaseInsensitive)) {
        // System theme changed — check dark mode
        NppDarkMode::instance().setEnabled(
            NppDarkMode::instance().isEnabled());
    }
    return true;
}

// ============================================================================
// WM_SYSCOMMAND — system commands (minimize, maximize, etc.)
// ============================================================================
bool NppBigSwitch::handleSysCommand(int sysCommand)
{
    Q_UNUSED(sysCommand);
    // Qt handles SYSCOMMAND via QWidget::close() / showMinimized() etc.
    return false;
}

// ============================================================================
// WM_NCACTIVATE — non-client activation
// ============================================================================
bool NppBigSwitch::handleNcActivate(bool activate)
{
    Q_UNUSED(activate);
    // Inactive window title bar styling is handled by Qt's window framework
    return false;
}

// ============================================================================
// WM_APPCOMMAND — app commands (media keys, browser back/forward)
// ============================================================================
bool NppBigSwitch::handleAppCommand(int cmd)
{
    // Map APPCOMMAND_* to NPP commands
    // APPCOMMAND_BROWSER_BACK -> IDM_FILE_RELOAD
    // APPCOMMAND_BROWSER_FORWARD -> (no direct equivalent)
    // APPCOMMAND_BROWSER_REFRESH -> IDM_FILE_RELOAD
    // APPCOMMAND_PRINT -> IDM_FILE_PRINT
    // etc.
    // Not commonly used on Linux, but handled here for completeness
    Q_UNUSED(cmd);
    return false;
}

// ============================================================================
// WM_MOUSEWHEEL with Ctrl — zoom
// ============================================================================
bool NppBigSwitch::handleMouseWheelZoom(int delta)
{
    Q_UNUSED(delta);
    // Zoom is handled directly by ScintillaEditView::wheelEvent()
    // Ctrl+Wheel is intercepted by the Scintilla component
    return false;
}

// ============================================================================
// WM_TIMER handler
// ============================================================================
bool NppBigSwitch::handleTimer(int timerId)
{
    Q_UNUSED(timerId);
    // Timers are handled by QObject::timerEvent() on specific objects
    return false;
}

// ============================================================================
// Dark mode refresh
// ============================================================================
void NppBigSwitch::refreshDarkMode()
{
    // Re-apply dark mode to all widgets
    // This is triggered by NPPM_INTERNAL_REFRESHDARKMODE
    NppDarkMode::instance().setEnabled(
        NppDarkMode::instance().isEnabled());

    // Also notify all widgets
    qApp->setStyleSheet(qApp->styleSheet());
}

// ============================================================================
// Static helpers
// ============================================================================
QEvent* NppBigSwitch::createNppEvent(int nppEventId, const QVariant& data)
{
    return new NppCustomEvent(nppEventId, data);
}

int NppBigSwitch::nppEventIdFromQEvent(QEvent::Type type)
{
    if (type >= QEvent::User && type < QEvent::MaxUser) {
        return static_cast<int>(type) - QEvent::User;
    }
    return -1;
}
