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

#pragma once

#include <QTableWidget>
#include <QHeaderView>
#include <QString>
#include <vector>

struct ColumnInfo {
    int _width;
    QString _label;

    ColumnInfo(const QString& label, int width) : _width(width), _label(label) {}
};

// ListView - QTableWidget-based list view control
// Replaces MFC CListCtrl with Qt QTableWidget
class ListView : public QTableWidget
{
    Q_OBJECT

public:
    enum SortDirection {
        SortIncrease = 0,
        SortDecrease = 1
    };

    explicit ListView(QWidget* parent = nullptr)
        : QTableWidget(parent)
        , _extraStyle(0)
    {
        // Set up as a report-style list view
        setColumnCount(0);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setAlternatingRowColors(false);
        verticalHeader()->setVisible(false);
        horizontalHeader()->setStretchLastSection(true);
        
        // Connect signals
        connect(this, &QTableWidget::itemClicked, this, &ListView::onItemClicked);
    }

    ~ListView() override = default;

    // addColumn() should be called before init()
    void addColumn(const ColumnInfo& column2Add) {
        _columnInfos.push_back(column2Add);
        insertColumn(columnCount());
        setHorizontalHeaderItem(columnCount() - 1, new QTableWidgetItem(column2Add._label));
    }

    void setColumnText(size_t i, const QString& txt2Set) {
        if (i < static_cast<size_t>(columnCount())) {
            auto* item = horizontalHeaderItem(static_cast<int>(i));
            if (item) {
                item->setText(txt2Set);
            }
        }
    }

    // setStyleOption() should be called before init()
    void setStyleOption(int extraStyle) {
        _extraStyle = extraStyle;
    }

    size_t findAlphabeticalOrderPos(const QString& string2Cmp, SortDirection sortDir);

    void addLine(const QStringList& values2Add, intptr_t lParam = 0, int pos2insert = -1);

    int nbItem() const { return rowCount(); }

    int getSelectedIndex() const {
        auto items = selectedItems();
        if (!items.isEmpty()) {
            return items.first()->row();
        }
        return -1;
    }

    void setSelection(int itemIndex) {
        if (itemIndex >= 0 && itemIndex < rowCount()) {
            selectRow(itemIndex);
            scrollToItem(this->item(itemIndex, 0));
        }
    }

    intptr_t getLParamFromIndex(int itemIndex) {
        if (itemIndex >= 0 && itemIndex < rowCount()) {
            auto* item = this->item(itemIndex, 0);
            if (item) {
                return item->data(Qt::UserRole).toLongLong();
            }
        }
        return 0;
    }

    bool removeFromIndex(size_t i) {
        if (i < static_cast<size_t>(rowCount())) {
            removeRow(static_cast<int>(i));
            return true;
        }
        return false;
    }

    std::vector<size_t> getCheckedIndexes() const {
        std::vector<size_t> checked;
        for (int i = 0; i < rowCount(); ++i) {
            auto* item = this->item(i, 0);
            if (item && item->checkState() == Qt::Checked) {
                checked.push_back(static_cast<size_t>(i));
            }
        }
        return checked;
    }

    void init(QWidget* parent) {
        Q_UNUSED(parent);
        // Apply column widths
        int col = 0;
        for (const auto& colInfo : _columnInfos) {
            if (col < columnCount()) {
                setColumnWidth(col, colInfo._width);
            }
            ++col;
        }
        
        // Apply extra styles (LVS_EX_FULLROWSELECT equivalent handled via Qt selection behavior)
        if (_extraStyle & 0x00000020) {  // 0x20 = LVS_EX_FULLROWSELECT
            setSelectionBehavior(QAbstractItemView::SelectRows);
        }
    }

    void destroy() {
        clear();
        setRowCount(0);
        setColumnCount(0);
        _columnInfos.clear();
    }

    void redraw(bool force = false) {
        Q_UNUSED(force);
        viewport()->update();
    }

signals:
    void itemDoubleClicked(int row, int column);
    void itemRightClicked(int row, int column);

private slots:
    void onItemClicked(QTableWidgetItem* item) {
        if (item) {
            // Could emit custom signals here
            Q_UNUSED(item);
        }
    }

private:
    int _extraStyle;
    std::vector<ColumnInfo> _columnInfos;
};

// AsciiListView - specialized ListView for ASCII character display
class AsciiListView : public ListView
{
    Q_OBJECT

public:
    AsciiListView(QWidget* parent = nullptr) : ListView(parent), _codepage(-1) {}

    void setValues(int codepage = 0);
    void resetValues(int codepage);

    QString getAscii(unsigned char value);
    QString getHtmlName(unsigned char value);
    int getHtmlNumber(unsigned char value);

private:
    int _codepage;
};