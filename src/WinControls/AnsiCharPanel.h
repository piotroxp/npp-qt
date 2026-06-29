// Semantic Lift: Win32 AnsiCharPanel → Qt6 implementation
// Original: PowerEditor/src/WinControls/AnsiCharPanel/*.h/*.cpp
// Target: npp-qt/src/WinControls/AnsiCharPanel.h + .cpp

#pragma once

#include "DockingWnd.h"
#include "Window.h"
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>

// =============================================================================
// Constants (mirrors Win32 ansiCharPanel_rc.h)
// =============================================================================

constexpr int IDD_ANSIASCII_PANEL = 2700;
constexpr int IDC_LIST_ANSICHAR   = IDD_ANSIASCII_PANEL + 1;

// Forward declaration — AsciiListView is defined in AnsiCharPanel/asciiListView.h
// and the .cpp includes it directly.
class AsciiListView;

// =============================================================================
// AnsiCharPanel — docking panel showing ASCII character table
// =============================================================================

class AnsiCharPanel : public DockingDlgInterface
{
    Q_OBJECT

public:
    AnsiCharPanel();
    ~AnsiCharPanel() override = default;

    void init(QApplication* app, QWidget* parent, void* ppEditView);
    void setParent(QWidget* parent2set);

    void switchEncoding();
    void insertChar(unsigned char char2insert);
    void insertString(const QString& string2insert);

    void setBackgroundColor(const QColor& bgColour);
    void setForegroundColor(const QColor& fgColour);

    // Win32 compat: setText(bool) — show/hide panel
    void setText(bool show = true) { display(show); }
    void setText(const QString& /*title*/) { /* stub */ }

signals:
    // Emitted when the user double-clicks a character cell.
    // MainWindow connects this and sends to Scintilla.
    void charInsertRequested(const QByteArray& bytes);

    // Emitted when the user presses Return on a row.
    void rowActivated(int rowIndex);

public slots:
    void onItemDoubleClicked(int row, int col);
    void onItemActivated(int row);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    void* _ppEditView = nullptr;
    AsciiListView* _listView = nullptr;
    QColor _fgColor;
    QColor _bgColor;
    QWidget* _hSelf = nullptr;
    QWidget* _hParent = nullptr;
    QApplication* _hInst = nullptr;
    bool _isCreated = false;
};
