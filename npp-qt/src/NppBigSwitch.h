// npp-qt: Qt6 port of Notepad++ command dispatcher
// Semantic lift from: PowerEditor/src/NppBigSwitch.cpp
// Win32 message switch (switch(message)) → Qt event handlers
// RegisterWindowMessage → intptr_t run_dlgProc()

#pragma once

#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QHash>
#include <QSharedPointer>

// Forward declarations
class Notepad_plus;
class NppCommands;
class NppParameters;

// Custom Qt event types (replacing Win32 RegisterWindowMessage IDs)
namespace NppEvent {
    // Custom event IDs - avoid clashing with Qt's event types (0-999)
    constexpr int TaskbarCreated = 0x0801;   // Shell taskbar recreation
    constexpr int DpiChanged = 0x02E0;     // WM_DPICHANGED

    // Npp-internal custom events (replace WM_COMMAND range)
    constexpr int DockUserDefine = 0x1000;
    constexpr int UndockUserDefine = 0x1001;
    constexpr int RemoveUserLang = 0x1002;
    constexpr int RenameUserLang = 0x1003;
    constexpr int CloseUserDefine = 0x1004;
    constexpr int ReplaceAllInOpened = 0x1005;
    constexpr int FindAllInOpened = 0x1006;
    constexpr int FindAllInCurrent = 0x1007;
    constexpr int FindInFiles = 0x1008;
    constexpr int FindInProjects = 0x1009;
    constexpr int FindInCurrentFinder = 0x100A;
    constexpr int ReplaceInFiles = 0x100B;
    constexpr int ReplaceInProjects = 0x100C;
    constexpr int DoOpen = 0x100D;
    constexpr int FilePrint = 0x100E;
    constexpr int ReloadScrollToEnd = 0x100F;
    constexpr int RefreshDarkMode = 0x1010;
    constexpr int RestoreFromMinimized = 0x1011;
    constexpr int TaskbarButtonCreated = 0x1012;

    // Menu command range (maps to IDM_* from menuCmdID.h)
    constexpr int MenuCommandBase = 0x2000;
    constexpr int MenuCommandMax = 0x4000;
}

/**
 * NppBigSwitch — Qt6 port of the Win32 message dispatcher
 *
 * Win32 pattern:
 *   switch (message) { case WM_COMMAND: id = LOWORD(wParam); switch(id) { ... } }
 *
 * Qt6 pattern:
 *   QHash<int, std::function<void()>> commandMap;  // menu ID → handler
 *   event(e) { if (e->type() == QEvent::Type::User+1) dispatch(e); }
 *
 * Or simply connect menu QAction::triggered() signals to NppCommands slots directly.
 * This class provides the mapping infrastructure and custom event handling.
 */
class NppBigSwitch : public QObject
{
    Q_OBJECT

public:
    explicit NppBigSwitch(QObject* parent = nullptr);
    ~NppBigSwitch();

    /**
     * Main event handler — replaces the Win32 WndProc switch statement.
     * Called from MainWindow::event() to handle custom Npp events.
     *
     * @return true if the event was handled, false otherwise
     */
    bool handleNppEvent(QEvent* event);

    /**
     * Handle custom events (NppEvent::*) — replaces RegisterWindowMessage dispatch
     */
    intptr_t run_dlgProc(int eventId, QEvent* event);

    /**
     * Register a command handler for a menu ID
     */
    void registerCommand(int commandId, const std::function<void()>& handler);

    /**
     * Unregister a command handler
     */
    void unregisterCommand(int commandId);

    // Static: create custom Qt event
    static QEvent* createNppEvent(int nppEventId, const QVariant& data = QVariant());
    static int nppEventIdFromQEvent(QEvent::Type type);

signals:
    void nppEventReceived(int eventId);
    void commandDispatched(int commandId);
    void dpiChanged(int newDpi);
    void taskbarButtonCreated();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // Command handler map (replaces switch(id) { case IDM_*: ... })
    QHash<int, std::function<void()>> _commandHandlers;

    // Pointer to NppCommands (set during initialization)
    NppCommands* _pNppCommands = nullptr;

    // Register all standard command handlers
    void registerDefaultHandlers();

    // Handle WM_COMMAND (menu commands)
    bool handleCommand(int commandId);

    // Handle WM_SIZE
    bool handleSize(int w, int h);

    // Handle WM_MOVE / WM_MOVING
    bool handleMove(int x, int y);

    // Handle WM_DRAWITEM (owner-drawn controls)
    bool handleDrawItem(QEvent* event);

    // Handle WM_DPICHANGED
    bool handleDpiChanged(int newDpi, const QRect& suggestedRect);

    // Handle WM_COPYDATA (inter-process file open)
    bool handleCopyData(QEvent* event);

    // Handle WM_SETTINGCHANGE (system theme, locale, etc.)
    bool handleSettingChange(const QString& settingName);

    // Handle WM_SYSCOMMAND (window control commands)
    bool handleSysCommand(int sysCommand);

    // Handle WM_NCACTIVATE (non-client activation)
    bool handleNcActivate(bool activate);

    // Handle WM_APPCOMMAND (media keys, browser back/forward, etc.)
    bool handleAppCommand(int cmd);

    // Handle WM_MOUSEWHEEL with Ctrl (zoom)
    bool handleMouseWheelZoom(int delta);

    // Handle WM_TIMER (internal timers)
    bool handleTimer(int timerId);

    // Refresh dark mode
    void refreshDarkMode();

    // DPI management
    int _currentDpi = 96;  // Default DPI
};

// Custom Qt event type for Npp internal events
class NppCustomEvent : public QEvent
{
public:
    NppCustomEvent(int nppEventId, QVariant data = QVariant())
        : QEvent(static_cast<QEvent::Type>(QEvent::User + nppEventId))
        , _nppEventId(nppEventId)
        , _data(data)
    {}

    int nppEventId() const { return _nppEventId; }
    QVariant data() const { return _data; }

private:
    int _nppEventId;
    QVariant _data;
};
