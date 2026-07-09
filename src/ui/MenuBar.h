// MenuBar.h - Application menu bar
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QMenuBar>
#include <QString>

class QMenu;
class QAction;

class MenuBar : public QMenuBar {
    Q_OBJECT
public:
    explicit MenuBar(QWidget* parent = nullptr);
    ~MenuBar() override;

    QMenu* fileMenu() const { return _fileMenu; }
    QMenu* editMenu() const { return _editMenu; }
    QMenu* searchMenu() const { return _searchMenu; }
    QMenu* viewMenu() const { return _viewMenu; }
    QMenu* encodingMenu() const { return _encodingMenu; }
    QMenu* languageMenu() const { return _languageMenu; }
    QMenu* settingsMenu() const { return _settingsMenu; }
    QMenu* helpMenu() const { return _helpMenu; }

signals:
    void menuCommand(const QString& cmd);

private:
    void buildMenus();

    QMenu* _fileMenu = nullptr;
    QMenu* _editMenu = nullptr;
    QMenu* _searchMenu = nullptr;
    QMenu* _viewMenu = nullptr;
    QMenu* _encodingMenu = nullptr;
    QMenu* _languageMenu = nullptr;
    QMenu* _settingsMenu = nullptr;
    QMenu* _helpMenu = nullptr;
};
