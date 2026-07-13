#pragma once

#include <QMenu>
#include <QWidget>
#include <QString>

class ContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit ContextMenu();
    ~ContextMenu() override;

    void init(QWidget* parent);

    // Menu item management
    void addItem(const QString& text, int cmdId, bool isEnabled = true);
    void addSeparator();
    void addSubMenu(const QString& text, QMenu* subMenu);
    void removeItem(int cmdId);
    void enableItem(int cmdId, bool enable);
    void checkItem(int cmdId, bool check);
    void setItemText(int cmdId, const QString& text);

    // Show menu
    void trackPopupMenu();
    void trackPopupMenu(const QPoint& pt, QWidget* parent = nullptr);

    void destroy();
    void clear();

    QMenu& menu();

signals:
    void menuCommand(int cmdId);

private:
    QMenu _menu;
    QWidget* _parent = nullptr;
    void* _hMenu = nullptr;  // Not used in Qt
};

