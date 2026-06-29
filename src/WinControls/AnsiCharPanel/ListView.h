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

#pragma once

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QString>
#include <QVector>
#include <QVariant>
#include <vector>
#include <string>

#include "Window.h"

// Column descriptor for ListView columns
struct columnInfo {
    size_t _width;
    QString _label;

    columnInfo(const QString& label, size_t width) : _width(width), _label(label) {}
};

// ListView wraps a QTableWidget to replicate the Win32 ListView API surface.
// All indices are 0-based in Qt; Win32 ListView_InsertColumn uses 1-based index
// so this is adjusted internally.
class ListView : public WindowBase
{
    Q_OBJECT

public:
    ListView() = default;
    ~ListView() override = default;

    enum SortDirection {
        sortEncrease = 0,
        sortDecrease = 1
    };

    // addColumn() should be called before init()
    void addColumn(const columnInfo& column2Add) {
        _columnInfos.push_back(column2Add);
    }

    // Set column header text (0-based index)
    void setColumnText(size_t i, const QString& txt2Set) {
        if (i < static_cast<size_t>(_tableWidget->columnCount())) {
            _tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            _tableWidget->setHorizontalHeaderItem(static_cast<int>(i), new QTableWidgetItem(txt2Set));
        }
    }

    // setStyleOption() should be called before init()
    void setStyleOption(Qt::ItemFlags extraStyle) {
        _extraStyle = extraStyle;
    }

    // Find position to insert string maintaining sort order
    size_t findAlphabeticalOrderPos(const QString& string2Cmp, SortDirection sortDir);

    // Add a row with values for each column; lParam stored per row (role Qt::UserRole)
    // pos2insert: -1 means append
    void addLine(const std::vector<QString>& values2Add, QVariant lParam = QVariant(), int pos2insert = -1);

    // Number of rows
    size_t nbItem() const {
        return static_cast<size_t>(_tableWidget->rowCount());
    }

    // Get currently selected row index
    long getSelectedIndex() const {
        return _tableWidget->currentRow();
    }

    // Select and scroll to a row
    void setSelection(int itemIndex) const {
        if (itemIndex >= 0 && itemIndex < _tableWidget->rowCount()) {
            _tableWidget->selectRow(itemIndex);
            _tableWidget->scrollToItem(_tableWidget->item(itemIndex, 0));
        }
    }

    // Retrieve stored lParam for a row (Qt::UserRole)
    QVariant getLParamFromIndex(int itemIndex) const;

    // Delete a row by index
    bool removeFromIndex(size_t i) {
        if (i >= nbItem())
            return false;
        _tableWidget->removeRow(static_cast<int>(i));
        return true;
    }

    // Get indices of checked rows (checkstate column if present)
    std::vector<size_t> getCheckedIndexes() const;

    // Initialize: create QTableWidget as child of hParent
    void init(void* /*hInst*/, QWidget* hParent) override;
    void destroy() override;

Q_SIGNALS:
    // Emitted when user double-clicks a row
    void itemDoubleClicked(int row);

protected:
    Qt::ItemFlags _extraStyle = Qt::NoItemFlags;
    std::vector<columnInfo> _columnInfos;
    QTableWidget* _tableWidget = nullptr;

private:
    friend class AsciiListView;
};