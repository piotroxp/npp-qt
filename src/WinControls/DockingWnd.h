// Semantic Lift: Win32 DockingWnd → Qt6 QDockingSystem
// Original: PowerEditor/src/WinControls/DockingWnd/
// Target: npp-qt/src/WinControls/DockingWnd.h + .cpp

#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QEnterEvent>
#include <QApplication>
#include <QToolTip>
#include <QWindow>
#include <QScreen>
#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainterPath>

// Forward declarations
class DockingContainer;
class DockingManager;

// =============================================================================
// Constants (lifted from Docking.h)
// =============================================================================

constexpr int HIT_TEST_THICKNESS = 20;
constexpr int SPLITTER_WIDTH = 4;

// Container positions
constexpr int CONT_LEFT = 0;
constexpr int CONT_RIGHT = 1;
constexpr int CONT_TOP = 2;
constexpr int CONT_BOTTOM = 3;
constexpr int DOCKCONT_MAX = 4;

// Docking widget flags
constexpr quint32 DWS_ICONTAB = 0x00000001;
constexpr quint32 DWS_ICONBAR = 0x00000002;
constexpr quint32 DWS_ADDINFO = 0x00000004;
constexpr quint32 DWS_USEOWNDARKMODE = 0x00000008;
constexpr quint32 DWS_PARAMSALL = (DWS_ICONTAB | DWS_ICONBAR | DWS_ADDINFO);

// Default docking positions
constexpr quint32 DWS_DF_CONT_LEFT = (CONT_LEFT << 28);
constexpr quint32 DWS_DF_CONT_RIGHT = (CONT_RIGHT << 28);
constexpr quint32 DWS_DF_CONT_TOP = (CONT_TOP << 28);
constexpr quint32 DWS_DF_CONT_BOTTOM = (CONT_BOTTOM << 28);
constexpr quint32 DWS_DF_FLOATING = 0x80000000;

// =============================================================================
// DockedWidgetData (lifted from Docking.h)
// =============================================================================

struct DockedWidgetData {
    QWidget* client = nullptr;           // hClient
    std::wstring pszName;               // was QString name
    int dlgID = 0;                       // dialog ID

    // User modifications
    quint32 uMask = 0;                   // uMask (Win32 field name)
    QIcon iconTab;                       // hIconTab
    QString addInfo;                      // pszAddInfo

    // Internal data
    QRect rcFloat;                       // floating position
    int iPrevCont = 0;                   // was int prevCont
    std::wstring pszModuleName;          // was QString moduleName
};

// =============================================================================
// DockMgr — dock region data (lifted from Docking.h)
// =============================================================================

struct DockMgr {
    QWidget* hWnd = nullptr;
    QRect rcRegion[DOCKCONT_MAX];
};

// =============================================================================
// DockingDlgInterface — base class for all docking dialog panels
// Mirrors Win32 DockingDlgInterface (IDD_DOCKING_DIALOG).
// Inherits from StaticDialog which is a QDialog base.
// =============================================================================

class DockingDlgInterface : public QWidget
{
    Q_OBJECT

public:
    explicit DockingDlgInterface(int panelID = 0);
    ~DockingDlgInterface() override = default;

    // Initialization (mirrors Win32 WM_INITDIALOG)
    virtual void init(QApplication* app, QWidget* parent, int panelID = 0);

    // Panel display (mirrors Win32 display())
    virtual void display(bool toShow = true);

    // Docking data access
    int getDockedTabHandle() const { return _panelID; }
    DockedWidgetData* getDockedData() { return &_dockedData; }
    std::vector<int> getIconIDs() const { return {}; }
    void setDockedData(const DockedWidgetData& data) { _dockedData = data; }

    // Dark mode color override
    virtual void setBackgroundColor(const QColor& col);
    virtual void setForegroundColor(const QColor& col);

    // Win32 message dispatcher (default implementation — override as needed)
    virtual intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam);

    // getHSelf() — returns this widget (mirrors Win32 getHSelf())
    QWidget* getHSelf() const { return const_cast<DockingDlgInterface*>(this); }

protected:
    int _panelID = 0;
    QString _dockID;
    QString _dockName;
    DockedWidgetData _dockedData;
    QWidget* _hParent = nullptr;
    bool _isVisible = false;
    bool _isFloating = false;

    // Dark mode
    bool _useDarkMode = false;
};

// =============================================================================
// Win32 Message Constants — defined centrally in NppConstants.h
// (WM_INITDIALOG, WM_COMMAND, WM_SIZE, WM_DESTROY, WM_CLOSE, WM_PAINT, etc.)
// Do not redefine here; include NppConstants.h instead.
// =============================================================================

// =============================================================================
// DockingContainer — a dockable panel with tabs and caption (lifted from DockingCont)
// =============================================================================

class DockingContainer : public QWidget
{
    Q_OBJECT

public:
    DockingContainer(QWidget* parent = nullptr);
    ~DockingContainer() override;

    DockedWidgetData* addDockedWidget(const DockedWidgetData& data);
    void removeDockedWidget(QWidget* client);
    DockedWidgetData* findDockedWidgetByWnd(QWidget* client);
    DockedWidgetData* findDockedWidgetByName(const QString& name);

    void showDockedWidget(DockedWidgetData* data, bool show);
    void setActiveTab(int index);
    int activeTabIndex() const;
    DockedWidgetData* activeWidgetData() const;
    QVector<DockedWidgetData*> allWidgetData() const;
    QVector<DockedWidgetData*> visibleWidgetData() const;
    bool isWidgetVisible(DockedWidgetData* data) const;
    bool hasWidgets() const { return !_widgetData.isEmpty(); }

    // Win32 API stubs
    QVector<DockedWidgetData*> getDataOfVisTb() const { return visibleWidgetData(); }
    QVector<DockedWidgetData*> getDataOfAllTb() const { return allWidgetData(); }
    bool isTbVis(DockedWidgetData* data) const { return isWidgetVisible(data); }
    DockedWidgetData* getActiveTb() const { return activeWidgetData(); }
    QRect getWindowRect() const { return geometry(); }  // Win32 compat: rc = window rect
    QWidget* getTabWnd() const { return _tabWidget; }
    QWidget* getCaptionWnd() const { return _captionBar; }

    void setCaptionTop(bool isTop);
    bool isCaptionTop() const { return _isTopCaption; }

    void setActive(bool active);
    bool isActive() const { return _isActive; }

    // setText(bool) — Qt6 equivalent of Win32 show/hide for docking containers
    void setText(bool shouldShow) { shouldShow ? QWidget::show() : QWidget::hide(); }
    void setText(const QString& /*title*/) { /* stub: caption set via DockedWidgetData */ }

    bool isFloating() const { return _isFloating; }
    void setFloating(bool floating);

    QSize minimumSizeHint() const override;

signals:
    void widgetShown(QWidget* client);
    void widgetHidden(QWidget* client);
    void tabChanged(int index);
    void closeRequested();
    void moveRequested();
    void dockToggleRequested();
    void floatAllRequested();

public slots:
    void closeAllTabs();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);

private:
    void updateCaption();
    void updateTabOrder();
    void relayout();
    int tabIndexOf(QWidget* client) const;
    void selectTab(int index);
    int hideDockedWidget(DockedWidgetData* data, bool hideClient = true);
    void viewDockedWidget(DockedWidgetData* data);
    QRect captionRect() const;
    QRect closeButtonRect() const;
    void drawCaption(QPainter& painter);
    void drawTabBar(QPainter& painter);

    // DPI-aware sizing
    int captionHeight() const;
    int captionGap() const;
    int closeButtonSize() const;

    enum class MouseRegion { Outside, Caption, CloseButton };

    MouseRegion hitTest(const QPoint& pos) const;
    void setHoverRegion(MouseRegion region);

    bool _isActive = false;
    bool _isFloating = false;
    bool _isTopCaption = true;

    QVector<DockedWidgetData*> _widgetData;
    QTabWidget* _tabWidget = nullptr;
    QLabel* _captionLabel = nullptr;
    QWidget* _captionBar = nullptr;
    QStackedWidget* _clientStack = nullptr;

    bool _isMouseDown = false;
    bool _isMouseOverClose = false;
    MouseRegion _hoverRegion = MouseRegion::Outside;

    QFont _captionFont;
    QFont _tabFont;
    QString _captionText;

    static constexpr int HIGH_CAPTION_DPI = 18;
    static constexpr int CAPTION_GAP_DPI = 2;
    static constexpr int CLOSEBTN_SIZE_DPI = 12;
    static constexpr int CLOSEBTN_POS_DPI = 3;
};

// =============================================================================
// DockingSplitter — resize handle between dock regions (lifted from DockingSplitter)
// =============================================================================

class DockingSplitter : public QWidget
{
    Q_OBJECT

public:
    enum class Orientation { Vertical, Horizontal };

    explicit DockingSplitter(Orientation orient, QWidget* parent = nullptr);
    ~DockingSplitter() override = default;

    void setRange(int min, int max);

signals:
    void splitterMoved(int newSize);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Orientation _orientation;
    bool _isDragging = false;
    QPoint _dragStartPos;
    int _rangeMin = 0;
    int _rangeMax = 200;
    int _currentPos = 100;
};

// =============================================================================
// DockingManager — manages all dock containers (lifted from DockingManager)
// =============================================================================

class DockingManager : public QWidget
{
    Q_OBJECT

public:
    DockingManager(QWidget* parent = nullptr);
    ~DockingManager() override;

    void init(QWidget* mainWindow);
    void setClientWindow(QWidget* client);

    void resizeEvent(QResizeEvent* event) override;

public slots:
    void showFloatingContainers(bool show);
    void createDockableDialog(DockedWidgetData data, int container = CONT_LEFT, bool visible = false);
    void showDockableDialog(QWidget* dlg, bool show);
    void showDockableDialogByName(const QString& name, bool show);
    void setActiveTab(int container, int index);
    void setDockedContainerSize(int container, int size);
    void setDockedContSize(int container, int size) { setDockedContainerSize(container, size); }
    int dockedContainerSize(int container) const;
    int getDockedContSize(int container) const { return dockedContainerSize(container); }
    void setStyleCaption(bool captionOnTop);

    // Win32 API stub (returns containers for save/restore)
    std::vector<DockingContainer*> getContainerInfo() const;

    // resize() stub — Qt6 uses QWidget::resize; this provides Win32 compat signature
    void resize() { /* triggers internal relayout via resizeEvent */ update(); }

signals:
    void dockInfoChanged();

private:
    void updateContainerInfo(QWidget* client);
    int containerIndexOf(QWidget* client) const;
    int findEmptyContainer() const;
    void relayout();
    QWidget* findDockedWidgetByName(const QString& name);

    QVector<DockingContainer*> _containers;
    QVector<DockingSplitter*> _splitters;
    QVector<QPair<int, QRect>> _dockRegions;
    DockMgr _dockData;
    QWidget* _mainWindow = nullptr;
    QVector<QWidget*> _clients;
};
