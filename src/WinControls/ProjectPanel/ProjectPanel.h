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

#include "../StaticDialog/StaticDialog.h"
#include <QMenu>
#include <QString>
#include <QVector>

#include "DockingWnd/DockingDlgInterface.h"
#include "../TreeView/TreeView.h"

// Resource IDs
#define IDD_PROJECTPANEL 3100
#define IDD_FILERELOCALIZER_DIALOG 3200

#define PM_PROJECTPANELTITLE "Project Panel"
#define PM_WORKSPACEROOTNAME "Workspace"
#define PM_NEWFOLDERNAME "Folder Name"
#define PM_NEWPROJECTNAME "Project Name"

// ProjectPanel - Project/workspace panel
class ProjectPanel : public DockingDlgInterface
{
    Q_OBJECT

public:
    ProjectPanel();
    ~ProjectPanel() override = default;

    void init(QWidget* parent, int panelID);
    void setParent(QWidget* parent2set);

    void setPanelTitle(const QString& title);
    QString getPanelTitle() const;

    void newWorkSpace();
    bool saveWorkspaceRequest();
    bool openWorkSpace(const QString& projectFileName, bool force = false);
    bool saveWorkSpace();
    bool saveWorkSpaceAs(bool saveCopyAs);
    void setWorkSpaceFilePath(const QString& projectFileName);
    QString getWorkSpaceFilePath() const;
    bool isDirty() const { return _isDirty; }
    bool checkIfNeedSave();

    void setBackgroundColor(QRgb bgColour) override;
    void setForegroundColor(QRgb fgColour) override;

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);

private:
    void initMenus();
    void showContextMenu(const QPoint& pos);
    QMenu* getMenuHandler(QTreeWidgetItem* selectedItem);
    void popupMenuCmd(int cmdID);

    TreeView* _pTreeView = nullptr;
    QMenu* _hWorkSpaceMenu = nullptr;
    QMenu* _hProjectMenu = nullptr;
    QMenu* _hFolderMenu = nullptr;
    QMenu* _hFileMenu = nullptr;
    QString _panelTitle;
    QString _workSpaceFilePath;
    bool _isDirty = false;
    int _panelID = 0;
};

// FileRelocalizerDlg - Modify file path dialog
class FileRelocalizerDlg : public StaticDialog
{
    Q_OBJECT

public:
    FileRelocalizerDlg();
    ~FileRelocalizerDlg() override = default;

    int doDialog(const QString& fn, bool isRTL = false);
    void destroy() {};
    QString getFullFilePath() const { return _fullFilePath; }

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private:
    QString _fullFilePath;
    QLineEdit* _pPathEdit = nullptr;
};