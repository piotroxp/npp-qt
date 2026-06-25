// Semantic Lift: Win32 PluginsAdmin → Qt6 PluginsAdmin
// Original: PowerEditor/src/WinControls/PluginsAdmin/pluginsAdmin.cpp
// Target: npp-qt/src/WinControls/PluginsAdmin.cpp

#include "PluginsAdmin.h"
#include "StaticDialog.h"
#include <string>  // std::wstring
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QLineEdit>
#include <QTextEdit>

// Version methods are in Common.cpp (via Common.h)
// No local implementations needed here

// =============================================================================
// PluginUpdateInfo — methods
// =============================================================================

PluginUpdateInfo::PluginUpdateInfo(const QString& fullFilePath, const QString& fileName)
    : _fullFilePath(fullFilePath), _folderName(fileName)
{
    // Extract version from file metadata (requires filesystem)
}

QString PluginUpdateInfo::describe() const
{
    QString desc;
    if (!_description.isEmpty())
        desc += _description + QStringLiteral("\n\n");
    if (!_author.isEmpty())
        desc += QStringLiteral("Author: %1\n").arg(_author);
    if (!_homepage.isEmpty())
        desc += QStringLiteral("Homepage: %1\n").arg(_homepage);
    if (!_nppCompatibleVersions.first._major > 0 || _nppCompatibleVersions.second._major > 0)
        desc += QStringLiteral("Compatible: %1 - %2\n").arg(
            _nppCompatibleVersions.first.toString()).arg(_nppCompatibleVersions.second.toString());
    return desc;
}

// =============================================================================
// PluginListView — constructor
// Mirrors Win32 ListView initialization with LVS_EX_CHECKBOXES.
// =============================================================================

PluginListView::PluginListView(QWidget* parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);

    connect(this, &QListWidget::itemClicked,
            this, [this](QListWidgetItem* item) {
                Q_UNUSED(item);
                // Could emit signal for description update
            });
}

void PluginListView::addPlugin(const PluginUpdateInfo& info)
{
    auto* item = new QListWidgetItem(info._displayName, this);
    item->setData(Qt::UserRole + 1, QVariant::fromValue(info));
    item->setToolTip(info._description);
    item->setCheckState(Qt::Unchecked);

    PluginUpdateInfo* ptr = new PluginUpdateInfo(info);
    _pluginList.append(ptr);
    item->setData(Qt::UserRole + 2, reinterpret_cast<quint64>(ptr));
}

void PluginListView::addPluginItem(const QString& name, const QString& version, PluginUpdateInfo* info)
{
    auto* item = new QListWidgetItem(name, this);
    item->setData(Qt::UserRole + 1, version);
    item->setToolTip(info ? info->describe() : QString());
    item->setCheckState(Qt::Unchecked);
    if (info)
    {
        item->setData(Qt::UserRole + 2, reinterpret_cast<quint64>(info));
        _pluginList.append(info);
    }
}

QVector<size_t> PluginListView::getCheckedIndexes() const
{
    QVector<size_t> result;
    for (int i = 0; i < count(); ++i)
    {
        if (item(i)->checkState() == Qt::Checked)
            result.append(static_cast<size_t>(i));
    }
    return result;
}

size_t PluginListView::getSelectedIndex() const
{
    QListWidgetItem* sel = currentItem();
    if (!sel)
        return SIZE_MAX;
    return static_cast<size_t>(row(sel));
}

void PluginListView::setSelection(int index) const
{
    if (index >= 0 && index < count())
        QListWidget::setCurrentRow(index);
}

void PluginListView::clearAll()
{
    clear();
    for (PluginUpdateInfo* p : _pluginList)
        delete p;
    _pluginList.clear();
}

void PluginListView::removeItem(size_t index)
{
    if (index >= static_cast<size_t>(count()))
        return;
    if (index < static_cast<size_t>(_pluginList.size()))
    {
        delete _pluginList[static_cast<int>(index)];
        _pluginList.removeAt(static_cast<int>(index));
    }
    delete takeItem(static_cast<int>(index));
}

void PluginListView::removeItemByPtr(PluginUpdateInfo* info)
{
    for (int i = 0; i < count(); ++i)
    {
        quint64 ptrVal = item(i)->data(Qt::UserRole + 2).value<quint64>();
        if (reinterpret_cast<PluginUpdateInfo*>(ptrVal) == info)
        {
            _pluginList.removeOne(info);
            delete takeItem(i);
            return;
        }
    }
}

void PluginListView::hideFromIndex(size_t index)
{
    if (index < static_cast<size_t>(count()))
    {
        QListWidgetItem* it = item(static_cast<int>(index));
        if (it)
        {
            it->setHidden(true);
            if (index < static_cast<size_t>(_pluginList.size()))
                _pluginList[static_cast<int>(index)]->_isVisible = false;
        }
    }
}

bool PluginListView::restore(const QString& folderName)
{
    for (PluginUpdateInfo* info : _pluginList)
    {
        if (info->_folderName == folderName)
        {
            // Find hidden item and show it
            for (int i = 0; i < count(); ++i)
            {
                quint64 ptrVal = item(i)->data(Qt::UserRole + 2).value<quint64>();
                if (reinterpret_cast<PluginUpdateInfo*>(ptrVal) == info)
                {
                    item(i)->setHidden(false);
                    info->_isVisible = true;
                    return true;
                }
            }
        }
    }
    return false;
}

PluginUpdateInfo* PluginListView::pluginInfoAt(size_t index) const
{
    if (index >= static_cast<size_t>(_pluginList.size()))
        return nullptr;
    return _pluginList[static_cast<int>(index)];
}

PluginUpdateInfo* PluginListView::pluginInfoFromName(const QString& folderName, size_t& index) const
{
    index = 0;
    for (const PluginUpdateInfo* info : _pluginList)
    {
        if (info->_folderName == folderName)
            return const_cast<PluginUpdateInfo*>(info);
        ++index;
    }
    index = SIZE_MAX;
    return nullptr;
}

void PluginListView::sortBy(SortType type)
{
    _sortType = type;
    sortItems();
}

size_t PluginListView::findAlphabeticalOrderPos(const QString& name, bool increase) const
{
    for (int i = 0; i < count(); ++i)
    {
        QString otherName = item(i)->text();
        int cmp = QString::compare(name, otherName, Qt::CaseInsensitive);
        if (increase ? (cmp < 0) : (cmp > 0))
            return static_cast<size_t>(i);
    }
    return count();
}

// =============================================================================
// PluginsAdminDlg — constructor
// Mirrors Win32 PluginsAdminDlg::PluginsAdminDlg().
// =============================================================================

PluginsAdminDlg::PluginsAdminDlg()
    : StaticDialog()
{
    setWindowTitle(QStringLiteral("Plugin Admin"));
    setMinimumSize(700, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // Tab widget for plugin categories
    _tabWidget = new QTabWidget(this);
    _availableList = new PluginListView(_tabWidget);
    _updateList = new PluginListView(_tabWidget);
    _installedList = new PluginListView(_tabWidget);
    _incompatibleList = new PluginListView(_tabWidget);

    _tabWidget->addTab(_availableList, QStringLiteral("Available"));
    _tabWidget->addTab(_updateList, QStringLiteral("Updates"));
    _tabWidget->addTab(_installedList, QStringLiteral("Installed"));
    _tabWidget->addTab(_incompatibleList, QStringLiteral("Incompatible"));

    mainLayout->addWidget(_tabWidget);

    // Description area
    _descriptionLabel = new QLabel(this);
    _descriptionLabel->setWordWrap(true);
    _descriptionLabel->setTextFormat(Qt::RichText);
    _descriptionLabel->setText(QStringLiteral("<i>Select a plugin to see details</i>"));
    mainLayout->addWidget(_descriptionLabel);

    // Search bar
    QHBoxLayout* searchRow = new QHBoxLayout();
    _searchEdit = new QLineEdit(this);
    _searchEdit->setPlaceholderText(QStringLiteral("Search plugins (min 2 chars)..."));
    _searchPrevBtn = new QPushButton(QStringLiteral("▲"), this);
    _searchNextBtn = new QPushButton(QStringLiteral("▼"), this);

    connect(_searchEdit, &QLineEdit::textChanged, this, &PluginsAdminDlg::onSearchChanged);
    connect(_searchNextBtn, &QPushButton::clicked, this, &PluginsAdminDlg::onSearchNext);
    connect(_searchPrevBtn, &QPushButton::clicked, this, &PluginsAdminDlg::onSearchPrev);

    searchRow->addWidget(new QLabel(QStringLiteral("Search:"), this));
    searchRow->addWidget(_searchEdit, 1);
    searchRow->addWidget(_searchPrevBtn);
    searchRow->addWidget(_searchNextBtn);
    mainLayout->addLayout(searchRow);

    // Status row
    QHBoxLayout* statusRow = new QHBoxLayout();
    _statusLabel = new QLabel(QStringLiteral("Ready"), this);
    _progressBar = new QProgressBar(this);
    _progressBar->setVisible(false);
    _progressBar->setMaximumWidth(200);
    statusRow->addWidget(_statusLabel, 1);
    statusRow->addWidget(_progressBar);
    mainLayout->addLayout(statusRow);

    // Button row
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();

    _installBtn  = new QPushButton(QStringLiteral("Install"), this);
    _updateBtn   = new QPushButton(QStringLiteral("Update"), this);
    _removeBtn   = new QPushButton(QStringLiteral("Remove"), this);
    _refreshBtn  = new QPushButton(QStringLiteral("Refresh"), this);
    _closeBtn    = new QPushButton(QStringLiteral("Close"), this);

    connect(_installBtn, &QPushButton::clicked, this, &PluginsAdminDlg::onInstallClicked);
    connect(_updateBtn,  &QPushButton::clicked, this, &PluginsAdminDlg::onUpdateClicked);
    connect(_removeBtn,   &QPushButton::clicked, this, &PluginsAdminDlg::onRemoveClicked);
    connect(_refreshBtn,  &QPushButton::clicked, this, &PluginsAdminDlg::onRefreshClicked);
    connect(_closeBtn,    &QPushButton::clicked, this, [this]() { hide(); });

    btnRow->addWidget(_installBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(_updateBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(_removeBtn);
    btnRow->addSpacing(6);
    btnRow->addWidget(_refreshBtn);
    btnRow->addSpacing(12);
    btnRow->addWidget(_closeBtn);
    mainLayout->addLayout(btnRow);

    // Tab change → update description
    connect(_tabWidget, &QTabWidget::currentChanged, this, &PluginsAdminDlg::onTabChanged);

    // Item click → show description
    connect(_availableList, &QListWidget::itemClicked, this, &PluginsAdminDlg::onItemClicked);
    connect(_updateList,   &QListWidget::itemClicked, this, &PluginsAdminDlg::onItemClicked);
    connect(_installedList, &QListWidget::itemClicked, this, &PluginsAdminDlg::onItemClicked);
    connect(_incompatibleList, &QListWidget::itemClicked, this, &PluginsAdminDlg::onItemClicked);

    setLayout(mainLayout);
}

// =============================================================================
// initFromJson — load plugin list from JSON manifest
// Mirrors Win32 PluginsAdminDlg::initFromJson().
// =============================================================================

bool PluginsAdminDlg::initFromJson()
{
    // Load plugin list JSON (typically from nppPluginList.json or nppPluginList.dll)
    // On Linux: load from ~/.config/npp-plugins/nppPluginList.json

    QString jsonPath = _pluginListFullPath.isEmpty()
        ? QDir::homePath() + QStringLiteral("/.config/npp-plugins/nppPluginList.json")
        : _pluginListFullPath;

    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Fallback: add sample entries for demo
        addSamplePlugins();
        return true;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
        return false;

    QJsonObject root = doc.object();
    _pluginListVersion = root[QStringLiteral("version")].toString();

    QJsonArray plugins = root[QStringLiteral("npp-plugins")].toArray();
    for (const QJsonValue& val : plugins)
    {
        QJsonObject obj = val.toObject();
        PluginUpdateInfo* pi = new PluginUpdateInfo();
        pi->_folderName = obj[QStringLiteral("folder-name")].toString();
        pi->_displayName = obj[QStringLiteral("display-name")].toString();
        pi->_author = obj[QStringLiteral("author")].toString();
        pi->_description = obj[QStringLiteral("description")].toString();
        pi->_id = obj[QStringLiteral("id")].toString();
        pi->_repository = obj[QStringLiteral("repository")].toString();
        pi->_homepage = obj[QStringLiteral("homepage")].toString();

        QString verStr = obj[QStringLiteral("version")].toString();
        pi->_version = Version(verStr);

        // Parse compatibility
        QString compatStr = obj[QStringLiteral("npp-compatible-versions")].toString();
        if (!compatStr.isEmpty())
        {
            // Simple format: "[from,to]" or exact version
            // Stub: accept any version
            pi->_nppCompatibleVersions = {{Version(), Version(QStringLiteral("99.99.99.99"))}};
        }

        _allAvailablePlugins.push_back(pi);
        _availableList->_pluginList.push_back(pi);
    }

    return true;
}

void PluginsAdminDlg::addSamplePlugins()
{
    // Add sample plugins for demonstration when JSON not found
    PluginUpdateInfo sample1;
    sample1._displayName = QStringLiteral("JSON Viewer");
    sample1._folderName = QStringLiteral("JSONViewer");
    sample1._version = Version(QStringLiteral("1.2.0"));
    sample1._nppCompatibleVersions = {{Version(), Version(QStringLiteral("99.99.99.99"))}};
    sample1._description = QStringLiteral("Display JSON files in a tree view");
    sample1._author = QStringLiteral("Author Name");
    sample1._repository = QStringLiteral("https://github.com/example/json-viewer");
    _availableList->addPlugin(sample1);

    PluginUpdateInfo sample2;
    sample2._displayName = QStringLiteral("Explorer");
    sample2._folderName = QStringLiteral("Explorer");
    sample2._version = Version(QStringLiteral("0.9.0"));
    sample2._nppCompatibleVersions = {{Version(), Version(QStringLiteral("99.99.99.99"))}};
    sample2._description = QStringLiteral("File browser panel");
    sample2._author = QStringLiteral("Another Author");
    _availableList->addPlugin(sample2);
}

// =============================================================================
// initAvailablePluginsViewFromList — populate available list from loaded JSON
// Mirrors Win32 PluginsAdminDlg::initAvailablePluginsViewFromList().
// =============================================================================

bool PluginsAdminDlg::initAvailablePluginsViewFromList()
{
    // Filter by N++ version compatibility and add to available list
    // Requires NppParameters::getVersion() — stub for now
    for (PluginUpdateInfo* pi : _allAvailablePlugins)
    {
        _availableList->addPluginItem(pi->_displayName, pi->_version.toString(), pi);
    }
    return true;
}

// =============================================================================
// initIncompatiblePluginList — populate incompatible list
// Mirrors Win32 PluginsAdminDlg::initIncompatiblePluginList().
// =============================================================================

bool PluginsAdminDlg::initIncompatiblePluginList()
{
    // Add plugins that are not compatible with current N++ version
    return true;
}

// =============================================================================
// loadInstalledPlugins — scan installed plugins
// Mirrors Win32 PluginsAdminDlg::loadFromPluginInfos().
// =============================================================================

void PluginsAdminDlg::loadInstalledPlugins()
{
    _installedList->clearAll();
    // Requires PluginsManager access — stub for now
}

// =============================================================================
// loadFromPluginInfos — populate lists from PluginUpdateInfo objects
// Mirrors Win32 PluginsAdminDlg::loadFromPluginInfos().
// =============================================================================

bool PluginsAdminDlg::loadFromPluginInfos()
{
    collectNppCurrentStatusInfos();
    initAvailablePluginsViewFromList();
    initIncompatiblePluginList();
    loadInstalledPlugins();
    return true;
}

// =============================================================================
// collectNppCurrentStatusInfos — determine install location and admin mode
// Mirrors Win32 PluginsAdminDlg::collectNppCurrentStatusInfos().
// =============================================================================

void PluginsAdminDlg::collectNppCurrentStatusInfos()
{
    _nppCurrentStatus._isInProgramFiles = false; // Linux: always user dir
    _nppCurrentStatus._appdataPath = QDir::homePath() + QStringLiteral("/.config/npp-plugins/");
    _nppCurrentStatus._nppInstallPath = QApplication::applicationDirPath();
    _nppCurrentStatus._isAdminMode = false;
    _nppCurrentStatus._isAppDataPluginsAllowed = true;
}

// =============================================================================
// updateList — refresh the plugin lists
// Mirrors Win32 PluginsAdminDlg::updateList().
// =============================================================================

bool PluginsAdminDlg::updateList()
{
    setStatus(QStringLiteral("Refreshing plugin list..."));
    _progressBar->setVisible(true);
    _progressBar->setRange(0, 0); // Indeterminate

    bool ok = initFromJson() && loadFromPluginInfos();

    _progressBar->setVisible(false);
    setStatus(ok ? QStringLiteral("Ready") : QStringLiteral("Error loading plugin list"));
    return ok;
}

// =============================================================================
// Search
// Mirrors Win32 PluginsAdminDlg::searchInPlugins().
// =============================================================================

bool PluginsAdminDlg::isFoundInList(const PluginListView& list, int index, const QString& str, bool searchNames) const
{
    PluginUpdateInfo* info = list.pluginInfoAt(static_cast<size_t>(index));
    if (!info)
        return false;

    if (searchNames)
        return info->_displayName.contains(str, Qt::CaseInsensitive);
    else
        return info->_description.contains(str, Qt::CaseInsensitive);
}

long PluginsAdminDlg::searchFromCurrentSel(const PluginListView& list, const QString& str, bool searchNames, bool isNextMode) const
{
    long currentIndex = static_cast<long>(list.getSelectedIndex());
    int nbItem = list.count();

    if (currentIndex == -1)
    {
        for (int i = 0; i < nbItem; ++i)
        {
            if (isFoundInList(list, i, str, searchNames))
                return i;
        }
    }
    else
    {
        for (int i = static_cast<int>(currentIndex) + (isNextMode ? 1 : 0); i < nbItem; ++i)
        {
            if (isFoundInList(list, i, str, searchNames))
                return i;
        }
        for (int i = 0; i < currentIndex + (isNextMode ? 1 : 0); ++i)
        {
            if (isFoundInList(list, i, str, searchNames))
                return i;
        }
    }
    return -1;
}

bool PluginsAdminDlg::searchInPlugins(bool isNextMode) const
{
    QString text = _searchEdit->text();
    if (text.length() < 2)
        return false;

    PluginListView* currentList = nullptr;
    switch (_tabWidget->currentIndex())
    {
    case 3: currentList = _incompatibleList; break;
    case 2: currentList = _installedList; break;
    case 1: currentList = _updateList; break;
    default: currentList = _availableList; break;
    }

    long foundIndex = searchFromCurrentSel(*currentList, text, true, isNextMode);
    if (foundIndex == -1)
        foundIndex = searchFromCurrentSel(*currentList, text, false, isNextMode);

    if (foundIndex == -1)
        return false;

    currentList->setSelection(static_cast<int>(foundIndex));
    return true;
}

// =============================================================================
// Button handlers
// Mirrors Win32 PluginsAdminDlg button handlers.
// =============================================================================

void PluginsAdminDlg::onInstallClicked()
{
    auto checked = _availableList->getCheckedIndexes();
    if (checked.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Plugin Admin"),
            QStringLiteral("Please select plugin(s) to install."));
        return;
    }

    QVector<PluginUpdateInfo> toInstall;
    for (size_t idx : checked)
    {
        PluginUpdateInfo* pi = _availableList->pluginInfoAt(idx);
        if (pi)
            toInstall.append(*pi);
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        QStringLiteral("Install Plugins"),
        QStringLiteral("Install %1 plugin(s)?").arg(toInstall.size()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (installPlugins())
            emit pluginsInstalled(toInstall);
    }
}

void PluginsAdminDlg::onUpdateClicked()
{
    auto checked = _updateList->getCheckedIndexes();
    if (checked.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Plugin Admin"),
            QStringLiteral("Please select plugin(s) to update."));
        return;
    }
    updatePlugins();
}

void PluginsAdminDlg::onRemoveClicked()
{
    auto checked = _installedList->getCheckedIndexes();
    if (checked.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Plugin Admin"),
            QStringLiteral("Please select plugin(s) to remove."));
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::warning(this,
        QStringLiteral("Remove Plugins"),
        QStringLiteral("Remove %1 plugin(s)? This cannot be undone.").arg(checked.size()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (removePlugins())
            QMessageBox::information(this, QStringLiteral("Plugin Admin"),
                QStringLiteral("Plugins removed. Restart Notepad++ to complete."));
    }
}

void PluginsAdminDlg::onRefreshClicked()
{
    updateList();
}

void PluginsAdminDlg::onSearchChanged(const QString&)
{
    // Could implement live filtering here
}

void PluginsAdminDlg::onSearchNext()
{
    searchInPlugins(true);
}

void PluginsAdminDlg::onSearchPrev()
{
    searchInPlugins(false);
}

void PluginsAdminDlg::onTabChanged(int)
{
    // Update description for selected item
    PluginListView* currentList = nullptr;
    switch (_tabWidget->currentIndex())
    {
    case 3: currentList = _incompatibleList; break;
    case 2: currentList = _installedList; break;
    case 1: currentList = _updateList; break;
    default: currentList = _availableList; break;
    }

    size_t selIdx = currentList->getSelectedIndex();
    if (selIdx != SIZE_MAX)
    {
        PluginUpdateInfo* info = currentList->pluginInfoAt(selIdx);
        if (info)
            _descriptionLabel->setText(info->describe());
    }
}

void PluginsAdminDlg::onItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    PluginListView* list = qobject_cast<PluginListView*>(sender());
    if (!list)
        return;

    size_t idx = list->getSelectedIndex();
    PluginUpdateInfo* info = list->pluginInfoAt(idx);
    if (info)
        _descriptionLabel->setText(info->describe());
}

// =============================================================================
// installPlugins / updatePlugins / removePlugins
// Mirrors Win32 PluginsAdminDlg operations.
// These require filesystem access + NppParameters.
// =============================================================================

bool PluginsAdminDlg::installPlugins()
{
    setStatus(QStringLiteral("Installing plugins (requires plugin manager lift)..."));
    return false;
}

bool PluginsAdminDlg::updatePlugins()
{
    setStatus(QStringLiteral("Updating plugins (requires plugin manager lift)..."));
    return false;
}

bool PluginsAdminDlg::removePlugins()
{
    setStatus(QStringLiteral("Removing plugins (requires plugin manager lift)..."));
    return false;
}

// =============================================================================
// switchDialog — show a specific tab
// Mirrors Win32 PluginsAdminDlg::switchDialog(int).
// =============================================================================

void PluginsAdminDlg::switchDialog(int indexToSwitch)
{
    if (indexToSwitch >= 0 && indexToSwitch < _tabWidget->count())
        _tabWidget->setCurrentIndex(indexToSwitch);
}

void PluginsAdminDlg::changeTabName(PluginListType index, const QString& name)
{
    if (index >= 0 && index <= 3)
        _tabWidget->setTabText(index, name);
}

void PluginsAdminDlg::changeColumnName(int, const QString&)
{
    // Not applicable in Qt6 — using QListWidget
}

// =============================================================================
// doDialog — show the dialog non-modally
// Mirrors Win32 PluginsAdminDlg::doDialog().
// =============================================================================

void PluginsAdminDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    if (!isVisible())
    {
        loadFromPluginInfos();
    }
    show();
    raise();
}

// =============================================================================
// List getters
// =============================================================================

QVector<PluginUpdateInfo> PluginsAdminDlg::getAvailablePlugins() const
{
    QVector<PluginUpdateInfo> result;
    for (int i = 0; i < _availableList->count(); ++i)
    {
        PluginUpdateInfo* info = _availableList->pluginInfoAt(static_cast<size_t>(i));
        if (info)
            result.append(*info);
    }
    return result;
}

QVector<PluginUpdateInfo> PluginsAdminDlg::getInstalledPlugins() const
{
    QVector<PluginUpdateInfo> result;
    for (int i = 0; i < _installedList->count(); ++i)
    {
        PluginUpdateInfo* info = _installedList->pluginInfoAt(static_cast<size_t>(i));
        if (info)
            result.append(*info);
    }
    return result;
}

void PluginsAdminDlg::setStatus(const QString& msg)
{
    _statusLabel->setText(msg);
}

// =============================================================================
// run_dlgProc — message dispatcher
// Mirrors Win32 PluginsAdminDlg::run_dlgProc.
// =============================================================================

intptr_t PluginsAdminDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        loadFromPluginInfos();
        return TRUE;

    case WM_DESTROY:
        return TRUE;

    default:
        break;
    }
    return 0;
}


std::wstring PluginsAdminDlg::getPluginListVerStr() const
{
    return L"";  // no-op stub for Qt6 port
}
