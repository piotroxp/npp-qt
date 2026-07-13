// Semantic Lift: Win32 ShortcutMapper → Qt6 implementation
// Original: PowerEditor/src/WinControls/shortcut/shortcut.h + shortcut.cpp (2215 lines)
// Target: npp-qt/src/WinControls/ShortcutMapper.h
//
// Lifts the keyboard shortcut mapper dialog from Win32 ListView + hotkey control
// to Qt6 QTableWidget with QKeySequenceEdit for key capture.
// Follows the npp-qt WinControls translation pattern.

#pragma once

#include "StaticDialog.h"
#include "ScintillaComponent.h"
#include "MISC/Common/keys.h"
using namespace NppKeys;
#include <QTableWidget>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QVector>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>  // ShortcutTableDelegate base class
#include <vector>
#include <string>

// Key modifier flags (mirrors Win32 FVIRTKEY/F_CONTROL/etc.)
const int FVIRTKEY  = 0x01;
const int FCONTROL  = 0x02;
const int FALT      = 0x04;
const int FSHIFT    = 0x08;

// Win32 Virtual Key codes used in KeyCombo::isValid()
// All VK_* constants are defined in NppConstants.h (VK_SPACE, VK_CAPITAL, etc.)
// This file uses them via NppConstants.h inclusion chain.

// Scintilla key translate (mirrors Win32 keyTranslate())
// Use NPP_SCK_ prefix to avoid conflict with Scintilla.h macros (SCK_ESCAPE, etc.)
enum ScintillaKey {
    NPP_SCK_DOWN = 2000, NPP_SCK_UP, NPP_SCK_LEFT, NPP_SCK_RIGHT,
    NPP_SCK_HOME, NPP_SCK_END, NPP_SCK_PRIOR, NPP_SCK_NEXT,
    NPP_SCK_DELETE, NPP_SCK_INSERT, NPP_SCK_ESCAPE, NPP_SCK_BACK,
    NPP_SCK_TAB, NPP_SCK_RETURN, NPP_SCK_ADD, NPP_SCK_SUBTRACT,
    NPP_SCK_DIVIDE
};

// =============================================================================
// ShortcutKeyCombo — accelerator key combination for shortcut mapper
// (Local version to avoid conflict with shortcut.h's KeyCombo)
// =============================================================================

struct ShortcutKeyCombo {
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    unsigned char _key = 0;

    bool isEnabled() const { return _key != 0; }
    bool isValid() const {
        if (_key == 0) return true; // disabled is valid
        if ((_key >= 'A' && _key <= 'Z') ||
            (_key >= '0' && _key <= '9') ||
            _key == VK_SPACE || _key == VK_CAPITAL || _key == VK_BACK || _key == VK_RETURN)
            return _isCtrl || _isAlt;
        return true;
    }
};

// =============================================================================
// ShortcutItem — data for one shortcut table row
// Mirrors Win32 Shortcut / CommandShortcut / ScintillaKeyMap.
// =============================================================================

struct ShortcutItem {
    QString _name;          // Display name
    QString _category;      // Menu category (e.g. "File", "Edit")
    ShortcutKeyCombo _keyCombo;     // Current accelerator
    int _id = 0;            // Command ID
    int _type = 0;          // 0=command, 1=scintilla, 2=macro

    QString keyString() const {
        if (!isEnabled()) return QStringLiteral("(none)");
        QString s;
        if (_keyCombo._isCtrl) s += "Ctrl+";
        if (_keyCombo._isAlt) s += "Alt+";
        if (_keyCombo._isShift) s += "Shift+";
        s += QChar(_keyCombo._key);
        return s;
    }
    bool isEnabled() const { return _keyCombo._key != 0; }
};

// =============================================================================
// ShortcutTableDelegate — custom delegate for key sequence editing
// Mirrors Win32 hotkey control + LVN_GETDISPINFO.
// KeySequenceEdit — top-level widget for editing key sequences
// (moc doesn't support Q_OBJECT in nested classes)
class KeySequenceEdit : public QKeySequenceEdit
{
    Q_OBJECT
public:
    explicit KeySequenceEdit(QWidget* parent = nullptr);
    ShortcutKeyCombo keyCombo() const;
    void setKeyCombo(const ShortcutKeyCombo& kc);
};

// =============================================================================

class ShortcutTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ShortcutTableDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                               const QModelIndex&) const override;
};

// =============================================================================
// ShortcutMapper — keyboard shortcut configuration dialog
// Mirrors Win32 ShortcutMapper with 3 tabs: Commands / Scintilla / Macros.
// =============================================================================

class ShortcutMapper : public StaticDialog
{
    Q_OBJECT

public:
    explicit ShortcutMapper();
    ~ShortcutMapper() override = default;

    // Category: 0=Commands, 1=Scintilla, 2=Macros
    void switchMapperTab(int categoryIndex);
    void goTo(int categoryIndex);

    ShortcutKeyCombo getShortcutByCmdId(int cmdId) const;
    // Alias for plugin NPPM_GETSHORTCUTBYCMDID compatibility
    ShortcutKeyCombo getShortcutById(int cmdId) const { return getShortcutByCmdId(cmdId); }
    void updateShortcut(const ShortcutItem& item);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    QTabWidget* _tabWidget = nullptr;
    QTableWidget* _cmdTable = nullptr;
    QTableWidget* _sciTable = nullptr;
    QTableWidget* _macroTable = nullptr;

    ShortcutTableDelegate* _delegate = nullptr;

    int _currentCategory = 0;
    std::vector<ShortcutItem> _shortcuts;

    void initTables();
    void populateCommandTable();
    void populateScintillaTable();
    void populateMacroTable();

    void modifyShortcut(int row);
    void clearShortcut(int row);
    void saveShortcuts();
    void resetAllShortcuts();

    void filterByCategory(const QString& category);
    void filterByName(const QString& name);

    void goToSchwerCategory(int categoryIndex);
    QTableWidget* currentTable() const;
};

