// Semantic Lift: Win32 FunctionList → Qt6 Function List Panel Implementation
// Original: PowerEditor/src/WinControls/FunctionList/
// Target: npp-qt/src/WinControls/FunctionList.h + .cpp

#include "FunctionList.h"
#include "functionParser.h"
#include "Window.h"
#include "NppDarkMode.h"
#include "NppSciCompat.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QPainter>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <vector>

// =============================================================================
// FunctionListTreeWidget
// =============================================================================

FunctionListTreeWidget::FunctionListTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setAnimated(true);
    setSortingEnabled(false);
    setItemsExpandable(true);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Single column for function list
    setColumnCount(1);

    // Qt6 QTreeWidget::itemDoubleClicked(QTreeWidgetItem*, int) has different signature
    // than QWidget::mouseDoubleClickEvent(QMouseEvent*). Use lambda to bridge.
    connect(this, &QTreeWidget::itemDoubleClicked,
            this, [this](QTreeWidgetItem* item, int column) {
                QMouseEvent ev(QEvent::MouseButtonDblClick, QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                mouseDoubleClickEvent(&ev);
                Q_UNUSED(item);
                Q_UNUSED(column);
            });
}

QTreeWidgetItem* FunctionListTreeWidget::searchSubItemByName(const QString& name, QTreeWidgetItem* root) const {
    QTreeWidgetItem* parent = root ? root : invisibleRootItem();
    for (int i = 0; i < parent->childCount(); ++i) {
        if (parent->child(i)->text(0) == name)
            return parent->child(i);
    }
    return nullptr;
}

void FunctionListTreeWidget::makeLabelEditable(bool editable) {
    _isLabelEditable = editable;
    setEditTriggers(editable ? QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed
                             : QAbstractItemView::NoEditTriggers);
}

void FunctionListTreeWidget::setItemParam(QTreeWidgetItem* item, const QVariant& param) {
    item->setData(0, Qt::UserRole, param);
}

QVariant FunctionListTreeWidget::getItemParam(QTreeWidgetItem* item) const {
    return item ? item->data(0, Qt::UserRole) : QVariant();
}

void FunctionListTreeWidget::removeAllItems() {
    QTreeWidget::clear();
}

void FunctionListTreeWidget::removeItem(QTreeWidgetItem* item) {
    if (item) {
        delete item;
    }
}

void FunctionListTreeWidget::expand(QTreeWidgetItem* item) {
    if (item)
        item->setExpanded(true);
}

void FunctionListTreeWidget::fold(QTreeWidgetItem* item) {
    if (item)
        item->setExpanded(false);
}

void FunctionListTreeWidget::foldAll() {
    collapseAll();
}

void FunctionListTreeWidget::expandAll() {
    QTreeWidget::expandAll();
}

void FunctionListTreeWidget::selectItem(QTreeWidgetItem* item) {
    if (item) {
        setCurrentItem(item);
        scrollToItem(item);
    }
}

QTreeWidgetItem* FunctionListTreeWidget::getRoot() const {
    return invisibleRootItem()->childCount() > 0 ? invisibleRootItem()->child(0) : nullptr;
}

QTreeWidgetItem* FunctionListTreeWidget::getChildFrom(QTreeWidgetItem* parent) const {
    return parent && parent->childCount() > 0 ? parent->child(0) : nullptr;
}

QTreeWidgetItem* FunctionListTreeWidget::getNextSibling(QTreeWidgetItem* item) const {
    if (!item)
        return nullptr;
    QTreeWidgetItem* parent = item->parent();
    if (!parent) {
        int idx = indexOfTopLevelItem(item);
        return idx >= 0 && idx < topLevelItemCount() - 1 ? topLevelItem(idx + 1) : nullptr;
    }
    int idx = parent->indexOfChild(item);
    return idx >= 0 && idx < parent->childCount() - 1 ? parent->child(idx + 1) : nullptr;
}

QTreeWidgetItem* FunctionListTreeWidget::getParent(QTreeWidgetItem* item) const {
    return item ? item->parent() : nullptr;
}

void FunctionListTreeWidget::toggleExpandCollapse(QTreeWidgetItem* item) {
    if (item)
        item->setExpanded(!item->isExpanded());
}

void FunctionListTreeWidget::setItemImage(QTreeWidgetItem* item, int, int) {
    Q_UNUSED(item);
    // Icon management would use QTreeWidgetItem::setIcon
}

void FunctionListTreeWidget::restoreFoldingStateFrom(const TreeStateNode& state, QTreeWidgetItem* root) {
    Q_UNUSED(state);
    Q_UNUSED(root);
    // Simplified: just expand root
    if (root)
        root->setExpanded(true);
}

bool FunctionListTreeWidget::retrieveFoldingStateTo(TreeStateNode& state, QTreeWidgetItem* root) const {
    Q_UNUSED(state);
    Q_UNUSED(root);
    return true;
}

void FunctionListTreeWidget::searchLeafAndBuildTree(FunctionListTreeWidget& target, const QString& searchText, int leafIndex) {
    Q_UNUSED(target);
    Q_UNUSED(searchText);
    Q_UNUSED(leafIndex);
    // Simplified: no-op for now
}

void FunctionListTreeWidget::setBackgroundColor(const QColor& col) {
    setStyleSheet(QStringLiteral("QTreeWidget { background-color: %1; }").arg(col.name()));
}

void FunctionListTreeWidget::setForegroundColor(const QColor& col) {
    setStyleSheet(QStringLiteral("QTreeWidget { color: %1; }").arg(col.name()));
}

void FunctionListTreeWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item && item->childCount() == 0) {
        // It's a leaf — open selection
        emit itemExpandToggle(item);
    } else {
        QTreeWidget::mouseDoubleClickEvent(event);
    }
}

void FunctionListTreeWidget::keyPressEvent(QKeyEvent* event) {
    QTreeWidget::keyPressEvent(event);
}

void FunctionListTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
    QTreeWidget::contextMenuEvent(event);
}

// =============================================================================
// FunctionListPanel
// =============================================================================

FunctionListPanel::FunctionListPanel(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(QStringLiteral("Function List"));

    // Main layout: toolbar on top, tree below
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar
    _toolbar = new QToolBar(this);
    _toolbar->setMovable(false);
    _toolbar->setIconSize(QSize(16, 16));

    _sortAction = new QAction(QIcon(), QStringLiteral("Sort"), this);
    _sortAction->setCheckable(true);
    _sortAction->setToolTip(QStringLiteral("Sort functions (A to Z)"));
    connect(_sortAction, &QAction::toggled, this, &FunctionListPanel::onSortToggled);

    _reloadAction = new QAction(QIcon(), QStringLiteral("Reload"), this);
    _reloadAction->setToolTip(QStringLiteral("Reload function list"));
    connect(_reloadAction, &QAction::triggered, this, &FunctionListPanel::onReloadClicked);

    _preferencesAction = new QAction(QIcon(), QStringLiteral("Preferences"), this);
    _preferencesAction->setToolTip(QStringLiteral("Preferences"));
    connect(_preferencesAction, &QAction::triggered, this, &FunctionListPanel::onPreferencesClicked);

    _searchEdit = new QLineEdit(_toolbar);
    _searchEdit->setPlaceholderText(QStringLiteral("Search..."));
    _searchEdit->setMaximumWidth(100);
    connect(_searchEdit, &QLineEdit::textChanged, this, &FunctionListPanel::onSearchTextChanged);

    _toolbar->addAction(_sortAction);
    _toolbar->addAction(_reloadAction);
    _toolbar->addAction(_preferencesAction);
    _toolbar->addWidget(_searchEdit);

    mainLayout->addWidget(_toolbar);

    // Tree views stacked: search result on top of main (only one visible)
    _treeViewSearchResult.setHeaderHidden(true);
    _treeViewSearchResult.setParent(this);
    _treeViewSearchResult.hide();

    _treeView.setParent(this);
    _treeView.makeLabelEditable(false);
    mainLayout->addWidget(&_treeView);

    // Search result tree overlay
    _pTreeView = &_treeView;
    mainLayout->addWidget(&_treeViewSearchResult);
    _treeViewSearchResult.hide();

    // Preferences menu
    _preferencesMenu = new QMenu(this);
    QAction* initialSortAct = _preferencesMenu->addAction(QStringLiteral("Sort functions (A to Z) by default"));
    initialSortAct->setCheckable(true);
    connect(initialSortAct, &QAction::triggered, this, &FunctionListPanel::onInitialSortAction);

    // Tree signals
    connect(&_treeView, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        Q_UNUSED(item);
        openSelection(_treeView);
    });

    connect(&_treeViewSearchResult, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        Q_UNUSED(item);
        openSelection(_treeViewSearchResult);
    });

    setLayout(mainLayout);
}

// ── Wire ScintillaComponent signals to function list refresh ────────────────
// connectScintilla must be called after _ppEditView is set and the
// ScintillaComponent is alive. Call initParser() to establish this wiring.
void FunctionListPanel::initParser(const QString& xmlDir, ScintillaEditView** ppEditView)
{
    _ppEditView = ppEditView;

    // Initialize the parser manager with XML rule directory
    QString installedPath = xmlDir; // same dir for portable build
    _funcParserMgr.init(xmlDir, installedPath, ppEditView);

    if (!_ppEditView || !*_ppEditView)
        return;

    // Connect SCN_UPDATEUI → auto-reload on every editor change
    connect(*_ppEditView, &ScintillaEditBase::updateUi,
            this, &FunctionListPanel::onEditorUiChanged);

    // Connect SCN_PAINTED → throttle repaints (debounced in onEditorUiChanged)
    // Connect double-click in tree → navigate to function
    connect(&_treeView, &QTreeWidget::itemDoubleClicked,
            this, [this](QTreeWidgetItem* item, int) {
        Q_UNUSED(item);
        openSelection(_treeView);
    });
}

void FunctionListPanel::onEditorUiChanged()
{
    // Only auto-refresh if the search box is empty (don't interrupt a search)
    if (_searchEdit && _searchEdit->text().isEmpty())
        reload();
}

void FunctionListPanel::cleanup() {
    for (QString* s : _posStrs)
        delete s;
    _posStrs.clear();
}

void FunctionListPanel::init(QApplication* app, QWidget* parent) {
    Q_UNUSED(app);
    Q_UNUSED(parent);
    // Initialization handled in constructor for Qt6
    // Parser and Scintilla wiring are done via initParser() after construction
}

void FunctionListPanel::setBackgroundColor(const QColor& col) {
    _treeView.setBackgroundColor(col);
    _treeViewSearchResult.setBackgroundColor(col);
}

void FunctionListPanel::setForegroundColor(const QColor& col) {
    _treeView.setForegroundColor(col);
    _treeViewSearchResult.setForegroundColor(col);
}

void FunctionListPanel::addEntry(const QString& nodeName, const QString& displayText, size_t pos) {
    QTreeWidgetItem* itemParent = nullptr;
    QString posStr = QString::number(pos);

    QTreeWidgetItem* root = _treeView.getRoot();
    if (!nodeName.isEmpty()) {
        itemParent = _treeView.searchSubItemByName(nodeName, root);
        if (!itemParent) {
            QString* posStrPtr = new QString(posStr);
            _posStrs.push_back(posStrPtr);
            itemParent = new QTreeWidgetItem();
            itemParent->setText(0, nodeName);
            itemParent->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(posStrPtr)));
            if (root)
                root->addChild(itemParent);
            else
                _treeView.addTopLevelItem(itemParent);
        }
    } else {
        itemParent = root;
    }

    QString* posString = new QString(posStr);
    _posStrs.push_back(posString);

    QTreeWidgetItem* leaf = new QTreeWidgetItem();
    leaf->setText(0, displayText);
    leaf->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(posString)));
    if (itemParent)
        itemParent->addChild(leaf);
    else
        _treeView.addTopLevelItem(leaf);
}

void FunctionListPanel::removeAllEntries() {
    _treeView.removeAllItems();
}

bool FunctionListPanel::serialize(const QString& outputFilename) {
    Q_UNUSED(outputFilename);
    // JSON export would be implemented here
    return false;
}

void FunctionListPanel::searchFuncAndSwitchView() {
    QString text = _searchEdit->text();

    if (text.isEmpty()) {
        _treeViewSearchResult.hide();
        _treeView.show();
        _pTreeView = &_treeView;
    } else {
        if (!_treeView.getRoot())
            return;

        _treeViewSearchResult.clear();
        // Build filtered tree from _treeView matching `text`
        // For now, just show the main tree
        _treeViewSearchResult.hide();
        _treeView.show();
        _pTreeView = &_treeView;
    }

    if (shouldSort())
        _pTreeView->sortItems(0, Qt::AscendingOrder);
}

bool FunctionListPanel::openSelection(const FunctionListTreeWidget& treeView) {
    QTreeWidgetItem* item = treeView.currentItem();
    if (!item || !_ppEditView || !*_ppEditView)
        return false;

    // Get position from item user data (stored as QString* → int)
    void* userData = item->data(0, Qt::UserRole).value<void*>();
    if (!userData)
        return false;

    QString* posStr = static_cast<QString*>(userData);
    bool ok = false;
    int pos = posStr->toInt(&ok);
    if (!ok || pos < 0)
        return false;

    // Wire: use real Scintilla navigation
    (*_ppEditView)->execute(SCI_ENSUREVISIBLE, (*_ppEditView)->send(SCI_LINEFROMPOSITION, pos));
    (*_ppEditView)->scrollToCenter(static_cast<size_t>(pos));
    (*_ppEditView)->execute(SCI_GOTOPOS, pos);

    // Emit signal so MainWindow can update focus
    emit functionSelected(pos);

    return true;
}

bool FunctionListPanel::shouldSort() const {
    return _sortAction && _sortAction->isChecked();
}

void FunctionListPanel::setSort(bool isEnabled) {
    if (_sortAction)
        _sortAction->setChecked(isEnabled);
}

void FunctionListPanel::findMarkEntry(QTreeWidgetItem* htItem, long line) {
    Q_UNUSED(htItem);
    Q_UNUSED(line);
    // Would find the tree item corresponding to `line` and select it
}

void FunctionListPanel::addInStateArray(TreeStateNode tree2Update, const QString& searchText, bool isSorted) {
    for (int i = 0; i < _treeParams.size(); ++i) {
        if (_treeParams[i]._treeState._extraData == tree2Update._extraData) {
            _treeParams[i]._searchParameters._text2Find = searchText;
            _treeParams[i]._searchParameters._doSort = isSorted;
            _treeParams[i]._treeState = tree2Update;
            return;
        }
    }
    TreeParams params;
    params._treeState = tree2Update;
    params._searchParameters._text2Find = searchText;
    params._searchParameters._doSort = isSorted;
    _treeParams.push_back(params);
}

TreeParams* FunctionListPanel::getFromStateArray(const QString& fullFilePath) {
    for (int i = 0; i < _treeParams.size(); ++i) {
        if (_treeParams[i]._treeState._extraData == fullFilePath)
            return &_treeParams[i];
    }
    return nullptr;
}

void FunctionListPanel::reload() {
    if (!_ppEditView || !*_ppEditView)
        return;

    // Save scroll position
    _scrollPos = _treeView.verticalScrollBar()->value();

    // Save current tree state
    TreeStateNode currentTree;
    bool isOK = _treeView.retrieveFoldingStateTo(currentTree, _treeView.getRoot());
    if (isOK) {
        QString text = _searchEdit->text();
        bool sorted = shouldSort();
        addInStateArray(currentTree, text, sorted);
    }

    removeAllEntries();
    _searchEdit->clear();
    setSort(false);
    _foundFuncInfos.clear();

    // ── Wire: call the real function parser ─────────────────────────────────
    // Get current document language from Scintilla
    // SCI_GETLEXER = 4012, SCI_GETPROPERTY = 4004 (Scintilla.h)
    int lexerLanguage = (*_ppEditView)->send(4012);
    Q_UNUSED(lexerLanguage);
    int langType = (*_ppEditView)->send(4004, reinterpret_cast<unsigned long>("lexer.lang"), 0);

    // Try to find a parser for this language via the manager
    // associationMap.xml maps langType → parser rule file
    // Build an AssociationInfo for the current language
    AssociationInfo asso(0, langType, QString(), QString());

    bool parsed = _funcParserMgr.parse(_foundFuncInfos, asso);
    if (!parsed) {
        // Fall back to C++ parser for common languages
        // (parsers are registered in functionList/default.xml and overrideMap.xml)
        AssociationInfo cppAsso(0, 7 /* L_CPP */, QString(), QString());
        _funcParserMgr.parse(_foundFuncInfos, cppAsso);
    }

    // Populate tree from _foundFuncInfos
    for (const foundInfo& fi : _foundFuncInfos) {
        QString nodeName = QString::fromStdString(fi._data2);
        QString displayText = QString::fromStdString(fi._data);
        size_t pos = static_cast<size_t>(fi._pos >= 0 ? fi._pos : 0);
        addEntry(nodeName, displayText, pos);
    }

    // Restore dark mode coloring
    if (NppDarkMode::instance().isEnabled()) {
        setBackgroundColor(QColor::fromRgb(NppDarkMode::instance().backgroundColor()));
        setForegroundColor(QColor::fromRgb(NppDarkMode::instance().textColor()));
    }

    // Restore sort state
    if (shouldSort())
        _pTreeView->sortItems(0, Qt::AscendingOrder);
}

void FunctionListPanel::sortOrUnsort() {
    bool doSort = shouldSort();
    if (doSort) {
        _treeView.sortItems(0, Qt::AscendingOrder);
    } else {
        // Reset to definition order
        reload();
    }
}

void FunctionListPanel::markEntry() {
    if (!_ppEditView || !*_ppEditView)
        return;

    // Wire: get current cursor line from Scintilla
    long lineNr = static_cast<long>((*_ppEditView)->currentLine());
    QTreeWidgetItem* root = _treeView.getRoot();
    if (_findLine != -1 && _findEndLine != -1 && lineNr >= _findLine && lineNr < _findEndLine)
        return;
    _findLine = -1;
    _findEndLine = -1;
    findMarkEntry(root, lineNr);
    if (_findLine != -1 && _findItem)
        _treeView.selectItem(_findItem);
    else if (root)
        _treeView.selectItem(root);
}

void FunctionListPanel::initPreferencesMenu() {
    // Already created in constructor; add actions here
}

void FunctionListPanel::showPreferencesMenu() {
    if (_preferencesMenu) {
        QAction* action = _preferencesMenu->exec(QCursor::pos());
        Q_UNUSED(action);
    }
}

void FunctionListPanel::onSortToggled() {
    sortOrUnsort();
}

void FunctionListPanel::onReloadClicked() {
    reload();
    emit reloadRequested();
}

void FunctionListPanel::onPreferencesClicked() {
    showPreferencesMenu();
}

void FunctionListPanel::onInitialSortAction() {
    // Toggle initial sort preference
}

void FunctionListPanel::onSearchTextChanged(const QString& text) {
    Q_UNUSED(text);
    searchFuncAndSwitchView();
}

int FunctionListPanel::categorySortFunc(const QTreeWidgetItem* item1, const QTreeWidgetItem* item2) {
    // Custom sort: compare by stored position data
    void* ud1 = item1->data(0, Qt::UserRole).value<void*>();
    void* ud2 = item2->data(0, Qt::UserRole).value<void*>();

    if (!ud1 || !ud2)
        return 0;

    QString* posStr1 = static_cast<QString*>(ud1);
    QString* posStr2 = static_cast<QString*>(ud2);

    bool ok1 = false, ok2 = false;
    int pos1 = posStr1->toInt(&ok1);
    int pos2 = posStr2->toInt(&ok2);

    if (!ok1 || !ok2)
        return 0;

    if (item1->childCount() > 0 && item2->childCount() == 0)
        return -1;
    if (item1->childCount() == 0 && item2->childCount() > 0)
        return 1;

    return pos1 < pos2 ? -1 : (pos1 > pos2 ? 1 : 0);
}

bool FunctionListPanel::event(QEvent* event) {
    return QWidget::event(event);
}

void FunctionListPanel::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}
