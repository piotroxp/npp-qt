// Semantic Lift: Win32 BabyGrid → Qt6 BabyGrid
// Original: PowerEditor/src/WinControls/Grid/BabyGrid.h
// Target: npp-qt/src/WinControls/Grid/BabyGrid.h
//
// BabyGrid is a custom owner-draw spreadsheet-like control.
// Ported to Qt6 using QTableWidget as the base widget,
// with additional custom painting for title, headers, and grid cells.
//
// BabyGrid message IDs (BGM_*) are implemented as methods/signals.

#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QRect>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QString>

// =============================================================================
// BabyGrid message constants (mirrors Win32 BABYGRID_USER + BGM_*)
// =============================================================================

constexpr int BABYGRID_USER = 0x0400 + 7000;

#define BGM_SETGRIDDIM               (BABYGRID_USER +  6)
#define BGM_SETCOLWIDTH             (BABYGRID_USER + 11)
#define BGM_SETROWHEIGHT             (BABYGRID_USER + 16)
#define BGM_SETTITLEHEIGHT           (BABYGRID_USER + 33)
#define BGM_SETCOLAUTOWIDTH          (BABYGRID_USER + 41)
#define BGM_SHOWINTEGRALROWS        (BABYGRID_USER + 39)
#define BGM_SETELLIPSIS              (BABYGRID_USER + 40)

// Notify codes (mirrors BGN_*)
#define BGN_LBUTTONDOWN         0x0001
#define BGN_MOUSEMOVE           0x0002
#define BGN_OUTOFRANGE          0x0003
#define BGN_OWNERDRAW           0x0004
#define BGN_SELCHANGE           0x0005
#define BGN_ROWCHANGED          0x0006
#define BGN_COLCHANGED          0x0007
#define BGN_EDITBEGIN           0x0008
#define BGN_DELETECELL          0x0009
#define BGN_EDITEND             0x000A
#define BGN_CELLCLICKED         0x0019
#define BGN_CELLDBCLICKED       0x001A
#define BGN_CELLRCLICKED        0x001B

// =============================================================================
// BGCELL — cell coordinates (mirrors Win32 BGCELL)
// =============================================================================

struct BGCELL {
    int row = 0;
    int col = 0;
};

// =============================================================================
// CellData — stores text + metadata for a BabyGrid cell
// Cell format: "row|col|type|protection|text"
// =============================================================================

struct CellData {
    QString text;
    int iType = 1;      // 1=ALPHA, 2=NUMERIC, 3=BOOL_TRUE, 4=BOOL_FALSE, 5=USER_DRAWN
    int iProtection = 0; // 0=unprotected, 1=protected
};

// =============================================================================
// BabyGridDelegate — custom painting for grid cells
// Mirrors Win32 owner-draw: title bar, column headers, cell text, selection.
// =============================================================================

class BabyGridDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BabyGridDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                  const QModelIndex& index) const override;

    void setTitleText(const QString& text);
    void setTitleHeight(int h);
    void setHeaderHeight(int h);
    void setRowHeight(int h);
    void setTitleFont(const QFont& font);
    void setHeaderFont(const QFont& font);
    void setBodyFont(const QFont& font);

    void setTextColor(const QColor& c) { _textColor = c; }
    void setBackgroundColor(const QColor& c) { _bgColor = c; }
    void setGridlineColor(const QColor& c) { _gridColor = c; }
    void setHighlightColor(const QColor& c) { _highlightColor = c; }
    void setHighlightTextColor(const QColor& c) { _highlightTextColor = c; }
    void setTitleColor(const QColor& c) { _titleColor = c; }
    void setTitleTextColor(const QColor& c) { _titleTextColor = c; }
    void setTitleGridlineColor(const QColor& c) { _titleGridlineColor = c; }
    void setEllipsis(bool on) { _useEllipsis = on; }
    void setRowsNumbered(bool on) { _rowsNumbered = on; }
    void setColsNumbered(bool on) { _colsNumbered = on; }

signals:
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);
    void cellRightClicked(int row, int col);

protected:
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

private:
    void drawTitle(QPainter* p, const QRect& rect) const;
    void drawCell(QPainter* p, const QRect& rect, const QModelIndex& index,
                   bool isSelected, bool hasFocus) const;
    bool _useEllipsis = true;
    bool _rowsNumbered = true;
    bool _colsNumbered = true;
    int _titleHeight = 30;
    int _headerHeight = 20;
    int _rowHeight = 20;
    QString _titleText;
    QFont _titleFont;
    QFont _headerFont;
    QFont _bodyFont;
    QColor _textColor = Qt::black;
    QColor _bgColor = Qt::white;
    QColor _gridColor = QColor(220, 220, 220);
    QColor _highlightColor = QColor(0, 0, 128);
    QColor _highlightTextColor = Qt::white;
    QColor _titleColor;
    QColor _titleTextColor;
    QColor _titleGridlineColor;
};

// =============================================================================
// BabyGrid — spreadsheet-like custom control
// Mirrors Win32 BabyGrid class (a custom Win32 control).
// =============================================================================

class BabyGrid : public QTableWidget
{
    Q_OBJECT

public:
    BabyGrid(QWidget* parent = nullptr);
    ~BabyGrid() override = default;

    // === Win32 BGM_* equivalents ===

    void setGridDimensions(int rows, int cols);
    void setColumnWidth(unsigned int col, unsigned int width);
    int columnWidth(unsigned int col) const;
    void setRowHeight(unsigned int row, unsigned int height);
    int rowHeight(unsigned int row) const;
    void setTitleHeight(unsigned int h);
    void setTitleFont(const QFont& font);
    void setHeaderFont(const QFont& font);
    void setBodyFont(const QFont& font);
    void setTitle(const QString& text);

    void setTextColor(const QColor& c);
    void setBackgroundColor(const QColor& c);
    void setGridlineColor(const QColor& c);
    void setHighlightColor(const QColor& c);
    void setHighlightTextColor(const QColor& c);
    void setTitleColor(const QColor& c);
    void setTitleTextColor(const QColor& c);
    void setTitleGridlineColor(const QColor& c);

    void setCellText(int row, int col, const QString& text);
    QString cellText(int row, int col) const;
    void clearGrid();

    void setRowsNumbered(bool on);
    void setColsNumbered(bool on);
    void setEllipsis(bool on);
    void setAutoRow(bool on);
    void setColAutoWidth(bool on);
    void setShowIntegralRows(bool on);
    void setExtendLastColumn(bool on);

    // Get cursor position
    int cursorRow() const { return _cursorRow; }
    int cursorCol() const { return _cursorCol; }
    void setCursorPos(int row, int col);

    // Protection
    void protectCell(int row, int col, bool protect);
    bool isCellProtected(int row, int col) const;

    // Drawing
    void drawCursor();

signals:
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);
    void cellRightClicked(int row, int col);
    void selectionChanged();
    void editingFinished(int row, int col);
    void currentCellChanged(int currentRow, int currentCol);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool event(QEvent* event) override;

private:
    void updateCellRect();
    QPoint _cursorRect;  // XOR-drawn cursor position

    BabyGridDelegate* _delegate = nullptr;
    int _cursorRow = 1;
    int _cursorCol = 1;
    int _titleHeight = 30;
    int _headerHeight = 20;
    int _rowHeight = 20;
    int _titleFontHeight = 14;
    int _headerFontHeight = 12;
    int _bodyFontHeight = 12;

    bool _rowsNumbered = true;
    bool _colsNumbered = true;
    bool _ellipsis = true;
    bool _autoRow = true;
    bool _colAutoWidth = false;
    bool _showIntegralRows = true;
    bool _extendLastColumn = true;
    bool _gridHasFocus = false;

    QMap<QString, CellData> _cellData; // key = "row,col"
    QMap<QString, bool> _cellProtection; // key = "row,col"

    static QString makeKey(int row, int col) {
        return QStringLiteral("%1,%2").arg(row).arg(col);
    }
};

// Register BabyGrid class for use
void registerBabyGridClass();
