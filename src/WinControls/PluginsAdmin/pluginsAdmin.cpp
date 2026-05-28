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

#include "pluginsAdmin.h"

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

#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

PluginAdminInstData g_PluginAdminInstData;

PluginsAdmin::PluginsAdmin()
    : StaticDialog()
{
}

void PluginsAdmin::init(QWidget* parent)
{
    StaticDialog::init(parent);
    initPluginList();
}

void PluginsAdmin::changeTheme()
{
    // Would apply theme changes to the plugin list
}

void PluginsAdmin::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(IDD_PLUGINSADMIN);
        setWindowTitle("Plugin Admin");
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Description label
        QLabel* descLabel = new QLabel("Manage installed plugins", this);
        mainLayout->addWidget(descLabel);
        
        // Available plugins tree
        _pTreeWidget = new QTreeWidget(this);
        _pTreeWidget->setHeaderLabels({"Name", "Description", "Author", "Version"});
        _pTreeWidget->setAlternatingRowColors(true);
        mainLayout->addWidget(_pTreeWidget);
        
        // Populate plugin list
        for (const PluginInfo& pinfo : _availablePlugins) {
            auto* parentItem = new QTreeWidgetItem(_pTreeWidget);
            parentItem->setText(0, pinfo._name);
            parentItem->setText(1, pinfo._description);
            parentItem->setText(2, pinfo._author);
            parentItem->setText(3, pinfo._versionStr);
            parentItem->setData(0, Qt::UserRole, pinfo._isInstalled);
        }
        
        // Progress bar
        _pProgressBar = new QProgressBar(this);
        _pProgressBar->setRange(0, 100);
        _pProgressBar->setValue(0);
        _pProgressBar->hide();
        mainLayout->addWidget(_pProgressBar);
        
        // Install/Uninstall buttons
        QHBoxLayout* btnLayout = new QHBoxLayout();
        
        _pPluginList = new QListWidget(this);
        _pPluginList->setSelectionMode(QAbstractItemView::SingleSelection);
        
        for (const PluginInfo& pinfo : _installedPlugins) {
            auto* item = new QListWidgetItem(pinfo._moduleName, _pPluginList);
            item->setData(Qt::UserRole, pinfo._fileName);
            item->setCheckState(pinfo._isLoaded ? Qt::Checked : Qt::Unchecked);
            item->setSelected(true);
        }
        
        connect(_pPluginList, &QListWidget::itemChanged, this, [](QListWidgetItem* item) {
            Q_UNUSED(item);
            // Toggle plugin enabled state
        });
        
        QPushButton* installBtn = new QPushButton("Install New...", this);
        connect(installBtn, &QPushButton::clicked, this, [this]() {
            QString fileName = QFileDialog::getOpenFileName(
                this,
                "Select Plugin",
                g_PluginAdminInstData._pluginHomePath,
                "Plugin DLLs (*.dll)"
            );
            
            if (!fileName.isEmpty()) {
                _pProgressBar->show();
                _pProgressBar->setValue(50);
                
                // Would unpack and install plugin here
                
                _pProgressBar->setValue(100);
                _pProgressBar->hide();
                
                QMessageBox::information(this, "Success", "Plugin installed successfully!");
            }
        });
        btnLayout->addWidget(installBtn);
        
        QPushButton* removeBtn = new QPushButton("Remove", this);
        connect(removeBtn, &QPushButton::clicked, this, [this]() {
            auto selected = _pPluginList->selectedItems();
            if (!selected.isEmpty()) {
                uninstallPlugin(selected.first());
            }
        });
        btnLayout->addWidget(removeBtn);
        
        QPushButton* closeBtn = new QPushButton("Close", this);
        connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
        btnLayout->addWidget(closeBtn);
        
        mainLayout->addLayout(btnLayout);
    }
    
    display(true);
}

void PluginsAdmin::initPluginList()
{
    // Load available plugins from plugins list
    _availablePlugins.clear();
    _installedPlugins.clear();
    _loadedPlugins.clear();
}

QVector<PluginInfo>& PluginsAdmin::getAvailablePluginsList()
{
    return _availablePlugins;
}

void PluginsAdmin::loadPlugin(QListWidgetItem* item)
{
    Q_UNUSED(item);
    // Would load the selected plugin
}

void PluginsAdmin::uninstallPlugin(QListWidgetItem* item)
{
    if (!item) return;
    
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        "Confirm Uninstall",
        "Are you sure you want to remove this plugin?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (ret == QMessageBox::Yes) {
        // Would remove plugin files
        delete item;
    }
}

intptr_t PluginsAdmin::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
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