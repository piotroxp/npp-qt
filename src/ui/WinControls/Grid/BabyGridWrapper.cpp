// BabyGridWrapper.cpp - Qt6 wrapper for BabyGrid
#include "BabyGridWrapper.h"
#include <QHeaderView>
#include <QKeyEvent>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QLineEdit>
#include <QApplication>

BabyGridWrapper::BabyGridWrapper(QWidget* parent)
    : QWidget(parent) {
    _table = new QTableWidget(this);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setShowGrid(true);
    _table->setAlternatingRowColors(false);
    _table->verticalHeader()->setVisible(false);
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _table->setFont(QApplication::font());
    setupConnections();
}

void BabyGridWrapper::setupConnections() {
    connect(_table, &QTableWidget::cellClicked,
            this, &BabyGridWrapper::cellClicked);
    connect(_table, &QTableWidget::cellDoubleClicked,
            this, &BabyGridWrapper::cellDoubleClicked);
    connect(_table->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex& current) {
                if (current.isValid())
                    emit rowChanged(current.row());
            });
}

void BabyGridWrapper::init(QWidget* parent, int /*id*/) {
    setParent(parent);
    _table->setParent(this);
}

void BabyGridWrapper::setGridDimensions(size_t nbRow, size_t nbCol) {
    _table->setRowCount(static_cast<int>(nbRow));
    _table->setColumnCount(static_cast<int>(nbCol));
    // Row 0 is the header row
    if (_colsNumbered) {
        for (int c = 0; c < nbCol; ++c) {
            QTableWidgetItem* item = _table->item(0, c);
            if (!item) {
                item = new QTableWidgetItem();
                _table->setItem(0, c, item);
            }
            item->setText(QString::number(c));
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        }
    }
}

int BabyGridWrapper::numberOfRows() const { return _table->rowCount(); }
int BabyGridWrapper::numberOfCols() const { return _table->columnCount(); }

void BabyGridWrapper::setCellText(int row, int col, const QString& text) {
    if (row < 0 || col < 0) return;
    if (row >= _table->rowCount()) _table->setRowCount(row + 1);
    if (col >= _table->columnCount()) return;
    QTableWidgetItem* item = _table->item(row, col);
    if (!item) {
        item = new QTableWidgetItem();
        _table->setItem(row, col, item);
    }
    item->setText(text);
    // Apply highlight row if set
    if (_highlightRow && row == *_highlightRow) {
        item->setBackground(_highlightColor);
        item->setForeground(_highlightTextColor);
    }
}

QString BabyGridWrapper::cellText(int row, int col) const {
    auto* item = _table->item(row, col);
    return item ? item->text() : QString();
}

void BabyGridWrapper::deleteCell(int row, int col) {
    if (auto* item = _table->item(row, col)) {
        item->setText(QString());
    }
}

void BabyGridWrapper::setColWidth(unsigned int col, unsigned int width) {
    if (static_cast<int>(col) < _table->columnCount()) {
        _table->setColumnWidth(static_cast<int>(col), static_cast<int>(width));
    }
}

void BabyGridWrapper::setRowHeight(unsigned int row, unsigned int height) {
    if (static_cast<int>(row) < _table->rowCount()) {
        _table->setRowHeight(static_cast<int>(row), static_cast<int>(height));
    }
}

void BabyGridWrapper::setHeaderHeight(size_t h) {
    _table->horizontalHeader()->setDefaultSectionSize(static_cast<int>(h));
}

void BabyGridWrapper::clearGrid() {
    _table->clear();
    // Restore header row
    if (_colsNumbered) {
        for (int c = 0; c < _table->columnCount(); ++c) {
            if (auto* item = _table->item(0, c)) {
                item->setText(QString::number(c));
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            }
        }
    }
}

void BabyGridWrapper::setAutoRow(bool on) {
    // Auto-row: automatically scroll to keep selected row visible
    if (on) {
        connect(_table->selectionModel(), &QItemSelectionModel::currentChanged,
                this, [this]() { _table->scrollTo(_table->currentIndex()); });
    }
}

void BabyGridWrapper::setColsNumbered(bool on) {
    _colsNumbered = on;
    if (_table->rowCount() > 0) {
        for (int c = 0; c < _table->columnCount(); ++c) {
            if (auto* item = _table->item(0, c)) {
                item->setText(on ? QString::number(c) : QString());
            }
        }
    }
}

void BabyGridWrapper::setColAutoWidth(bool on) {
    _colsAutoWidth = on;
    if (on) {
        _table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
}

void BabyGridWrapper::setCursorColor(const QColor& color) {
    _cursorColor = color;
}

void BabyGridWrapper::setTextColor(const QColor& color) {
    _textColor = color;
    // Apply to all existing items
    for (int r = 0; r < _table->rowCount(); ++r) {
        for (int c = 0; c < _table->columnCount(); ++c) {
            if (auto* item = _table->item(r, c)) {
                item->setForeground(color);
            }
        }
    }
}

void BabyGridWrapper::setBackgroundColor(const QColor& color) {
    QPalette pal = _table->palette();
    pal.setColor(QPalette::Base, color);
    _table->setPalette(pal);
}

void BabyGridWrapper::setHighlightColor(const QColor& color) {
    _highlightColor = color;
}

void BabyGridWrapper::setHighlightTextColor(const QColor& color) {
    _highlightTextColor = color;
}

void BabyGridWrapper::setGridlinesColor(const QColor& color) {
    QPalette pal = _table->palette();
    pal.setColor(QPalette::Midlight, color);
    _table->setPalette(pal);
}

void BabyGridWrapper::setUnprotectColor(const QColor& color) {
    // Used for "unprotected" cells (in shortcut mapper: editable)
    Q_UNUSED(color);
}

void BabyGridWrapper::setProtectColor(const QColor& color) {
    // Used for "protected" cells (shortcut mapper: conflict)
    Q_UNUSED(color);
}

void BabyGridWrapper::setTitleTextColor(const QColor& color) {
    QPalette hpal = _table->horizontalHeader()->palette();
    hpal.setColor(QPalette::Text, color);
    _table->horizontalHeader()->setPalette(hpal);
}

void BabyGridWrapper::setTitleColor(const QColor& color) {
    QPalette hpal = _table->horizontalHeader()->palette();
    hpal.setColor(QPalette::Button, color);
    _table->horizontalHeader()->setPalette(hpal);
}

void BabyGridWrapper::setHighlightColorNoFocus(const QColor& color) {
    Q_UNUSED(color);
}

void BabyGridWrapper::setHighlightColorProtect(const QColor& color) {
    Q_UNUSED(color);
}

void BabyGridWrapper::setHighlightColorProtectNoFocus(const QColor& color) {
    Q_UNUSED(color);
}

int BabyGridWrapper::selectedRow() const {
    return _table->currentRow();
}

int BabyGridWrapper::selectedCol() const {
    return _table->currentColumn();
}

void BabyGridWrapper::hideCursor() {
    // Hide the selection
    _table->selectionModel()->clear();
}

void BabyGridWrapper::setHeaderFont(const QFont& font) {
    _table->horizontalHeader()->setFont(font);
}

void BabyGridWrapper::setRowFont(const QFont& font) {
    _table->setFont(font);
}

void BabyGridWrapper::setHomeRow(size_t row) {
    if (static_cast<int>(row) < _table->rowCount()) {
        _table->setCurrentCell(static_cast<int>(row), 0);
    }
}

size_t BabyGridWrapper::homeRow() const {
    return static_cast<size_t>(_table->currentRow());
}

void BabyGridWrapper::setLastView(size_t homeRow, size_t cursorRow) const {
    _lastView = {homeRow, cursorRow};
}

void BabyGridWrapper::setInitialContent(bool on) {
    Q_UNUSED(on);
    // BabyGrid sets initial content for first-time display
}

void BabyGridWrapper::setCurrentCell(int row, int col) {
    _table->setCurrentCell(row, col);
}

void BabyGridWrapper::setHighlightRow(int row) {
    _highlightRow = row;
    if (row >= 0 && row < _table->rowCount()) {
        for (int c = 0; c < _table->columnCount(); ++c) {
            if (auto* item = _table->item(row, c)) {
                item->setBackground(_highlightColor);
                item->setForeground(_highlightTextColor);
            }
        }
    }
}

void BabyGridWrapper::updateView() {
    _table->viewport()->update();
}
