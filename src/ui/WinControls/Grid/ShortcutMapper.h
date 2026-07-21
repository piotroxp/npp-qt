// ShortcutMapper.h - Qt6 port of ShortcutMapper dialog
// INTENT: source is a CDialog that uses BabyGrid to display/manage keyboard shortcuts.
// Target uses QDialog with QTabWidget + BabyGridWrapper to provide the same functionality.
// Columns: Category, Shortcut Name, Current Key, Assigned To.
// Tabs: Main menu, Macros, Run commands, Plugin commands, Scintilla commands.

#pragma once

#include "BabyGridWrapper.h"
#include "MISC/Common/shortcut.h"
#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QFont>
#include <QMenu>
#include <vector>
#include <string>

enum class GridState { STATE_MENU, STATE_MACRO, STATE_USER, STATE_PLUGIN, STATE_SCINTILLA };

// Re-use KeyCombo from shortcut.h

// Local shortcut item types (named to avoid clash with Shortcut QObject from shortcut.h)
struct ShortcutItem {
    QString _name;
    KeyCombo _keyCombo;
    int _id = 0;
};

struct PluginCmdShortcutItem {
    QString _name;
    KeyCombo _keyCombo;
    int _id = 0;
    QString _pluginName;
};

struct ScintillaKeyMapItem {
    QString _name;
    KeyCombo _keyCombo;
    int _sciMessage = 0;
};

// ShortcutMapper dialog — shows all keyboard shortcuts in a sortable grid.
class ShortcutMapper : public QDialog {
    Q_OBJECT

public:
    explicit ShortcutMapper(QWidget* parent = nullptr);
    ~ShortcutMapper() override = default;

    // Initialize with parent window handle (for DPI info)
    void init(QWidget* parent, GridState initState = GridState::STATE_MENU);

    // Show the dialog
    void doDialog(bool isRTL = false);

    // Column indices for the BabyGrid
    enum Col : int { COL_CATEGORY = 0, COL_NAME = 1, COL_SHORTCUT = 2, COL_ASSIGNED = 3 };

public slots:
    void onTabChanged(int index);
    void onFilterChanged(const QString& text);
    void onCellDoubleClicked(int row, int col);
    void onClearFilter();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUi();
    void setupTabs();
    void setupBabyGrid();
    void setupFilter();
    void fillGrid();
    void populateMainMenuTab();
    void populateMacrosTab();
    void populateRunCommandsTab();
    void populatePluginCommandsTab();
    void populateScintillaTab();

    bool isFilterValid(const ShortcutItem& sc) const;
    bool isFilterValid(const PluginCmdShortcutItem& sc) const;
    bool isFilterValid(const ScintillaKeyMapItem& sc) const;

    QString shortcutToString(const KeyCombo& kc) const;
    QString tabName(size_t i) const;

    static bool keyComboEqual(const KeyCombo& lhs, const KeyCombo& rhs) {
        return lhs._isCtrl == rhs._isCtrl && lhs._isAlt == rhs._isAlt
            && lhs._isShift == rhs._isShift && lhs._key == rhs._key;
    }

    QTabWidget* _tabWidget = nullptr;
    BabyGridWrapper* _babyGrid = nullptr;
    QLineEdit* _filterEdit = nullptr;
    QLabel* _infoLabel = nullptr;
    QMenu* _rightClickMenu = nullptr;

    GridState _currentState = GridState::STATE_MENU;
    QString _filterText;
    std::vector<ShortcutItem> _mainMenuShortcuts;
    std::vector<ShortcutItem> _macros;
    std::vector<ShortcutItem> _userShortcuts;
    std::vector<PluginCmdShortcutItem> _pluginShortcuts;
    std::vector<ScintillaKeyMapItem> _scintillaMaps;
    std::vector<size_t> _shortcutIndex;

    std::vector<QFont> _gridFonts;
    enum GridFont { GFONT_HEADER, GFONT_ROWS, MAX_GRID_FONTS };

    int _idCounter = 0;
    int nextId() { return ++_idCounter; }
};
