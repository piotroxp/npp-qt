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

#include <QDialog>
#include <QTreeWidget>
#include <QKeySequenceEdit>
#include <QString>
#include <QVector>
#include <QFont>

#include "../StaticDialog/StaticDialog.h"
#include "BabyGridWrapper.h"
#include "../ContextMenu/ContextMenu.h"

// Resource IDs
#define IDD_SHORTCUTMAPPER_DLG 2600
#define IDC_BABYGRID_TABBAR (IDD_SHORTCUTMAPPER_DLG + 4)
#define IDC_BABYGRID_INFO (IDD_SHORTCUTMAPPER_DLG + 5)
#define IDC_BABYGRID_FILTER (IDD_SHORTCUTMAPPER_DLG + 8)
#define IDC_BABYGRID_FILTER_CLEAR (IDD_SHORTCUTMAPPER_DLG + 9)

enum GridState { STATE_MENU, STATE_MACRO, STATE_USER, STATE_PLUGIN, STATE_SCINTILLA };

// ShortcutMapper - Keyboard shortcut mapping dialog
class ShortcutMapper : public StaticDialog
{
    Q_OBJECT

public:
    ShortcutMapper();
    ~ShortcutMapper() override;

    void init(QWidget* parent, GridState initState = STATE_MENU);
    void destroy();
    void doDialog(bool isRTL = false);

    bool findKeyConflicts(QString* keyConflictLocation, const QString& itemKeyCombo, size_t itemIndex);

signals:
    void shortcutChanged();

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private slots:
    void onTabChanged(int index);
    void onFilterChanged(const QString& text);
    void onClearFilter();
    void onGridItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);

private:
    void initTabs();
    void initBabyGrid();
    void fillOutBabyGrid();
    void resizeDialogElements();
    void getClientRect(QRect& rc) const;

    BabyGridWrapper _babygrid;
    ContextMenu _rightClickMenu;
    GridState _currentState;
    
    QTabWidget* _pTabCtrl = nullptr;
    QLineEdit* _pFilterEdit = nullptr;
    QTreeWidget* _pGrid = nullptr;
    
    QVector<QString> _tabNames;
    QVector<QString> _shortcutFilter;
    
    int _nbTab = 5;
    bool _dialogInitDone = false;
    
    QFont _headerFont;
    QFont _rowFont;
};