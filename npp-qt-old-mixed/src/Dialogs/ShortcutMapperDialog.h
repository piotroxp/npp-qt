// ShortcutMapperDialog.h — npp-qt Qt6 Shortcut Mapper dialog
// Replaces stub in NppCommands::settingsShortcutMapper()

#pragma once

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QKeySequenceEdit>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QVector>
#include <QString>

// =============================================================================
// ShortcutEntry — data for one keyboard shortcut row
// =============================================================================

struct ShortcutEntry {
    int commandId = 0;          // Menu command ID
    QString category;           // Menu category (File, Edit, Search, etc.)
    QString commandName;        // Display name
    QString shortcut1;          // Primary shortcut (QKeySequence string)
    QString shortcut2;          // Secondary shortcut
    bool isModified = false;    // Whether this entry has been changed
};

// =============================================================================
// ShortcutMapperDialog — keyboard shortcut configuration dialog
// =============================================================================

class ShortcutMapperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutMapperDialog(QWidget* parent = nullptr);
    ~ShortcutMapperDialog() override = default;

private slots:
    void onCategoryFilterChanged(int index);
    void onSearchTextChanged(const QString& text);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onClearShortcut();
    void onResetAll();
    void onApply();
    void onOk();
    void onCancel();

private:
    void setupUi();
    void populateShortcuts();
    void populateCommandTree();
    void applyShortcut(int row, const QString& keySeq);
    void loadShortcuts();
    void saveShortcuts();

    // Build the commands list from menu command IDs
    void buildCommands();

    // Controls
    QComboBox* _categoryFilter = nullptr;
    QLineEdit* _searchEdit = nullptr;
    QTreeWidget* _shortcutTree = nullptr;
    QPushButton* _btnClear = nullptr;
    QPushButton* _btnResetAll = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;

    // All shortcut entries
    QVector<ShortcutEntry> _entries;

    // Proxy model for search filtering
    QSortFilterProxyModel* _proxyModel = nullptr;
};
