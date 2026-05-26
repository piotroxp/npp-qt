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
#include <QTreeWidget>
#include <QString>
#include <QVector>
#include <QCheckBox>
#include <QProgressBar>

#include "../StaticDialog/StaticDialog.h"

// Resource IDs
#define IDD_PLUGINSADMIN 2550
#define IDC_LIST_PLUGIN (IDD_PLUGINSADMIN + 1)
#define IDC_PROGRESS_BAR (IDD_PLUGINSADMIN + 2)

struct PluginVersion {
    int _major = 0;
    int _minor = 0;
    int _build = 0;
};

struct PluginInfo {
    QString _name;
    QString _fileName;
    QString _moduleName;
    QString _description;
    QString _author;
    PluginVersion _version;
    QString _versionStr;
    bool _isInstalled = false;
    bool _isLoaded = false;
};

// PluginsAdmin - Plugin administration dialog
class PluginsAdmin : public StaticDialog
{
    Q_OBJECT

public:
    PluginsAdmin();
    ~PluginsAdmin() override = default;

    void init(QWidget* parent);
    void changeTheme();
    void doDialog(bool isRTL = false);
    void destroy() override;

signals:
    void pluginActivated(const QString& pluginName, bool load);
    void pluginClosed(const QString& pluginName);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    void initPluginList();
    QVector<PluginInfo>& getAvailablePluginsList();
    void loadPlugin(QListWidgetItem* item);
    void uninstallPlugin(QListWidgetItem* item);

    QListWidget* _pPluginList = nullptr;
    QTreeWidget* _pTreeWidget = nullptr;
    QProgressBar* _pProgressBar = nullptr;
    
    QVector<PluginInfo> _availablePlugins;
    QVector<PluginInfo> _installedPlugins;
    QVector<PluginInfo> _loadedPlugins;
};

enum PluginColumns {
    PLUGIN_NAME = 0,
    PLUGIN_DESCRIPTION,
    PLUGIN_AUTHOR,
    PLUGIN_VERSION,
    PLUGIN_nbCol
};

// Plugin admin installation data
struct PluginAdminInstData {
    QString _pluginHomePath;
    QString _pluginConfigPath;
    bool _needInitialization = false;
};

// Global plugin admin instance data
extern PluginAdminInstData g_PluginAdminInstData;