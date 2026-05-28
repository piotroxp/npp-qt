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
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QString>
#include <QVariant>
#include <QMap>

// BabyGrid message IDs
#define BABYGRID_USER 0xBABYGRID_BASE

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

#define BGM_SETGRIDDIM          (BABYGRID_USER + 6)
#define BGM_CLEARGRID           (BABYGRID_USER + 5)
#define BGM_SETCELLDATA         (BABYGRID_USER + 3)
#define BGM_GETCELLDATA         (BABYGRID_USER + 4)
#define BGM_GETROW              (BABYGRID_USER + 27)
#define BGM_GETCOL              (BABYGRID_USER + 28)
#define BGM_GETROWS             (BABYGRID_USER + 23)
#define BGM_GETCOLS             (BABYGRID_USER + 24)
#define BGM_GETCOLWIDTH         (BABYGRID_USER + 30)
#define BGM_SETCOLWIDTH         (BABYGRID_USER + 11)
#define BGM_SETROWHEIGHT        (BABYGRID_USER + 16)
#define BGM_SETTEXTCOLOR        (BABYGRID_USER + 51)
#define BGM_SETBACKGROUNDCOLOR  (BABYGRID_USER + 52)
#define BGM_SETTITLECOLOR       (BABYGRID_USER + 54)
#define BGM_SETTITLETEXTCOLOR   (BABYGRID_USER + 53)
#define BGM_SETHILIGHTCOLOR     (BABYGRID_USER + 34)
#define BGM_SETHILIGHTTEXTCOLOR (BABYGRID_USER + 35)
#define BGM_SETGRIDLINECOLOR    (BABYGRID_USER + 37)
#define BGM_SETCURSORCOLOR      (BABYGRID_USER + 17)
#define BGM_SETCOLAUTOWIDTH     (BABYGRID_USER + 41)
#define BGM_DELETECELL          (BABYGRID_USER + 7)
#define BGM_GETHOMEROW          (BABYGRID_USER + 45)
#define BGM_SETLASTVIEW         (BABYGRID_USER + 46)
#define BGM_AUTOROW             (BABYGRID_USER + 9)
#define BGM_SETINITIALCONTENT   (BABYGRID_USER + 47)

struct BGCELL {
    int row = 0;
    int col = 0;
};

// BabyGrid - Custom grid control
// Replaces MFC CGridCtrl with Qt QTableWidget
class BabyGrid : public QTableWidget
{
    Q_OBJECT

public:
    BabyGrid(QWidget* parent = nullptr);
    ~BabyGrid() override = default;

    // Grid dimension control
    void setGridDimensions(int rows, int cols);
    int getRows() const { return rowCount(); }
    int getCols() const { return columnCount(); }

    // Cell data
    void setCellText(int row, int col, const QString& text);
    QString getCellText(int row, int col) const;

    // Row/column sizes
    void setColWidth(int col, int width);
    void setRowHeight(int row, int height);
    int getColWidth(int col) const { return columnWidth(col); }
    int getRowHeight(int row) const { return rowHeight(row); }

    // Selection
    int getSelectedRow() const;
    int getSelectedCol() const;
    int getHomeRow() const { return _homerow; }

    // Colors
    void setTextColor(QRgb color);
    void setBackgroundColor(QRgb color);
    void setTitleColor(QRgb color);
    void setTitleTextColor(QRgb color);
    void setHighlightColor(QRgb color);
    void setHighlightTextColor(QRgb color);
    void setGridLineColor(QRgb color);
    void setCursorColor(QRgb color);

    // Style options
    void makeColAutoWidth(bool autoWidth = true);
    void clear();
    void deleteCell(int row, int col);

    // State
    void setLastView(size_t homeRow, size_t cursorRow);

signals:
    void selectionChanged(int row, int col);
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);
    void cellRightClicked(int row, int col);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void initGrid();
    
    int _homerow = 1;
    int _homecol = 1;
    QRgb _cursorColor = qRgb(255, 255, 255);
    QRgb _textColor = qRgb(0, 0, 0);
    QRgb _bgColor = qRgb(240, 240, 240);
    QRgb _titleColor = qRgb(240, 240, 240);
    QRgb _titleTextColor = qRgb(0, 0, 0);
    QRgb _highlightColor = qRgb(0, 0, 128);
    QRgb _highlightTextColor = qRgb(255, 255, 255);
    QRgb _gridLineColor = qRgb(220, 220, 220);
};