// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "BabyGrid.h"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QApplication>

BabyGrid::BabyGrid(QWidget* parent)
    : QTableWidget(parent)
{
    initGrid();
}

void BabyGrid::initGrid()
{
    // Basic setup
    setColumnCount(0);
    setRowCount(0);
    
    // Selection behavior
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    
    // Headers
    horizontalHeader()->setVisible(true);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    
    // Grid lines
    setShowGrid(true);
    setGridStyle(Qt::SolidLine);
    
    // Row height
    verticalHeader()->setDefaultSectionSize(21);
    setRowCount(2); // Start with some rows
    setColumnCount(2);
    
    // Connect signals
    connect(this, &QTableWidget::itemSelectionChanged, this, [this]() {
        int row = currentRow();
        int col = currentColumn();
        if (row >= 0 && col >= 0) {
            emit selectionChanged(row, col);
        }
    });
}

void BabyGrid::setGridDimensions(int rows, int cols)
{
    if (rows < 0 || cols < 0) return;
    
    int currentRows = rowCount();
    int currentCols = columnCount();
    
    // Add/remove rows as needed
    if (rows > currentRows) {
        setRowCount(rows);
    } else if (rows < currentRows) {
        for (int i = currentRows - 1; i >= rows; --i) {
            removeRow(i);
        }
    }
    
    // Add/remove columns as needed
    if (cols > currentCols) {
        setColumnCount(cols);
        // Set default widths
        for (int i = currentCols; i < cols; ++i) {
            setColumnWidth(i, 50);
        }
    } else if (cols < currentCols) {
        for (int i = currentCols - 1; i >= cols; --i) {
            removeColumn(i);
        }
    }
}

void BabyGrid::setCellText(int row, int col, const QString& text)
{
    if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount()) return;
    
    auto* item = this->item(row, col);
    if (!item) {
        item = new QTableWidgetItem();
        setItem(row, col, item);
    }
    item->setText(text);
}

QString BabyGrid::getCellText(int row, int col) const
{
    if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount()) return QString();
    
    auto* item = this->item(row, col);
    return item ? item->text() : QString();
}

void BabyGrid::setColWidth(int col, int width)
{
    if (col >= 0 && col < columnCount()) {
        setColumnWidth(col, width);
    }
}

void BabyGrid::setRowHeight(int row, int height)
{
    if (row >= 0 && row < rowCount()) {
        setRowHeight(row, height);
    }
}

int BabyGrid::getSelectedRow() const
{
    return currentRow();
}

int BabyGrid::getSelectedCol() const
{
    return currentColumn();
}

void BabyGrid::setTextColor(QRgb color)
{
    _textColor = color;
    QTableWidget::setStyleSheet(QString(
        "QTableWidget { color: rgb(%1, %2, %3); }"
    ).arg(qRed(color)).arg(qGreen(color)).arg(qBlue(color)));
}

void BabyGrid::setBackgroundColor(QRgb color)
{
    _bgColor = color;
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, QColor::fromRgb(color));
    setPalette(pal);
}

void BabyGrid::setTitleColor(QRgb color)
{
    _titleColor = color;
    horizontalHeader()->setStyleSheet(QString(
        "QHeaderView::section { background-color: rgb(%1, %2, %3); }"
    ).arg(qRed(color)).arg(qGreen(color)).arg(qBlue(color)));
}

void BabyGrid::setTitleTextColor(QRgb color)
{
    _titleTextColor = color;
    horizontalHeader()->setStyleSheet(QString(
        "QHeaderView::section { color: rgb(%1, %2, %3); }"
    ).arg(qRed(color)).arg(qGreen(color)).arg(qBlue(color)));
}

void BabyGrid::setHighlightColor(QRgb color)
{
    _highlightColor = color;
}

void BabyGrid::setHighlightTextColor(QRgb color)
{
    _highlightTextColor = color;
}

void BabyGrid::setGridLineColor(QRgb color)
{
    _gridLineColor = color;
    QTableWidget::setGridStyle(Qt::NoPen);
    QFrame::setStyleSheet(QString(
        "QTableWidget::item { border: 1px solid rgb(%1, %2, %3); }"
    ).arg(qRed(color)).arg(qGreen(color)).arg(qBlue(color)));
}

void BabyGrid::setCursorColor(QRgb color)
{
    _cursorColor = color;
}

void BabyGrid::makeColAutoWidth(bool autoWidth)
{
    if (autoWidth) {
        horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
}

void BabyGrid::clear()
{
    for (int r = 0; r < rowCount(); ++r) {
        for (int c = 0; c < columnCount(); ++c) {
            delete takeItem(r, c);
        }
    }
}

void BabyGrid::deleteCell(int row, int col)
{
    if (row >= 0 && col >= 0 && row < rowCount() && col < columnCount()) {
        delete takeItem(row, col);
    }
}

void BabyGrid::setLastView(size_t homeRow, size_t cursorRow)
{
    _homerow = static_cast<int>(homeRow);
    if (cursorRow < static_cast<size_t>(rowCount())) {
        setCurrentCell(static_cast<int>(cursorRow), 0);
    }
}

void BabyGrid::keyPressEvent(QKeyEvent* event)
{
    QTableWidget::keyPressEvent(event);
}

void BabyGrid::mousePressEvent(QMouseEvent* event)
{
    QTableWidget::mousePressEvent(event);
    int row = currentRow();
    int col = currentColumn();
    if (row >= 0 && col >= 0) {
        emit cellClicked(row, col);
    }
}

void BabyGrid::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTableWidget::mouseDoubleClickEvent(event);
    int row = currentRow();
    int col = currentColumn();
    if (row >= 0 && col >= 0) {
        emit cellDoubleClicked(row, col);
    }
}

void BabyGrid::contextMenuEvent(QContextMenuEvent* event)
{
    QTableWidget::contextMenuEvent(event);
}