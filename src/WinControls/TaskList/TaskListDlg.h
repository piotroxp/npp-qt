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
#include <QListWidget>

#include "../StaticDialog/StaticDialog.h"
#include "TaskList.h"

// Resource IDs
#define IDD_TASKLIST_DLG 2450
#define ID_PICKUP (IDD_TASKLIST_DLG + 1)

#define WM_GETTASKLISTINFO (0xBABA)

// Task list function status
struct TaskLstFnStatus {
    int _iView = -1;
    int _docIndex = 0;
    QString _fn;
    int _status = 0;
    void* _bufID = nullptr;
    int _docColor = -1;
};

// Task list info
struct TaskListInfo {
    QVector<TaskLstFnStatus> _tlfsLst;
    int _currentIndex = -1;
};

// TaskListDlg - Task list dialog
class TaskListDlg : public StaticDialog
{
    Q_OBJECT

public:
    TaskListDlg();
    ~TaskListDlg() override = default;

    void init(QWidget* parent, bool dir);
    int doDialog(bool isRTL = false);
    void destroy() override;

signals:
    void taskPicked(int index);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    TaskList _taskList;
    TaskListInfo _taskListInfo;
    bool _initDir = false;
    static int _instanceCount;
    
    QListWidget* _pTaskList = nullptr;
};

int TaskListDlg::_instanceCount = 0;