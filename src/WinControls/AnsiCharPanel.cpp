// Semantic Lift: Win32 AnsiCharPanel → Qt6 implementation
// Original: PowerEditor/src/WinControls/AnsiCharPanel/*.cpp
// Target: npp-qt/src/WinControls/AnsiCharPanel.h + .cpp

#include "AnsiCharPanel.h"
#include "NppDarkMode.h"
#include "DockingWnd.h"
#include "NppConstants.h"
#include "Notepad_plus_msgs.h"
#include "AnsiCharPanel/asciiListView.h"

// Win32 macro replacements for portability (LOWORD/HIWORD used in run_dlgProc)
#ifndef LOWORD
#define LOWORD(l) ((quint16)(((quintptr)(l)) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((quint16)((((quintptr)(l)) >> 16) & 0xFFFF))
#endif
#include <QApplication>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QKeyEvent>

// =============================================================================
// AnsiCharPanel
// =============================================================================

AnsiCharPanel::AnsiCharPanel()
    : DockingDlgInterface()
{
    setWindowTitle(QStringLiteral("ASCII Codes Insertion Panel"));
    _hSelf = this;
    _isCreated = true;
}

void AnsiCharPanel::init(QApplication* app, QWidget* parent, void* ppEditView)
{
    DockingDlgInterface::init(app, parent, 0);
    _hInst = app;
    _hParent = parent;
    _ppEditView = ppEditView;

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _listView = new AsciiListView(this);
    _listView->setObjectName(QString::number(IDC_LIST_ANSICHAR));
    _listView->init(app, this);

    // Add columns (matching the Win32 DialogTemplate)
    _listView->addColumn(columnInfo(QStringLiteral("Value"), 45));
    _listView->addColumn(columnInfo(QStringLiteral("Hex"), 45));
    _listView->addColumn(columnInfo(QStringLiteral("Character"), 70));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Name"), 90));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Decimal"), 100));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Hexadecimal"), 120));

    // Default codepage = 0 (system default / ASCII)
    _listView->setValues(0);
    _listView->table()->setColumnWidth(0, 45);
    _listView->table()->setColumnWidth(1, 45);
    _listView->table()->setColumnWidth(2, 70);
    _listView->table()->setColumnWidth(3, 90);
    _listView->table()->setColumnWidth(4, 100);
    _listView->table()->setColumnWidth(5, 120);

    // Connect double-click → insert char/string
    connect(_listView, &AsciiListView::itemDoubleClicked,
            this, [this](int row) {
                onItemDoubleClicked(row, 2);  // default to character column
            });

    // Connect Return/Enter key → insert char
    connect(_listView->table(), &QTableWidget::activated,
            this, [this](const QModelIndex& idx) {
                onItemActivated(idx.row());
            });

    layout->addWidget(_listView);
    setLayout(layout);

    _isCreated = true;
}

void AnsiCharPanel::setParent(QWidget* parent2set)
{
    _hParent = parent2set;
}

void AnsiCharPanel::switchEncoding()
{
    // When the editor's encoding changes, update the table.
    // Mirrors Win32 AnsiCharPanel::switchEncoding().
    // Full implementation: query current buffer codepage from ScintillaEditView.
    // For now, reset to default (system ANSI codepage).
    int codepage = 0; // 0 = default/system codepage
    _listView->resetValues(codepage);
}

void AnsiCharPanel::setBackgroundColor(const QColor& bgColour)
{
    _bgColor = bgColour;
    QPalette pal = _listView->palette();
    pal.setColor(QPalette::Base, bgColour);
    pal.setColor(QPalette::AlternateBase, bgColour.lighter(105));
    _listView->setPalette(pal);
    _listView->table()->setPalette(pal);
    _listView->table()->setAlternatingRowColors(true);
    update();
}

void AnsiCharPanel::setForegroundColor(const QColor& fgColour)
{
    _fgColor = fgColour;
    QPalette pal = _listView->palette();
    pal.setColor(QPalette::Text, fgColour);
    _listView->setPalette(pal);
    _listView->table()->setPalette(pal);
    update();
}

void AnsiCharPanel::insertChar(unsigned char char2insert)
{
    // Mirrors Win32: insert the character into the current Scintilla buffer.
    // In the Win32 version, this uses MultiByteToWideChar + SCI_ADDTEXT.
    // Here we emit a signal that MainWindow connects to ScintillaEditView.
    QByteArray data;
    if (char2insert < 128) {
        data = QByteArray(1, static_cast<char>(char2insert));
    } else {
        // For non-ASCII (128-255), emit the raw byte.
        // MainWindow/ScintillaEditView will handle encoding conversion
        // based on the current buffer's codepage.
        data = QByteArray(1, static_cast<char>(char2insert));
    }

    // Real signal — MainWindow connects charInsertRequested to ScintillaEditView
    emit charInsertRequested(data);
}

void AnsiCharPanel::insertString(const QString& string2insert)
{
    // Mirrors Win32: insert a decoded string (HTML entity, etc.)
    emit charInsertRequested(string2insert.toUtf8());
}

void AnsiCharPanel::onItemDoubleClicked(int row, int col)
{
    // Mirrors Win32 NM_DBLCLK handler.
    // col == 2: insert the character at that row (ASCII value == row index).
    // other cols: insert the text from the cell.
    if (row < 0) return;
    if (col == 2) {
        insertChar(static_cast<unsigned char>(row));
    } else {
        QTableWidgetItem* item = _listView->table()->item(row, col);
        if (item)
            insertString(item->text());
    }
}

void AnsiCharPanel::onItemActivated(int row)
{
    // Mirrors Win32 VK_RETURN handler in TaskList.
    if (row < 0) return;
    insertChar(static_cast<unsigned char>(row));
}

intptr_t AnsiCharPanel::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            // Mirrors Win32 WM_INITDIALOG — dark mode theming applied in doDialog
            return TRUE;
        }

        case WM_SIZE: {
            // Mirrors Win32 WM_SIZE: resize list view to fill panel
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            Q_UNUSED(width);
            Q_UNUSED(height);
            if (_listView)
                _listView->resize(size());
            return TRUE;
        }

        case NPPM_INTERNAL_REFRESHDARKMODE: {
            // Re-apply dark mode to child controls
            // MainWindow connects NppDarkMode signals to this
            return TRUE;
        }

        default:
            break;
    }
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}