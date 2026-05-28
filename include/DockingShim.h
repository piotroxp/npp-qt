// DockingShim.h - Qt6 docking window compatibility shim
#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QMenu>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QDockWidget>
#include <map>
#include <string>

// Docking manager states
enum class DockingState {
    Floating = 0,
    DockLeft = 1,
    DockRight = 2,
    DockTop = 3,
    DockBottom = 4,
    Hidden = 5
};

// Docking feature flags
enum DockingFeature {
    DockingFeatureEnabled = 0x01,
    DockingFeatureVisible = 0x02,
    DockingFeatureCloseable = 0x04,
    DockingFeatureFloatable = 0x08,
    DockingFeatureMovable = 0x10
};

// Simplified docking manager for Qt6
class DockingManager : public QObject {
    Q_OBJECT
public:
    explicit DockingManager(QMainWindow* mainWindow)
        : QObject(mainWindow), _mainWindow(mainWindow) {
        if (mainWindow) {
            mainWindow->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
            mainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
            mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
            mainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
        }
    }

    // Add a dockable widget
    QDockWidget* addDockWidget(const QString& title, QWidget* widget,
                           Qt::DockWidgetArea area = Qt::RightDockWidgetArea,
                           QWidget* prev = nullptr) {
        if (!_mainWindow || !widget) return nullptr;
        
        QDockWidget* dock = new QDockWidget(title, _mainWindow);
        dock->setWidget(widget);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setFloating(false);
        
        Qt::DockWidgetAreas areas;
        switch (area) {
            case Qt::LeftDockWidgetArea: areas = Qt::LeftDockWidgetArea; break;
            case Qt::RightDockWidgetArea: areas = Qt::RightDockWidgetArea; break;
            case Qt::TopDockWidgetArea: areas = Qt::TopDockWidgetArea; break;
            case Qt::BottomDockWidgetArea: areas = Qt::BottomDockWidgetArea; break;
            default: areas = Qt::AllDockWidgetArea; break;
        }
        _mainWindow->addDockWidget(dock, area);
        
        _docks[widget] = dock;
        return dock;
    }

    // Remove a dockable widget  
    void removeDockWidget(QWidget* widget) {
        auto it = _docks.find(widget);
        if (it != _docks.end()) {
            if (_mainWindow) _mainWindow->removeDockWidget(it->second);
            it->second->deleteLater();
            _docks.erase(it);
        }
    }

    // Toggle visibility
    void toggleDockWidget(QWidget* widget, bool show) {
        auto it = _docks.find(widget);
        if (it != _docks.end()) {
            if (show) it->second->show();
            else it->second->hide();
        }
    }

    // Set dock state
    void setDockState(QWidget* widget, DockingState state) {
        auto it = _docks.find(widget);
        if (it != _docks.end()) {
            switch (state) {
                case DockingState::Floating:
                    it->second->setFloating(true);
                    break;
                case DockingState::Hidden:
                    it->second->hide();
                    break;
                default:
                    it->second->setFloating(false);
                    break;
            }
        }
    }

    // Get dock state
    DockingState getDockState(QWidget* widget) const {
        auto it = _docks.find(const_cast<QWidget*>(widget));
        if (it != _docks.end()) {
            if (it->second->isFloating()) return DockingState::Floating;
            if (!it->second->isVisible()) return DockingState::Hidden;
            return DockingState::DockRight; // Simplified
        }
        return DockingState::Hidden;
    }

    // Window operation callbacks
    void setWindowOpCallback(void (*callback)(void*, int, QPoint&)) {
        _windowOpCallback = callback;
    }

    // Placeholder methods
    void apply() { /* Apply saved layout */ }
    void applyFromFile(const QString&) { /* Load layout from file */ }
    void saveToFile(const QString&) { /* Save layout to file */ }
    void getActiveDockingList(std::vector<QString>&) { /* Fill list */ }
    void getClosedDockList(std::vector<QString>&) { /* Fill list */ }

    // Toolbars
    void addToolbar(const QString& name, int id = -1) {
        if (!_mainWindow) return;
        QToolBar* tb = _mainWindow->addToolBar(name);
        _toolbars[name] = tb;
    }

    void addToolbarButton(int toolbarId, const QString& text, int cmdId,
                        const QString& iconPath = QString(), int keyComp = 0,
                        const QString& callback = QString()) {
        // Find toolbar by id - simplified
        for (auto& kv : _toolbars) {
            QAction* a = new QAction(text, kv.second);
            kv.second->addAction(a);
            _actions[kv.first].push_back(a);
            break; // Just add to first toolbar for now
        }
    }

    void refreshDarkMode() { /* Handle theme changes */ }

public:
    QMainWindow* _mainWindow;
    std::map<QWidget*, QDockWidget*> _docks;
    std::map<QString, QToolBar*> _toolbars;
    std::map<QString, std::vector<QAction*>> _actions;
    void (*_windowOpCallback)(void*, int, QPoint&) = nullptr;
};