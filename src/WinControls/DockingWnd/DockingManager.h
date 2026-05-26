// DockingManager.h — Qt6 translation
#pragma once

#include <QDockWidget>
#include <QMap>

class DockedWidgetData;
class QMainWindow;

class DockingManager : public QObject
{
    Q_OBJECT

public:
    DockingManager(QMainWindow* mainWindow, QObject* parent = nullptr);
    ~DockingManager() override;

    // Init
    void init();

    // Create dockable dialog
    void createDockableDlg(const DockedWidgetData& data, int container = 0, bool isVisible = false);

    // Show/hide panels
    void showDockableDlg(QWidget* client, bool show);
    void showDockableDlg(const QString& name, bool show);

    // Active tab
    void setActiveTab(int container, int tabIndex);

    // Floating containers
    void showFloatingContainers(bool show);

    // Style
    void setStyleCaption(bool captionOnTop);

    // Size
    int getDockedContSize(int container) const;
    void setDockedContSize(int container, int size);

    // Save/restore layout
    QByteArray saveLayout() const;
    bool restoreLayout(const QByteArray& state);

    // Container access
    QVector<QDockWidget*> getContainers() const { return _containers; }

signals:
    void layoutChanged();

private:
    QMainWindow* _mainWindow = nullptr;
    QVector<QDockWidget*> _containers; // 4 main containers
    QVector<QDockWidget*> _floatingContainers;
    QMap<QWidget*, DockedWidgetData> _widgetData;
};

// DockingSplitter — resizable splitter for panels
class DockingSplitter : public QWidget
{
    Q_OBJECT

public:
    enum Direction { Horizontal, Vertical };

    explicit DockingSplitter(QWidget* parent = nullptr);
    void init(Direction dir);

signals:
    void splitterMoved(int offset);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Direction _direction = Horizontal;
    bool _isDragging = false;
    QPoint _dragStart;
    int _startOffset = 0;
};
