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

#include "TaskListDlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

TaskListDlg::TaskListDlg()
    : StaticDialog()
    , _initDir(false)
{
    _instanceCount++;
}

void TaskListDlg::init(QWidget* parent, bool dir)
{
    StaticDialog::init(parent);
    _initDir = dir;
}

int TaskListDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(IDD_TASKLIST_DLG);
        setWindowTitle("Task List");
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        QLabel* label = new QLabel("Select a document:", this);
        mainLayout->addWidget(label);
        
        _pTaskList = new QListWidget(this);
        _pTaskList->setSelectionMode(QAbstractItemView::SingleSelection);
        
        // Populate with items from _taskListInfo
        for (const auto& item : _taskListInfo._tlfsLst) {
            _pTaskList->addItem(item._fn);
        }
        
        if (_taskListInfo._currentIndex >= 0 && _taskListInfo._currentIndex < _pTaskList->count()) {
            _pTaskList->setCurrentRow(_taskListInfo._currentIndex);
        }
        
        connect(_pTaskList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
            Q_UNUSED(item);
            int index = _pTaskList->currentRow();
            emit taskPicked(index);
            display(false);
        });
        
        mainLayout->addWidget(_pTaskList);
        
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        
        QPushButton* okBtn = new QPushButton("OK", this);
        connect(okBtn, &QPushButton::clicked, this, [this]() {
            int index = _pTaskList->currentRow();
            emit taskPicked(index);
            display(false);
        });
        btnLayout->addWidget(okBtn);
        
        QPushButton* cancelBtn = new QPushButton("Cancel", this);
        connect(cancelBtn, &QPushButton::clicked, this, [this]() {
            display(false);
        });
        btnLayout->addWidget(cancelBtn);
        
        mainLayout->addLayout(btnLayout);
    }
    
    display(true);
    return IDOK;
}

intptr_t TaskListDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
            
        case WM_GETTASKLISTINFO:
            // Return task list info
            return TRUE;
            
        case WM_COMMAND:
            if (wParam == IDOK) {
                display(false);
                return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}