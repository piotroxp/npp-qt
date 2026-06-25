// Semantic Lift: Win32 BabyGridWrapper → Qt6 BabyGridWrapper
// Original: PowerEditor/src/WinControls/Grid/BabyGridWrapper.h
// Target: npp-qt/src/WinControls/Grid/BabyGridWrapper.h

#pragma once

#include "Window.h"
#include "BabyGrid.h"
#include <QWidget>

// BabyGridWrapper mirrors Win32 BabyGridWrapper:
// provides a thin Qt6 wrapper around BabyGrid with method-for-SendMessage
// style API for compatibility with ShortcutMapper code.

class BabyGridWrapper : public QWidget
{
    Q_OBJECT

public:
    BabyGridWrapper(QWidget* parent = nullptr);
    ~BabyGridWrapper() override = default;

    // init — creates BabyGrid child widget
    void init(QApplication* app, QWidget* parent, int id);

    // Mirrors Win32 SendMessage(..., BGM_SETGRIDDIM, rows, cols)
    void setLineColNumber(size_t nbRow, size_t nbCol) {
        if (_grid) _grid->setGridDimensions(static_cast<int>(nbRow), static_cast<int>(nbCol));
    }

    // Mirrors Win32 BGM_SETCURSORCOLOR
    void setCursorColour(const QColor& coulour) {
        if (_grid) {
            _grid->setHighlightColor(coulour);
            _grid->setHighlightTextColor(Qt::white);
        }
    }

    void hideCursor() {
        setCursorColour(Qt::black);
    }

    void setColsNumbered(bool isNumbered = true) {
        if (_grid) _grid->setColsNumbered(isNumbered);
    }

    // Mirrors Win32 BGM_SETCELLDATA: set cell text
    void setText(size_t row, size_t col, const QString& text) {
        if (_grid) _grid->setCellText(static_cast<int>(row), static_cast<int>(col), text);
    }

    // Mirrors Win32 BGM_SETCOLAUTOWIDTH
    void makeColAutoWidth(bool autoWidth = true) {
        if (_grid) _grid->setColAutoWidth(autoWidth);
    }

    // Mirrors Win32 BGM_GETROW
    int getSelectedRow() const {
        return _grid ? _grid->cursorRow() : 0;
    }

    // Mirrors Win32 BGM_DELETECELL
    void deleteCell(int row, int col) {
        if (_grid) _grid->setCellText(row, col, QString());
    }

    // Mirrors Win32 BGM_SETCOLWIDTH
    void setColWidth(unsigned int col, unsigned int width) {
        if (_grid) _grid->setColumnWidth(col, width);
    }

    void clear() {
        if (_grid) _grid->clearGrid();
    }

    // Mirrors Win32 BGM_GETROWS
    int getNumberRows() const {
        return _grid ? _grid->rowCount() : 0;
    }

    // Mirrors Win32 BGM_GETHOMEROW
    int getHomeRow() const {
        return 0;
    }

    // Mirrors Win32 BGM_SETLASTVIEW
    void setLastView(const size_t homeRow, const size_t cursorRow) const {
        if (_grid) _grid->setCursorPos(static_cast<int>(cursorRow), 1);
        Q_UNUSED(homeRow);
    }

    void updateView() const {
        if (_grid) _grid->update();
    }

    // Colour setters (mirrors BGM_SET*COLOR)
    void setTextColor(const QColor& color) const {
        if (_grid) _grid->setTextColor(color);
    }

    void setBackgroundColor(const QColor& color) const {
        if (_grid) _grid->setBackgroundColor(color);
    }

    void setUnprotectColor(const QColor& color) const {
        // Unprotect color not implemented in Qt6 version
        Q_UNUSED(color);
    }

    void setTitleTextColor(const QColor& color) const {
        if (_grid) _grid->setTitleTextColor(color);
    }

    void setTitleColor(const QColor& color) const {
        if (_grid) _grid->setTitleColor(color);
    }

    void setTitleGridlinesColor(const QColor& color) const {
        if (_grid) _grid->setTitleGridlineColor(color);
    }

    void setHighlightTextColor(const QColor& color) const {
        if (_grid) _grid->setHighlightTextColor(color);
    }

    void setHighlightColor(const QColor& color) const {
        if (_grid) _grid->setHighlightColor(color);
    }

    void setHighlightColorNoFocus(const QColor& color) const {
        Q_UNUSED(color);
    }

    void setProtectColor(const QColor& color) const {
        Q_UNUSED(color);
    }

    void setHighlightColorProtect(const QColor& color) const {
        Q_UNUSED(color);
    }

    void setHighlightColorProtectNoFocus(const QColor& color) const {
        Q_UNUSED(color);
    }

    void setGridlinesColor(const QColor& color) const {
        if (_grid) _grid->setGridlineColor(color);
    }

    bool setMarker(const bool isMarker) const {
        Q_UNUSED(isMarker);
        return true;
    }

    void setAutoRow(const bool isAutoRow) const {
        if (_grid) _grid->setAutoRow(isAutoRow);
    }

    void setInitialContent(const bool isInitialContent) const {
        Q_UNUSED(isInitialContent);
    }

    void setHeaderFont(const QFont& font) const {
        if (_grid) _grid->setHeaderFont(font);
    }

    void setRowFont(const QFont& font) const {
        if (_grid) _grid->setBodyFont(font);
    }

    void setHeaderHeight(const size_t headerHeight) const {
        Q_UNUSED(headerHeight);
    }

    void setRowHeight(const size_t rowHeight) const {
        // Applied per-row in ShortcutMapper
        Q_UNUSED(rowHeight);
    }

signals:
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);

private:
    BabyGrid* _grid = nullptr;
    bool _isRegistered = false;
};
