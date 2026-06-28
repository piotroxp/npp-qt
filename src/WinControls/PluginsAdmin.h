// Semantic Lift: Win32 PluginsAdmin → Qt6 PluginsAdmin
// Original: PowerEditor/src/WinControls/PluginsAdmin/pluginsAdmin.h
// Target: npp-qt/src/WinControls/PluginsAdmin.h

#pragma once

#include "StaticDialog.h"
#include <QTabWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QCheckBox>
#include <QVector>
#include <QMap>
#include <QString>
#include <vector>
#include <string>

// Version is defined in Common.h — reuses NppParameters::getVersion() + plugin compatibility
#include "MISC/Common/Common.h"

// =============================================================================
// PluginUpdateInfo — plugin metadata (mirrors Win32 PluginUpdateInfo)
// =============================================================================

struct PluginUpdateInfo {
    QString _fullFilePath;          // Only for installed plugins
    QString _folderName;           // Plugin folder name (unique)
    QString _displayName;           // Display name
    Version _version;
    std::pair<Version, Version> _nppCompatibleVersions; // <from, to>
    std::pair<std::pair<Version, Version>, std::pair<Version, Version>> _oldVersionCompatibility;
    QString _homepage;
    QString _sourceUrl;
    QString _description;
    QString _author;
    QString _id;                   // SHA-256 plugin ID
    QString _repository;
    bool _isVisible = true;

    PluginUpdateInfo() = default;
    PluginUpdateInfo(const QString& fullFilePath, const QString& fileName);

    QString describe() const;
};

// =============================================================================
// NppCurrentStatus — installation location and mode (mirrors Win32)
// =============================================================================

struct NppCurrentStatus {
    bool _isAdminMode = false;
    bool _isInProgramFiles = true;
    bool _isAppDataPluginsAllowed = false;
    QString _nppInstallPath;
    QString _appdataPath;

    bool shouldLaunchInAdmMode() const { return _isInProgramFiles; }
};

// List type enumeration (mirrors Win32 LIST_TYPE)
enum PluginListType { Available = 0, Updates = 1, Installed = 2, Incompatible = 3 };

// =============================================================================
// PluginListView — list of plugins with checkbox support
// Mirrors Win32 ListView with LVS_EX_CHECKBOXES.
// =============================================================================

class PluginListView : public QListWidget
{
    Q_OBJECT

public:
    explicit PluginListView(QWidget* parent = nullptr);
    ~PluginListView() override = default;

    void addPlugin(const PluginUpdateInfo& info);
    void addPluginItem(const QString& name, const QString& version, PluginUpdateInfo* info);
    QVector<size_t> getCheckedIndexes() const;
    size_t getSelectedIndex() const;
    void setSelection(int index);
    void clearAll();
    void removeItem(size_t index);
    void removeItemByPtr(PluginUpdateInfo* info);
    void hideFromIndex(size_t index);
    bool restore(const QString& folderName);

    PluginUpdateInfo* pluginInfoAt(size_t index) const;
    PluginUpdateInfo* pluginInfoFromName(const QString& folderName, size_t& index) const;
    size_t count() const { return _pluginList.size(); }

    // Sorting
    enum SortType { NameAZ, NameZA };
    void sortBy(SortType type);
    size_t findAlphabeticalOrderPos(const QString& name, bool increase) const;

    // Internal: append raw pointer (avoids duplicating addPlugin logic)
    void addPluginPtr(PluginUpdateInfo* info) { _pluginList.append(info); }

private:
    QVector<PluginUpdateInfo*> _pluginList;
    SortType _sortType = NameAZ;
};

// =============================================================================
// PluginsAdminDlg — Plugin Manager dialog
// Mirrors Win32 PluginsAdminDlg.
// =============================================================================

class PluginsAdminDlg : public StaticDialog
{
    Q_OBJECT

public:
    PluginsAdminDlg();

    void doDialog(bool isRTL = false);
    bool initFromJson();
    void setPluginsManager(void* pluginsManager) { _pPluginsManager = pluginsManager; }

    void switchDialog(int indexToSwitch);
    void setAdminMode(bool isAdm) {
        _nppCurrentStatus._isAdminMode = isAdm;
    }

    bool updateList();
    bool installPlugins();
    bool updatePlugins();
    bool removePlugins();

    // Tab/column rename
    void changeTabName(PluginListType index, const QString& name);
    void changeColumnName(int index, const QString& name);

    // List access
    QVector<PluginUpdateInfo> getAvailablePlugins() const;
    QVector<PluginUpdateInfo> getInstalledPlugins() const;
    std::wstring getPluginListVerStr() const;

    // Win32 compatibility: plugin manager passes these lists to _pluginsManager.loadPlugins()
    std::vector<PluginUpdateInfo*>* getAvailablePluginUpdateInfoList() { return &_allAvailablePlugins; }
    std::vector<PluginUpdateInfo*>* getIncompatibleList() {
        // Build incompatible list from _incompatibleList if not cached
        if (_incompatiblePlugins.empty() && _incompatibleList) {
            for (size_t i = 0; i < _incompatibleList->count(); ++i) {
                if (auto* info = _incompatibleList->pluginInfoAt(i))
                    _incompatiblePlugins.push_back(info);
            }
        }
        return &_incompatiblePlugins;
    }

signals:
    void pluginsInstalled(const QVector<PluginUpdateInfo>& plugins);
    void pluginsRemoved(const QVector<PluginUpdateInfo>& plugins);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    void collectNppCurrentStatusInfos();
    void initPluginTabs();
    void loadInstalledPlugins();
    bool loadFromPluginInfos();
    bool initAvailablePluginsViewFromList();
    bool initIncompatiblePluginList();
    void addSamplePlugins();

    // Search
    bool searchInPlugins(bool isNextMode) const;
    bool isFoundInList(const PluginListView& list, int index, const QString& str, bool searchNames) const;
    long searchFromCurrentSel(const PluginListView& list, const QString& str, bool searchNames, bool isNextMode) const;

    // Tab management
    QTabWidget* _tabWidget = nullptr;

    // Lists per tab
    PluginListView* _availableList = nullptr;
    PluginListView* _updateList = nullptr;
    PluginListView* _installedList = nullptr;
    PluginListView* _incompatibleList = nullptr;

    // Description area
    QLabel* _descriptionLabel = nullptr;
    QLabel* _repoLink = nullptr;

    // Buttons
    QPushButton* _installBtn = nullptr;
    QPushButton* _updateBtn = nullptr;
    QPushButton* _removeBtn = nullptr;
    QPushButton* _refreshBtn = nullptr;
    QPushButton* _closeBtn = nullptr;

    // Search
    QLineEdit* _searchEdit = nullptr;
    QPushButton* _searchNextBtn = nullptr;
    QPushButton* _searchPrevBtn = nullptr;

    // Progress
    QProgressBar* _progressBar = nullptr;
    QLabel* _statusLabel = nullptr;

    // Plugin manager reference
    void* _pPluginsManager = nullptr;

    // Admin state
    NppCurrentStatus _nppCurrentStatus;

    // Updater paths
    QString _updaterDir;
    QString _updaterFullPath;
    QString _pluginListFullPath;
    QString _pluginListVersion;

    // Lists (raw PluginUpdateInfo*)
    std::vector<PluginUpdateInfo*> _allAvailablePlugins;
    std::vector<PluginUpdateInfo*> _incompatiblePlugins;  // cached from _incompatibleList

    void onInstallClicked();
    void onUpdateClicked();
    void onRemoveClicked();
    void onRefreshClicked();
    void onSearchChanged(const QString& text);
    void onSearchNext();
    void onSearchPrev();
    void onTabChanged(int index);
    void onItemClicked(QListWidgetItem* item);

    void setStatus(const QString& msg);
};
