// ControlsTab.h - Qt6 port of ControlsTab
#pragma once
#include <QTabWidget>
#include <QVector>
#include <QPair>
#include <QResizeEvent>

// Controls tab (tab bar for settings/preferences panels)
class ControlsTab : public QTabWidget
{
    Q_OBJECT

public:
    ControlsTab(QWidget* parent = nullptr);
    ~ControlsTab() override;

    void createTabs(const QVector<QPair<QString, QWidget*>>& tabs);

    void clickedUpdate()
    {
        activateWindowAt(currentIndex());
    }

    void renameTab(size_t index, const QString& newName);
    bool renameTab(const QString& internalName, const QString& newName);

signals:
    void tabChanged(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void activateWindowAt(int index);

    QVector<QPair<QString, QWidget*>> _tabs;
    int _current = 0;
};