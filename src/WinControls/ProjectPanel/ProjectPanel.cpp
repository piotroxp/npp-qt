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

#include "ProjectPanel.h"

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
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif

#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ProjectPanel::ProjectPanel()
    : DockingDlgInterface(IDD_PROJECTPANEL)
{
    setWindowTitle(PM_PROJECTPANELTITLE);
}

void ProjectPanel::init(QWidget* parent, int panelID)
{
    DockingDlgInterface::init(parent);
    _panelID = panelID;
    initMenus();
}

void ProjectPanel::setParent(QWidget* parent2set)
{
    _hParent = parent2set;
}

void ProjectPanel::setPanelTitle(const QString& title)
{
    _panelTitle = title;
    setWindowTitle(title);
}

QString ProjectPanel::getPanelTitle() const
{
    return _panelTitle;
}

void ProjectPanel::newWorkSpace()
{
    if (!_pTreeView) return;
    
    _pTreeView->clear();
    _workSpaceFilePath.clear();
    _isDirty = false;
    
    // Add root "Workspace" item
    _pTreeView->addItem(PM_WORKSPACEROOTNAME, nullptr, 0);
}

bool ProjectPanel::saveWorkspaceRequest()
{
    if (_workSpaceFilePath.isEmpty()) {
        return saveWorkSpaceAs(true);
    }
    return saveWorkSpace();
}

bool ProjectPanel::openWorkSpace(const QString& projectFileName, bool force)
{
    Q_UNUSED(force);
    
    QFile file(projectFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject rootObj = doc.object();
    setWorkSpaceFilePath(projectFileName);
    
    // Parse workspace from JSON
    if (_pTreeView) {
        QTreeWidgetItem* root = nullptr;
        if (rootObj.contains("recursive")) {
            // Parse files/folders recursively from JSON
            QJsonArray filesArray = rootObj["recursive"].toArray();
            for (const QJsonValue& value : filesArray) {
                QString folderName = value.toString();
                _pTreeView->addItem(folderName, root, -1);
            }
        }
        
        _isDirty = false;
    }
    
    return true;
}

bool ProjectPanel::saveWorkSpace()
{
    if (_workSpaceFilePath.isEmpty()) {
        return false;
    }
    
    QJsonObject rootObj;
    
    // Save recursive folders as JSON array
    if (_pTreeView) {
        QJsonArray filesArray;
        QTreeWidgetItem* root = _pTreeView->getRoot();
        if (root) {
            // Would traverse tree and add folders
            Q_UNUSED(root);
        }
        rootObj["recursive"] = filesArray;
    }
    
    QJsonDocument doc(rootObj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    QFile file(_workSpaceFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(data);
    file.close();
    
    _isDirty = false;
    return true;
}

bool ProjectPanel::saveWorkSpaceAs(bool saveCopyAs)
{
    Q_UNUSED(saveCopyAs);
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Workspace",
        QDir::homePath(),
        "Workspace Files (*.json)"
    );
    
    if (fileName.isEmpty()) {
        return false;
    }
    
    setWorkSpaceFilePath(fileName);
    return saveWorkSpace();
}

void ProjectPanel::setWorkSpaceFilePath(const QString& projectFileName)
{
    _workSpaceFilePath = projectFileName;
    _isDirty = false;
}

QString ProjectPanel::getWorkSpaceFilePath() const
{
    return _workSpaceFilePath;
}

bool ProjectPanel::checkIfNeedSave()
{
    if (!_isDirty) return false;
    
    QMessageBox::StandardButton ret = QMessageBox::question(
        this,
        "Unsaved Changes",
        "Do you want to save the changes to this workspace?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );
    
    if (ret == QMessageBox::Save) {
        return saveWorkspaceRequest();
    } else if (ret == QMessageBox::Discard) {
        return true;
    }
    
    return false;
}

void ProjectPanel::setBackgroundColor(QRgb bgColour)
{
    if (_pTreeView) {
        QPalette pal = _pTreeView->palette();
        pal.setColor(QPalette::Base, QColor::fromRgb(bgColour));
        _pTreeView->setPalette(pal);
    }
}

void ProjectPanel::setForegroundColor(QRgb fgColour)
{
    if (_pTreeView) {
        QPalette pal = _pTreeView->palette();
        pal.setColor(QPalette::Text, QColor::fromRgb(fgColour));
        _pTreeView->setPalette(pal);
    }
}

void ProjectPanel::initMenus()
{
    _hWorkSpaceMenu = new QMenu(this);
    _hProjectMenu = new QMenu(this);
    _hFolderMenu = new QMenu(this);
    _hFileMenu = new QMenu(this);
}

void ProjectPanel::showContextMenu(const QPoint& pos)
{
    QMenu* menu = getMenuHandler(_pTreeView->currentItem());
    if (menu) {
        menu->exec(pos);
    }
}

QMenu* ProjectPanel::getMenuHandler(QTreeWidgetItem* selectedItem)
{
    Q_UNUSED(selectedItem);
    // Would return appropriate menu based on item type
    return _hWorkSpaceMenu;
}

void ProjectPanel::popupMenuCmd(int cmdID)
{
    Q_UNUSED(cmdID);
    // Handle menu commands
}

void ProjectPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    // Open or expand the clicked item
    Q_UNUSED(item);
}

void ProjectPanel::onContextMenuRequested(const QPoint& pos)
{
    showContextMenu(pos);
}

intptr_t ProjectPanel::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* layout = new QVBoxLayout(this);
            
            _pTreeView = new TreeView(this);
            _pTreeView->setHeaderHidden(true);
            layout->addWidget(_pTreeView);
            
            connect(_pTreeView, &TreeView::itemDoubleClicked,
                    this, &ProjectPanel::onItemDoubleClicked);
            connect(_pTreeView, &TreeView::customContextMenuRequested,
                    this, &ProjectPanel::onContextMenuRequested);
            
            newWorkSpace();
            return TRUE;
        }
        
        case WM_SIZE: {
            if (_pTreeView) {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                _pTreeView->resize(width, height);
            }
            return TRUE;
        }
        
        case WM_NOTIFY: {
            return TRUE;
        }
        
        default:
            break;
    }
    
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

// FileRelocalizerDlg implementation

FileRelocalizerDlg::FileRelocalizerDlg()
    : StaticDialog()
{
}

int FileRelocalizerDlg::doDialog(const QString& fn, bool isRTL)
{
    Q_UNUSED(isRTL);
    _fullFilePath = fn;
    
    if (!isCreated()) {
        create(IDD_FILERELOCALIZER_DIALOG);
        setWindowTitle("File Path");
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        QLabel* label = new QLabel("New file location:", this);
        mainLayout->addWidget(label);
        
        _pPathEdit = new QLineEdit(fn, this);
        mainLayout->addWidget(_pPathEdit);
        
        QPushButton* browseBtn = new QPushButton("Browse...", this);
        connect(browseBtn, &QPushButton::clicked, this, [this]() {
            QString dir = QFileDialog::getExistingDirectory(
                this,
                "Select Directory",
                QFileInfo(_pPathEdit->text()).absolutePath()
            );
            if (!dir.isEmpty()) {
                _pPathEdit->setText(dir);
            }
        });
        mainLayout->addWidget(browseBtn);
        
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        
        QPushButton* okBtn = new QPushButton("OK", this);
        connect(okBtn, &QPushButton::clicked, this, [this]() {
            _fullFilePath = _pPathEdit->text();
            display(true);
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
    return _fullFilePath.isEmpty() ? IDCANCEL : IDOK;
}

intptr_t FileRelocalizerDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}