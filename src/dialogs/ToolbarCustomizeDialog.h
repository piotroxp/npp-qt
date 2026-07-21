// ToolbarCustomizeDialog.h — toolbar customize UI (Wave 14 task #2).
//
// Lets the user add, remove, and reorder the actions on the main toolbar.
// The dialog reads the toolbar's current action list and the full pool of
// available commands (from the toolbar's m_actions map, which ToolBar
// populates in setupActions()) and lets the user choose which to show.
//
// On accept, the dialog applies the changes by calling removeAction /
// addAction on the toolbar in the desired order. The toolbar emits
// toolBarCommand when its actions are clicked, so all the existing
// dispatch wiring continues to work.

#pragma once

#include <QDialog>
#include <QString>

class QListWidget;
class QListWidgetItem;
class QPushButton;
class ToolBar;

class ToolbarCustomizeDialog : public QDialog {
    Q_OBJECT
public:
    explicit ToolbarCustomizeDialog(ToolBar* toolbar, QWidget* parent = nullptr);
    ~ToolbarCustomizeDialog() override;

    // Apply the user's selections to the toolbar. Called automatically on
    // accept(), but exposed publicly so a future "Apply" button can drive
    // it without closing the dialog.
    void applyToToolbar();

private Q_SLOTS:
    void onAdd();
    void onRemove();
    void onMoveUp();
    void onMoveDown();
    void onReset();
    void onSelectionChanged();

private:
    void rebuildVisibleList();
    void rebuildAvailableList();
    QString actionDisplayName(const QString& command) const;
    QPushButton* makeButton(const QString& label);

    ToolBar* m_toolbar = nullptr;

    QListWidget* m_available = nullptr;
    QListWidget* m_visible = nullptr;

    QPushButton* m_addBtn = nullptr;
    QPushButton* m_removeBtn = nullptr;
    QPushButton* m_upBtn = nullptr;
    QPushButton* m_downBtn = nullptr;
    QPushButton* m_resetBtn = nullptr;

    // Cached default order — used by Reset to put the toolbar back to
    // its initial state (the order in which ToolBar::setupActions created
    // the actions).
    QStringList m_defaultVisible;
};