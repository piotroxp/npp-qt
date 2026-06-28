// Semantic Lift: Win32 AnsiCharPanel → Qt6 implementation
// Original: PowerEditor/src/WinControls/AnsiCharPanel/*.h/*.cpp
// Target: npp-qt/src/WinControls/AnsiCharPanel.h + .cpp

#pragma once

#include "StaticDialog.h"
#include "Window.h"
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>

// =============================================================================
// Constants (mirrors Win32 ansiCharPanel_rc.h)
// =============================================================================

constexpr int IDD_ANSIASCII_PANEL = 2700;
constexpr int IDC_LIST_ANSICHAR   = IDD_ANSIASCII_PANEL + 1;

// =============================================================================
// columnInfo — describes a table column
// =============================================================================

struct columnInfo {
    int _width = 0;
    QString _label;

    columnInfo() = default;
    columnInfo(const QString& label, int width) : _width(width), _label(label) {}
};

// =============================================================================
// ListView — custom table widget for ASCII panel
// Extends Window; mimics Win32 ListView with columns and item selection
// =============================================================================

class ListView : public Window
{
    Q_OBJECT

public:
    enum SortDirection {
        sortEncrease = 0,
        sortDecrease = 1
    };

    ListView(QWidget* parent = nullptr);
    ~ListView() override = default;

    // addColumn() must be called before init()
    void addColumn(const columnInfo& col2Add);
    void setColumnText(size_t i, const QString& txt2Set);

    size_t nbItem() const;
    long getSelectedIndex() const;
    void setSelection(int itemIndex);
    void clear();
    bool removeFromIndex(size_t i);

    void init(void* hInst, QWidget* hParent) override;
    void destroy() override;

    // Internal table access for AnsiCharPanel
    QTableWidget* table() { return _table; }
    const QTableWidget* table() const { return _table; }

signals:
    void itemDoubleClicked(int row, int col);
    void itemActivated(int row);
    void keyPressed(int key);
    void customContextMenuRequested(const QPoint& pos);

protected:
    QVector<columnInfo> _columnInfos;
    QTableWidget* _table = nullptr;
};

// =============================================================================
// AsciiListView — list view populated with ASCII character data
// =============================================================================

class AsciiListView : public ListView
{
    Q_OBJECT

public:
    AsciiListView(QWidget* parent = nullptr);
    ~AsciiListView() override = default;

    void setValues(int codepage = 0);
    void resetValues(int codepage);

    static QString getAscii(unsigned char value);
    static QString getHtmlName(unsigned char value);
    static int getHtmlNumber(unsigned char value);

private:
    int _codepage = -1;
};

// =============================================================================
// AnsiCharPanel — docking panel showing ASCII character table
// =============================================================================

class AnsiCharPanel : public StaticDialog
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
};
