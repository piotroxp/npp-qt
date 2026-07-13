#pragma once

#include <QWidget>
#include <QString>

// Grid-specific ShortcutMapper — handles the shortcut grid in preferences
// Note: The main ShortcutMapper is in WinControls/shortcut/
class GridShortcutMapper : public QWidget
{
    Q_OBJECT
public:
    explicit GridShortcutMapper(QWidget* parent = nullptr);
    ~GridShortcutMapper() override;

    void init(QWidget* parent);
    void addShortcut(const QString& commandName, int key, int modifiers);
    void removeShortcut(const QString& commandName);
    void clearAll();
    void save();

signals:
    void shortcutChanged(const QString& commandName, int key, int modifiers);

private:
    QWidget* _parent = nullptr;
};

