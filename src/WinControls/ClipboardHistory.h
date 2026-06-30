// Semantic Lift: Win32 ClipboardHistoryPanel → Qt6 QListWidget panel
// Original:  PowerEditor/src/WinControls/ClipboardHistory/clipboardHistoryPanel.h
// Target:    npp-qt/src/WinControls/ClipboardHistory.h
//
// Translates:
//   CreateWindowEx(WC_LISTBOX)      → QListWidget
//   WM_CHANGECBCHAIN / WM_DRAWCLIPBOARD → QClipboard::changed signal
//   WM_NOTIFY(LBN_DBLCLK)            → QListWidget::itemDoubleClicked signal
//   LPNMITEMACTIVATE                 → QListWidgetItem* + QMouseEvent
//   HMENU context menus              → QMenu with QAction
//   ClipboardViewer chain            → QApplication::clipboard() monitoring
//   Custom draw (WM_DRAWITEM)        → QAbstractItemDelegate::paint()
//   SetClipboardViewer / ChangeClipboardChain → N/A (Qt handles clipboard internally)
//   drawItem()                      → ClipboardListDelegate::paint()

#pragma once

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVector>
#include <QString>
#include <QColor>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QAbstractItemDelegate>
#include <QStyleOptionViewItem>
#include <QStyle>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>

#include "DockingWnd.h"

// =============================================================================
// Win32 API stubs used by this class
// =============================================================================
#define IDM_CLIPBOARD_FIND_DATA   0x1000
#define IDM_CLIPBOARD_CLEAR       0x1001
#define IDM_CLIPBOARD_PASTE       0x1002

// =============================================================================
// clipboardFormatNppTextLen — N++ internal clipboard format
// Win32:  RegisterClipboardFormat(CF_NPPTEXTLEN)
// Qt6:    QMimeData custom format (application/x-notepadpp-textlen)
// =============================================================================
static inline QString clipboardFormatNppTextLen() {
    return QStringLiteral("application/x-notepadpp-textlen");
}

// =============================================================================
// ClipboardDataInfo — raw clipboard data + binary flag
// Win32:  stores HGLOBAL bytes from GetClipboardData()
// Qt6:    stores QVector<unsigned char> from QMimeData
// =============================================================================
struct ClipboardDataInfo {
    QVector<unsigned char> _data;
    bool _isBinaryContained = false;
};

// =============================================================================
// ByteArray — owned byte buffer
// Win32:  new unsigned char[GlobalAlloc] copy of HGLOBAL data
// Qt6:    heap-allocated unsigned char[]
// =============================================================================
class ByteArray {
public:
    ByteArray() = default;
    explicit ByteArray(const ClipboardDataInfo& cd);

    ~ByteArray() {
        if (_pBytes)
            delete[] _pBytes;
    }

    // Prevent copy (expensive); allow move
    ByteArray(ByteArray&& other) noexcept
        : _pBytes(other._pBytes), _length(other._length) {
        other._pBytes = nullptr;
        other._length = 0;
    }
    ByteArray& operator=(ByteArray&& other) noexcept {
        if (this != &other) {
            if (_pBytes) delete[] _pBytes;
            _pBytes = other._pBytes;
            _length = other._length;
            other._pBytes = nullptr;
            other._length = 0;
        }
        return *this;
    }

    const unsigned char* getPointer() const { return _pBytes; }
    size_t getLength() const { return _length; }

protected:
    unsigned char* _pBytes = nullptr;
    size_t _length = 0;
};

// =============================================================================
// StringArray — byte buffer displayed as a truncated string
// Win32:  copies raw bytes, nulls last few bytes and appends "..."
// Qt6:    same logic, truncates to maxLen and appends "..."
// =============================================================================
class StringArray : public ByteArray {
public:
    StringArray(const ClipboardDataInfo& cd, size_t maxLen);
};

// =============================================================================
// ClipboardListDelegate — custom paint for list items with dark/light theme
// Win32:  WM_DRAWITEM + SetTextColor/SetBkColor + DrawText
// Qt6:    QAbstractItemDelegate::paint()
// =============================================================================
class ClipboardListDelegate : public QAbstractItemDelegate {
    Q_OBJECT
public:
    explicit ClipboardListDelegate(QObject* parent = nullptr)
        : QAbstractItemDelegate(parent), _bgColor(Qt::white), _fgColor(Qt::black) {}

    void setBackgroundColor(const QColor& c) { _bgColor = c; }
    void setForegroundColor(const QColor& c) { _fgColor = c; }

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem&,
                   const QModelIndex&) const override {
        return {0, 22};
    }

private:
    QColor _bgColor;
    QColor _fgColor;
};

// =============================================================================
// ClipboardHistoryPanel — docking clipboard history panel
// Win32:  ClipboardViewer chain window with ListBox custom draw
// Qt6:    QListWidget subclass + QClipboard monitoring
// =============================================================================
class ClipboardHistoryPanel : public DockingDlgInterface {
    Q_OBJECT

public:
    ClipboardHistoryPanel();
    ~ClipboardHistoryPanel() override;

    // Win32 compat: init(HINSTANCE, HWND, ScintillaEditView**)
    void init(HINSTANCE hInst, HWND hPere, ScintillaEditView** ppEditView);

    // Dark mode colors — mirrors Win32 setBackgroundColor / setForegroundColor
    void setBackgroundColor(const QColor& bgColor) override;
    void setForegroundColor(const QColor& fgColor) override;

    // Track clipboard ops toggle
    bool trackClipboardOps(bool bTrack);

    // Add/remove clipboard entries
    void addToClipboadHistory(const ClipboardDataInfo& cbd);
    int getClipboardDataIndex(const ClipboardDataInfo& cbd);

    // Win32 compat: display(bool)
    void display(bool toShow = true) override;

    // Internal message dispatcher (Win32 run_dlgProc equivalent)
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    // Get/set the list widget (for dark mode scrollbar)
    QListWidget* getListWidget() const { return _listWidget; }

signals:
    // Emitted when user double-clicks a clipboard item
    void itemDoubleClicked(int index);
    // Emitted when user pastes (Enter key on selected item)
    void itemActivated(int index);

private slots:
    void onClipboardChanged(QClipboard::Mode mode);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);
    void onItemActivated(QListWidgetItem* item);

protected:
    // Context menu: right-click on list item
    void contextMenuEvent(QContextMenuEvent* event) override;
    // Keyboard: Enter to paste, Delete to remove
    void keyPressEvent(QKeyEvent* event) override;
    // WM_SIZE: resize list to fill panel
    void resizeEvent(QResizeEvent* event) override;

private:
    // Read current system clipboard into ClipboardDataInfo
    ClipboardDataInfo getClipboadData();
    // Remove item at index from list widget and internal vector
    void removeItemAt(int index);
    // Insert clipboard data as top item in the list
    void insertTopItem(const ClipboardDataInfo& cbd);

    // Build display string for a ClipboardDataInfo (max MAX_DISPLAY_LENGTH chars)
    QString buildDisplayString(const ClipboardDataInfo& cbd);

    // Paste item at row into Scintilla editor
    void pasteItemAt(int row);

    // Win32 compat: internal run_dlgProc body
    intptr_t run_dlgProcImpl(unsigned int message, intptr_t wParam, intptr_t lParam);

    QListWidget* _listWidget = nullptr;
    ClipboardListDelegate* _delegate = nullptr;
    ScintillaEditView** _ppEditView = nullptr;
    QVector<ClipboardDataInfo> _clipboardDataInfos;

    // Colors — -1 means "use default"
    int _lbBgColor = -1;  // -1 = white, else RGB
    int _lbFgColor = -1;  // -1 = black, else RGB
    bool _isTrackingClipboardOps = true;

    // Context menu
    QMenu* _contextMenu = nullptr;
    QAction* _actPaste = nullptr;
    QAction* _actClear = nullptr;
    QAction* _actFind = nullptr;

    static constexpr size_t MAX_DISPLAY_LENGTH = 64;
};
