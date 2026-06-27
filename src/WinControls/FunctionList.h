// Semantic Lift: Win32 FunctionList → Qt6 Function List Panel
// Original: PowerEditor/src/WinControls/FunctionList/
// Target: npp-qt/src/WinControls/FunctionList.h + .cpp

#pragma once

#include <vector>
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QFont>
#include <QIcon>
#include <QVector>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QScrollBar>
#include <QHeaderView>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <vector>

struct TreeStateNode;
struct SearchParameters;
struct TreeParams;
#include "functionParser.h"

// =============================================================================
// FunctionListTreeWidget — lifted from TreeView for function list context
// =============================================================================

class FunctionListTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit FunctionListTreeWidget(QWidget* parent = nullptr);
    ~FunctionListTreeWidget() override = default;

    // Image indices for tree items
    static constexpr int INDEX_ROOT = 0;
    static constexpr int INDEX_NODE = 1;
    static constexpr int INDEX_LEAF = 2;

    // Tree operations
    QTreeWidgetItem* searchSubItemByName(const QString& name, QTreeWidgetItem* root) const;
    void makeLabelEditable(bool editable);
    void setItemParam(QTreeWidgetItem* item, const QVariant& param);
    QVariant getItemParam(QTreeWidgetItem* item) const;
    void removeAllItems();
    void removeItem(QTreeWidgetItem* item);
    void expand(QTreeWidgetItem* item);
    void fold(QTreeWidgetItem* item);
    void foldAll();
    void expandAll();
    void selectItem(QTreeWidgetItem* item);
    QTreeWidgetItem* getRoot() const;
    QTreeWidgetItem* getChildFrom(QTreeWidgetItem* parent) const;
    QTreeWidgetItem* getNextSibling(QTreeWidgetItem* item) const;
    QTreeWidgetItem* getParent(QTreeWidgetItem* item) const;
    void toggleExpandCollapse(QTreeWidgetItem* item);
    void setItemImage(QTreeWidgetItem* item, int openIdx, int closeIdx);
    void restoreFoldingStateFrom(const TreeStateNode& state, QTreeWidgetItem* root);
    bool retrieveFoldingStateTo(TreeStateNode& state, QTreeWidgetItem* root) const;
    void searchLeafAndBuildTree(FunctionListTreeWidget& target, const QString& searchText, int leafIndex);

    void setBackgroundColor(const QColor& col);
    void setForegroundColor(const QColor& col);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void itemExpandToggle(QTreeWidgetItem* item);
    void labelEditRequested(QTreeWidgetItem* item);

private:
    void buildFoldingState(QTreeWidgetItem* item, TreeStateNode& state) const;
    bool _isLabelEditable = false;
};

// =============================================================================
// FunctionListPanel — main docking panel (lifted from FunctionListPanel)
// =============================================================================

class FunctionListPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    ~FunctionListPanel() override;

    void init(QApplication* app, QWidget* parent);
    void setParentWindow(QWidget* parent) { _hParent = parent; }

    // Wire parser — call after ScintillaComponent is ready.
    // xmlDir: path to functionList/ dir with .xml rule files.
    // ppEditView: pointer-to-pointer to the current ScintillaComponent.
    void initParser(const QString& xmlDir, ScintillaEditView** ppEditView);

    void setBackgroundColor(const QColor& col);
    void setForegroundColor(const QColor& col);

    // State save/restore
    void sortOrUnsort();
    void reload();
    void markEntry();
    bool serialize(const QString& outputFilename = QString());
    void searchFuncAndSwitchView();

public slots:
    void onSortToggled();
    void onReloadClicked();
    void onPreferencesClicked();
    void onInitialSortAction();
    void onSearchTextChanged(const QString& text);
    void onEditorUiChanged();
    void cleanup();

signals:
    void closeRequested();
    void functionSelected(int position);
    void reloadRequested();

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    // Internal data
    void addEntry(const QString& nodeName, const QString& displayText, size_t pos);
    void removeAllEntries();
    bool openSelection(const FunctionListTreeWidget& treeView);
    bool shouldSort() const;
    void setSort(bool isEnabled);
    void findMarkEntry(QTreeWidgetItem* htItem, long line);
    void initPreferencesMenu();
    void showPreferencesMenu();

    // Tree state management
    void addInStateArray(TreeStateNode tree2Update, const QString& searchText, bool isSorted);
    TreeParams* getFromStateArray(const QString& fullFilePath);

    // Callback for tree sort
    static int categorySortFunc(const QTreeWidgetItem* item1, const QTreeWidgetItem* item2);

    // Toolbar
    QToolBar* _toolbar = nullptr;
    QLineEdit* _searchEdit = nullptr;
    QAction* _sortAction = nullptr;
    QAction* _reloadAction = nullptr;
    QAction* _preferencesAction = nullptr;
    QFont _searchFont;

    // Tree views (main + search result)
    FunctionListTreeWidget* _pTreeView = nullptr;
    FunctionListTreeWidget _treeView;
    FunctionListTreeWidget _treeViewSearchResult;

    // Scroll state
    int _scrollPos = 0;
    long _findLine = -1;
    long _findEndLine = -1;
    QTreeWidgetItem* _findItem = nullptr;

    // Localization strings
    QString _sortTipStr = QStringLiteral("Sort");
    QString _reloadTipStr = QStringLiteral("Reload");
    QString _preferenceTipStr = QStringLiteral("Preferences");

    // Parsed function info
    std::vector<foundInfo> _foundFuncInfos;

    // Stored string pointers (for LPARAM compatibility)
    QVector<QString*> _posStrs;

    ScintillaEditView** _ppEditView = nullptr;
    FunctionParsersManager _funcParserMgr;

    // Saved tree states per file
    QVector<TreeParams> _treeParams;

    // Parent window
    QWidget* _hParent = nullptr;

    // Preferences menu
    QMenu* _preferencesMenu = nullptr;
};

// =============================================================================
// Supporting structures (lifted from functionListPanel.h)
// =============================================================================

struct SearchParameters {
    QString _text2Find;
    bool _doSort = false;

    bool hasParams() const {
        return (!_text2Find.isEmpty() || _doSort);
    }
};

struct TreeParams {
    TreeStateNode _treeState;
    SearchParameters _searchParameters;
};
