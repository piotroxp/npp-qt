#pragma once

#include <QString>
#include <QStringList>
#include <QMetaType>
#include <QObject>

// Forward declare to avoid pulling in full scintilla headers
struct SCNotification;
class ScintillaComponent;

class NppNotification : public QObject
{
    Q_OBJECT
public:
    explicit NppNotification(QObject* parent = nullptr) : QObject(parent) {}

    // Emulate Windows NMHDR
    struct NMHDR {
        void* hwndFrom = nullptr;  // Not used in Qt
        int idFrom = 0;
        int code = 0;
    };

    // Scintilla notification data
    NMHDR nmhdr;
    int position = 0;
    int ch = 0;
    int modifiers = 0;
    int modificationType = 0;
    QString text;
    int length = 0;
    int linesAdded = 0;
    int message = 0;
    long long wParam = 0;
    long long lParam = 0;
    int line = 0;
    int foldLevelNow = 0;
    int foldLevelPrev = 0;
    int spaceBefore = 0;
    int linesVisible = 0;
    int wrappedLine = 0;
    int wrappedLineModified = 0;
    int id = 0;
    void* userData = nullptr;

    void fromScintilla(int id, void* pSC);
    void beNotifiedOf(SCNotification* scn);

    // =====================================================================
    // Notification handlers — Qt6 slot implementations
    // Called from MainWindow when connecting ScintillaComponent signals.
    // Each mirrors a Win32 SCN_* case from the original NppNotification.cpp.
    // =====================================================================

    // SCN_STYLENEEDED: Scintilla requests more text styled — trigger lexer
    void onStyleNeeded(ScintillaComponent* editor, size_t position);

    // SCN_UPDATEUI: document state changed — update status bar, brace match
    void onUpdateUI(ScintillaComponent* editor, int updated);

    // SCN_MODIFIED: document modified — dirty flag, tab name, link re-process
    void onModified(ScintillaComponent* editor,
                    int type, size_t position, size_t length,
                    const QString& text, size_t line,
                    int foldLevelNow, int foldLevelPrev);

    // SCN_SAVEPOINTREACHED / SCN_SAVEPOINTLEFT: save state changed
    void onSavePointReached(ScintillaComponent* editor);
    void onSavePointLeft(ScintillaComponent* editor);
    void handleSavePointChanged(ScintillaComponent* editor, bool dirty);

    // SCN_CHARADDED: char typed — auto-indent, auto-completion trigger
    void onCharAdded(ScintillaComponent* editor, int ch);

    // SCN_MARGINCLICK: margin clicked — bookmarks (margin 1) or folding (margin 3)
    void onMarginClicked(ScintillaComponent* editor, size_t position,
                          int modifiers, int margin);

    // SCN_DOUBLECLICK: double-click — URL open or delimiter selection
    void onDoubleClicked(ScintillaComponent* editor, size_t position, size_t line);

    // SCN_ZOOM: zoom level changed — emit zoomChanged signal
    void onZoomChanged(ScintillaComponent* editor, int zoom);

    // SCN_PAINTED: painting complete — update line numbers, doc map
    void onPainted(ScintillaComponent* editor);

    // SCN_AUTOCSELECTION: auto-complete item selected — enter/tab confirm
    void onAutoCSelection(ScintillaComponent* editor,
                          const QString& text, int method);

    // SCN_CALLTIPCLICK: user clicked in call tip — cycle overloads
    void onCallTipClick(ScintillaComponent* editor, int position);

    // SCN_MODIFYATTEMPTRO: tried to edit read-only buffer
    void onModifyAttemptRO(ScintillaComponent* editor);

    // SCN_NEEDSHOWN: lines need to be made visible (after fold or scroll)
    void onNeedShown(ScintillaComponent* editor, size_t position, size_t length);

signals:
    // Forwarded Scintilla notification signals
    void styleNeeded(const NppNotification& nn);
    void charAdded(const NppNotification& nn);
    void savePointReached(const NppNotification& nn);
    void savePointLeft(const NppNotification& nn);
    void modifyAttemptRO(const NppNotification& nn);
    void key(const NppNotification& nn);
    void doubleClick(const NppNotification& nn);
    void updateUI(const NppNotification& nn);
    void modified(const NppNotification& nn);
    void macroRecord(const NppNotification& nn);
    void marginClick(const NppNotification& nn);
    void needShown(const NppNotification& nn);
    void highlight(const NppNotification& nn);
    void cacheAllocated(const NppNotification& nn);
    void dwellStart(const NppNotification& nn);
    void dwellEnd(const NppNotification& nn);
    void zoom(const NppNotification& nn);
    void hotspotClick(const NppNotification& nn);
    void hotspotDoubleClick(const NppNotification& nn);
    void callTipClick(const NppNotification& nn);
    void autoCSelection(const NppNotification& nn);
    void indicatorClick(const NppNotification& nn);
    void indicatorComplete(const NppNotification& nn);
    void userListSelection(const NppNotification& nn);
    void uriDropped(const NppNotification& nn);
    void dwell(const NppNotification& nn);
    void lexerBuffered(const NppNotification& nn);
    void lexerDropped(const NppNotification& nn);
    void modeEventMaskChange(const NppNotification& nn);
    void keyReleased(const NppNotification& nn);
    void focusIn(const NppNotification& nn);
    void focusOut(const NppNotification& nn);
    void autoCCancelled(const NppNotification& nn);
    void autoCCharDeleted(const NppNotification& nn);
    void listX(const NppNotification& nn);
    void listY(const NppNotification& nn);
    void multiSelChange(const NppNotification& nn);
    void minimize(const NppNotification& nn);
    void maximize(const NppNotification& nn);
    void restore(const NppNotification& nn);
    void tabRightClick(const NppNotification& nn);
    void tabPinned(const NppNotification& nn);
};

// Central notification hub
class NppNotificationHub : public QObject
{
    Q_OBJECT
public:
    explicit NppNotificationHub(QObject* parent = nullptr);
    void notify(SCNotification* scn);
signals:
    void nppNotification(void* scn);
};

