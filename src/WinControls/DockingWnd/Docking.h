// Docking.h — Qt6 translation of docking structures
#pragma once

#include <QDockWidget>
#include <QMainWindow>

// ─── Docking constants ────────────────────────────────────────

#define CONT_LEFT      0
#define CONT_RIGHT     1
#define CONT_TOP      2
#define CONT_BOTTOM    3
#define DOCKCONT_MAX   4

// Docking mask flags
#define DWS_ICONTAB          0x00000001
#define DWS_ICONBAR         0x00000002
#define DWS_ADDINFO         0x00000004
#define DWS_USEOWNDARKMODE   0x00000008
#define DWS_PARAMSALL       (DWS_ICONTAB | DWS_ICONBAR | DWS_ADDINFO)

// Default docking positions
#define DWS_DF_CONT_LEFT    (CONT_LEFT << 28)
#define DWS_DF_CONT_RIGHT   (CONT_RIGHT << 28)
#define DWS_DF_CONT_TOP     (CONT_TOP << 28)
#define DWS_DF_CONT_BOTTOM  (CONT_BOTTOM << 28)
#define DWS_DF_FLOATING     0x80000000

#define HIT_TEST_THICKNESS   20
#define SPLITTER_WIDTH      4

// ─── DockedWidgetData ────────────────────────────────────────

struct DockedWidgetData
{
    QWidget* client = nullptr;           // hClient
    QString name;                       // pszName
    int dlgID = 0;                     // dialog ID

    unsigned mask = 0;                // DWS_* flags
    QIcon iconTab;                    // hIconTab
    QString addInfo;                   // pszAddInfo

    // Internal
    QRect floatRect;                  // rcFloat
    int prevCont = 0;                 // iPrevCont
    QString moduleName;               // pszModuleName
};

// ─── DockMgr ────────────────────────────────────────────────

struct DockMgr
{
    QMainWindow* mainWindow = nullptr;
    QRect region[DOCKCONT_MAX];
};

// ─── Dock container direction ────────────────────────────────

enum class CaptionSide { Top, Bottom };

// ─── DockingPanel ──────────────────────────────────────────

class DockingPanel : public QDockWidget
{
    Q_OBJECT

public:
    DockingPanel(const QString& title, QWidget* container, QMainWindow* parent = nullptr);

    void setCaptionTop(bool onTop);
    void setActive(bool active);

    QWidget* getClient() const { return _client; }
    QWidget* getTabBar() const { return _tabBar; }

signals:
    void closeRequested();
    void floatRequested();
    void dockRequested();
    void moved(const QPoint& globalPos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QWidget* _client = nullptr;
    QWidget* _tabBar = nullptr;
    bool _isFloating = false;
    bool _isActive = false;
    CaptionSide _captionSide = CaptionSide::Top;
};

// ─── DockingContainer ──────────────────────────────────────

class DockingContainer : public QWidget
{
    Q_OBJECT

public:
    DockingContainer(QMainWindow* mainWindow, CaptionSide side = CaptionSide::Top,
                   QWidget* parent = nullptr);
    ~DockingContainer() override;

    void addPanel(QDockWidget* panel);
    void removePanel(QDockWidget* panel);
    void showPanel(QDockWidget* panel, bool show);

    bool isFloating() const { return _isFloating; }
    void setFloating(bool floating);
    void setCaptionTop(bool onTop);

    QWidget* getTabBar() const { return _tabBar; }

signals:
    void panelCloseRequested(QDockWidget* panel);
    void panelFloatRequested(QDockWidget* panel);

private:
    QMainWindow* _mainWindow = nullptr;
    QDockWidget* _activePanel = nullptr;
    QWidget* _tabBar = nullptr;
    bool _isFloating = false;
    CaptionSide _captionSide = CaptionSide::Top;
};

// ─── DockingManager ────────────────────────────────────────

class DockingManager : public QWidget
{
    Q_OBJECT

public:
    DockingManager(QMainWindow* mainWindow, QWidget* parent = nullptr);
    ~DockingManager() override;

    void init();

    // Create/manage dockable panels
    void createDockablePanel(DockedWidgetData& data, int container = CONT_LEFT);
    void showDockablePanel(QWidget* client, bool show);
    void setActiveTab(int container, int tabIndex);

    // Layout
    void resizeEvent(QResizeEvent* event) override;
    void saveLayout(QByteArray& state) const;
    bool restoreLayout(const QByteArray& state);

    // Container access
    QDockWidgetArea getDockArea(int container) const;

signals:
    void layoutChanged();

private:
    QMainWindow* _mainWindow = nullptr;
    QVector<QWidget*> _containers; // CONT_LEFT, CONT_RIGHT, CONT_TOP, CONT_BOTTOM, CONT_FLOATING...
    DockMgr _dockData;
};
