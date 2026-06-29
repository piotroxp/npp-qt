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

// Semantic Lift: Win32 ListView (commctrl.h) → Qt6 QTableWidget

#include "ListView.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QScrollBar>
#include <stdexcept>
#include <algorithm>

void ListView::init(void* /*hInst*/, QWidget* hParent)
{
    if (hParent)
        setParent(hParent);

    // Create the QTableWidget (replaces WC_LISTVIEW + CreateWindow)
    _tableWidget = new QTableWidget(this);
    _tableWidget->setObjectName("ListView");

    // Configure as report view with single selection, no sorting by header
    _tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _tableWidget->setShowGrid(true);
    _tableWidget->setSortingEnabled(false);
    _tableWidget->setAlternatingRowColors(true);

    // Enable full row selection + border select (LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT)
    _tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Double buffering is default in Qt
    // Apply extra style flags
    if (_extraStyle != Qt::NoItemFlags) {
        // Future: apply additional Qt flags if needed
    }

    // Create columns
    if (!_columnInfos.empty()) {
        _tableWidget->setColumnCount(static_cast<int>(_columnInfos.size()));
        int i = 0;
        for (const auto& colInfo : _columnInfos) {
            QTableWidgetItem* headerItem = new QTableWidgetItem(colInfo._label);
            _tableWidget->setHorizontalHeaderItem(i, headerItem);
            _tableWidget->setColumnWidth(i, static_cast<int>(colInfo._width));
            ++i;
        }
    }

    // Don't show vertical header numbers
    _tableWidget->verticalHeader()->setVisible(false);

    // Initial size (will be resized by parent layout)
    _tableWidget->resize(0, 0);
    _tableWidget->show();

    // Connect double-click signal
    connect(_tableWidget, &QTableWidget::itemDoubleClicked,
            this, [this](QTableWidgetItem* item) {
                if (item)
                    Q_EMIT itemDoubleClicked(item->row());
            });
}

void ListView::destroy()
{
    // In Qt parent-child model, deleting the widget destroys children automatically
    if (_tableWidget) {
        _tableWidget->deleteLater();
        _tableWidget = nullptr;
    }
    deleteLater();
}

void ListView::addLine(const std::vector<QString>& values2Add, QVariant lParam, int pos2insert)
{
    if (values2Add.empty())
        return;

    if (pos2insert == -1)
        pos2insert = static_cast<int>(nbItem());

    // Clamp to valid range
    if (pos2insert < 0)
        pos2insert = 0;
    if (pos2insert > static_cast<int>(nbItem()))
        pos2insert = static_cast<int>(nbItem());

    int row = pos2insert;
    _tableWidget->insertRow(row);

    // First column: store lParam in UserRole
    auto* firstItem = new QTableWidgetItem(values2Add[0]);
    firstItem->setData(Qt::UserRole, lParam);
    _tableWidget->setItem(row, 0, firstItem);

    // Subsequent columns
    for (size_t j = 1; j < values2Add.size(); ++j) {
        auto* item = new QTableWidgetItem(values2Add[j]);
        _tableWidget->setItem(row, static_cast<int>(j), item);
    }
}

size_t ListView::findAlphabeticalOrderPos(const QString& string2Cmp, SortDirection sortDir)
{
    const size_t itemCount = nbItem();
    if (!itemCount)
        return 0;

    for (size_t i = 0; i < itemCount; ++i) {
        QTableWidgetItem* item = _tableWidget->item(static_cast<int>(i), 0);
        if (!item)
            continue;

        const QString cellText = item->text();
        const int res = QString::compare(string2Cmp, cellText, Qt::CaseInsensitive);

        if (res < 0) { // string2Cmp < cellText
            if (sortDir == sortEncrease)
                return i;
        } else { // string2Cmp >= cellText
            if (sortDir == sortDecrease)
                return i;
        }
    }
    return itemCount;
}

QVariant ListView::getLParamFromIndex(int itemIndex) const
{
    if (itemIndex < 0 || itemIndex >= static_cast<int>(nbItem()))
        return QVariant();

    QTableWidgetItem* item = _tableWidget->item(itemIndex, 0);
    if (item)
        return item->data(Qt::UserRole);

    return QVariant();
}

std::vector<size_t> ListView::getCheckedIndexes() const
{
    std::vector<size_t> checkedIndexes;
    const size_t itemCount = nbItem();

    for (size_t i = 0; i < itemCount; ++i) {
        QTableWidgetItem* item = _tableWidget->item(static_cast<int>(i), 0);
        if (item && item->checkState() == Qt::Checked)
            checkedIndexes.push_back(i);
    }
    return checkedIndexes;
}