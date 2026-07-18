// BabyGridWrapper.h - Qt6 wrapper for BabyGrid (MFC grid control for shortcut display)
// INTENT: source uses a raw Win32 custom-drawn grid control (BGM_* messages).
// Target uses QTableWidget as the backing widget to expose the same API surface.
// Notepad++ ShortcutMapper uses BabyGridWrapper; other dialogs can use it too.

#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QHeaderView>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QColor>
#include <optional>

// Match Notepad++ cell struct
struct BabyCell {
    int row = 0;
    int col = 0;
};

// BabyGridWrapper wraps QTableWidget to provide a BabyGrid-compatible API.
// All dimensions are 1-based to match BabyGrid's API (row 0 = header row).
class BabyGridWrapper : public QWidget {
    Q_OBJECT

public:
    explicit BabyGridWrapper(QWidget* parent = nullptr);
    ~BabyGridWrapper() override = default;

    // --- BabyGrid-compatible API ---

    // Initialize with parent window and integer ID
    void init(QWidget* parent, int id);

    // Grid dimensions (rows include header row at index 0)
    void setGridDimensions(size_t nbRow, size_t nbCol);
    int numberOfRows() const;
    int numberOfCols() const;

    // Cell data
    void setCellText(int row, int col, const QString& text);
    QString cellText(int row, int col) const;
    void deleteCell(int row, int col);

    // Row/column management
    void setColWidth(unsigned int col, unsigned int width);
    void setRowHeight(unsigned int row, unsigned int height);
    void setHeaderHeight(size_t h);
    void clearGrid();

    // Auto-sizing
    void setAutoRow(bool on);
    void setColsNumbered(bool on);
    void setColAutoWidth(bool on);

    // Colors
    void setCursorColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setBackgroundColor(const QColor& color);
    void setHighlightColor(const QColor& color);
    void setHighlightTextColor(const QColor& color);
    void setGridlinesColor(const QColor& color);
    void setUnprotectColor(const QColor& color);
    void setProtectColor(const QColor& color);
    void setTitleTextColor(const QColor& color);
    void setTitleColor(const QColor& color);
    void setHighlightColorNoFocus(const QColor& color);
    void setHighlightColorProtect(const QColor& color);
    void setHighlightColorProtectNoFocus(const QColor& color);

    // Selection & cursor
    int selectedRow() const;
    int selectedCol() const;
    void hideCursor();

    // Fonts
    void setHeaderFont(const QFont& font);
    void setRowFont(const QFont& font);

    // View state
    void setHomeRow(size_t row);
    size_t homeRow() const;
    void setLastView(size_t homeRow, size_t cursorRow) const;
    void setInitialContent(bool on);

    // Selection / highlight
    void setCurrentCell(int row, int col);
    void setHighlightRow(int row);

    // Update
    void updateView();

    // Qt accessor for internal widget
    QTableWidget* tableWidget() { return _table; }
    const QTableWidget* tableWidget() const { return _table; }

signals:
    // BabyGrid notification signals
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);
    void rowChanged(int row);
    void colChanged(int col);
    void selectionChanged();

private:
    void setupConnections();

    QTableWidget* _table = nullptr;
    QColor _cursorColor{255, 255, 255};
    QColor _textColor{0, 0, 0};
    QColor _highlightColor{0, 0, 128};
    QColor _highlightTextColor{255, 255, 255};
    bool _colsNumbered = false;
    bool _colsAutoWidth = false;
    std::optional<int> _highlightRow;
    mutable std::optional<std::pair<size_t, size_t>> _lastView;
};
