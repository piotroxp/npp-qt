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

#include "ShortcutMapper.h"

// Win32 macro stubs for Linux Qt6 port
#ifndef WM_INITDIALOG
#define WM_INITDIALOG 0x0110
#endif
#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif
#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif
#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef IDOK
#define IDOK 1
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef IDYES
#define IDYES 6
#endif
#ifndef IDNO
#define IDNO 7
#endif
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

ShortcutMapper::ShortcutMapper()
    : StaticDialog()
    , _currentState(STATE_MENU)
{
    _tabNames = {"Main Menu", "Macros", "User Defined", "Plugin Commands", "Scintilla"};
}

ShortcutMapper::~ShortcutMapper()
{
}

void ShortcutMapper::init(QWidget* parent, GridState initState)
{
    StaticDialog::init(parent);
    _currentState = initState;
}

void ShortcutMapper::destroy()
{
    _babygrid.destroy();
}

void ShortcutMapper::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(IDD_SHORTCUTMAPPER_DLG);
        setWindowTitle("Shortcut Mapper");
        
        // Main layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Filter input
        QHBoxLayout* filterLayout = new QHBoxLayout();
        QLabel* filterLabel = new QLabel("Filter:", this);
        filterLayout->addWidget(filterLabel);
        
        _pFilterEdit = new QLineEdit(this);
        connect(_pFilterEdit, &QLineEdit::textChanged, this, &ShortcutMapper::onFilterChanged);
        filterLayout->addWidget(_pFilterEdit);
        
        QPushButton* clearBtn = new QPushButton("Clear", this);
        connect(clearBtn, &QPushButton::clicked, this, &ShortcutMapper::onClearFilter);
        filterLayout->addWidget(clearBtn);
        
        mainLayout->addLayout(filterLayout);
        
        // Tab control
        _pTabCtrl = new QTabWidget(this);
        for (int i = 0; i < _nbTab; ++i) {
            _pTabCtrl->addTab(new QWidget(), _tabNames[i]);
        }
        connect(_pTabCtrl, &QTabWidget::currentChanged, this, &ShortcutMapper::onTabChanged);
        mainLayout->addWidget(_pTabCtrl);
        
        // Grid widget
        _pGrid = new QTreeWidget(this);
        _pGrid->setHeaderLabels({"Name", "Shortcut", "Category"});
        _pGrid->setAlternatingRowColors(true);
        _pGrid->setSelectionMode(QAbstractItemView::SingleSelection);
        
        connect(_pGrid, &QTreeWidget::itemDoubleClicked, 
                this, &ShortcutMapper::onGridItemDoubleClicked);
        connect(_pGrid, &QTreeWidget::customContextMenuRequested,
                this, &ShortcutMapper::onContextMenuRequested);
        _pGrid->setContextMenuPolicy(Qt::CustomContextMenu);
        
        mainLayout->addWidget(_pGrid);
        
        // Info label
        QLabel* infoLabel = new QLabel(this);
        infoLabel->setText("Double-click on a shortcut to modify it.");
        mainLayout->addWidget(infoLabel);
        
        // Close button
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        
        QPushButton* closeBtn = new QPushButton("Close", this);
        connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
        btnLayout->addWidget(closeBtn);
        
        mainLayout->addLayout(btnLayout);
        
        // Initialize
        initBabyGrid();
        fillOutBabyGrid();
    }
    
    display(true);
}

void ShortcutMapper::initTabs()
{
    // Tabs are already set up in doDialog
}

void ShortcutMapper::initBabyGrid()
{
    // Baby grid would be used for the detailed view
    // For now we use QTreeWidget
    _pGrid->setColumnWidth(0, 250);
    _pGrid->setColumnWidth(1, 150);
}

void ShortcutMapper::fillOutBabyGrid()
{
    _pGrid->clear();
    
    // This would be populated with actual shortcuts from NppParameters
    // For now, add some sample data
    for (int i = 0; i < 10; ++i) {
        auto* item = new QTreeWidgetItem(_pGrid);
        item->setText(0, QString("Command %1").arg(i));
        item->setText(1, "Ctrl+Shift+X");
        item->setText(2, "Main Menu");
        item->setData(0, Qt::UserRole, i);
    }
}

void ShortcutMapper::resizeDialogElements()
{
    // Adjust column widths based on dialog size
    if (_pGrid) {
        int width = _pGrid->width();
        _pGrid->setColumnWidth(0, width * 0.5);
        _pGrid->setColumnWidth(1, width * 0.3);
    }
}

void ShortcutMapper::getClientRect(QRect& rc) const
{
    if (isVisible()) {
        rc = rect();
    } else {
        rc = QRect(0, 0, 600, 400);
    }
}

bool ShortcutMapper::findKeyConflicts(QString* keyConflictLocation, const QString& itemKeyCombo, size_t itemIndex)
{
    Q_UNUSED(keyConflictLocation);
    Q_UNUSED(itemKeyCombo);
    Q_UNUSED(itemIndex);
    
    // Would check for conflicts with existing shortcuts
    return false;
}

void ShortcutMapper::onTabChanged(int index)
{
    _currentState = static_cast<GridState>(index);
    fillOutBabyGrid();
}

void ShortcutMapper::onFilterChanged(const QString& text)
{
    // Filter the tree items
    QList<QTreeWidgetItem*> items = _pGrid->findItems(text, Qt::MatchContains | Qt::MatchRecursive, 0);
    
    // Hide non-matching items
    for (int i = 0; i < _pGrid->topLevelItemCount(); ++i) {
        auto* item = _pGrid->topLevelItem(i);
        bool matches = text.isEmpty();
        for (int j = 0; j < item->columnCount(); ++j) {
            if (item->text(j).contains(text, Qt::CaseInsensitive)) {
                matches = true;
                break;
            }
        }
        item->setHidden(!matches);
    }
}

void ShortcutMapper::onClearFilter()
{
    if (_pFilterEdit) {
        _pFilterEdit->clear();
    }
    onFilterChanged(QString());
}

void ShortcutMapper::onGridItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item) return;
    
    // Show input dialog for new shortcut
    bool ok;
    QString newShortcut = QInputDialog::getText(this, "Edit Shortcut", 
        "Enter new shortcut:", QLineEdit::Normal, item->text(1), &ok);
    
    if (ok && !newShortcut.isEmpty()) {
        item->setText(1, newShortcut);
        emit shortcutChanged();
    }
}

void ShortcutMapper::onContextMenuRequested(const QPoint& pos)
{
    QTreeWidgetItem* item = _pGrid->itemAt(pos);
    if (!item) return;
    
    QMenu menu(this);
    menu.addAction("Modify", this, [this, item]() {
        onGridItemDoubleClicked(item, 1);
    });
    menu.addAction("Clear", this, [item]() {
        item->setText(1, "");
    });
    menu.exec(QCursor::pos());
}

intptr_t ShortcutMapper::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            // Apply dark mode if enabled
            return TRUE;
            
        case WM_SIZE:
            resizeDialogElements();
            return TRUE;
            
        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                case IDCANCEL:
                    display(false);
                    return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}