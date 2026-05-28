// ToolBar.h — Qt6 translation of CToolBar → QToolBar
#pragma once

#include <QToolBar>
#include <QMap>
#include <QVector>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QBoxLayout>
#include <memory>

class ToolBarButtonUnit
{
public:
    int _cmdID = 0;
    QIcon _defaultIcon;
    QIcon _hoverIcon;
    QIcon _disabledIcon;
    QString _tooltip;
};

enum class ToolBarStatusType { TB_SMALL, TB_LARGE, TB_SMALL2, TB_LARGE2, TB_STANDARD };

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar(QWidget* parent = nullptr);
    ~ToolBar() override;

    // Init the toolbar with button definitions
    void init(const QVector<ToolBarButtonUnit>& buttonArray);

    // Replace icon at index
    bool replaceIcon(size_t listIndex, size_t iconIndex, const QString& iconPath);

    // Enable/disable a button by command ID
    void enable(int cmdID, bool doEnable);

    // Check/uncheck a button
    bool getCheckState(int cmdID) const;
    void setCheck(int cmdID, bool willBeChecked);

    // State management
    ToolBarStatusType getState() const { return _state; }
    void setState(ToolBarStatusType state);

    // Size adjustments
    void reduce();
    void enlarge();
    void reduceToSet2();
    void enlargeToSet2();
    void setToBmpIcons();

    // Register dynamic buttons (plugins)
    void registerDynBtn(int message, const QPixmap& icon, const QIcon& absentIcon);
    void registerDynBtnDarkMode(int message, const QPixmap& icon, const QPixmap& iconDark);

    // Show popup for hidden buttons
    void doPopup(const QPoint& chevPoint);

    // Dark mode support
    void setDarkMode(bool enabled);
    bool isDarkMode() const { return _isDarkMode; }

    // DPI
    void resizeIconsDpi(int dpi);

signals:
    void buttonClicked(int cmdID);
    void toolbarIconsChanged();

private slots:
    void onActionTriggered();
    void onCustomContextMenuRequested(const QPoint& pos);

private:
    void rebuildActions(const QVector<ToolBarButtonUnit>& buttonArray);
    void applyIconSet();
    QIcon loadIconFromPath(const QString& path, int size = 16) const;

    QVector<ToolBarButtonUnit> _buttonArray;
    QMap<int, QAction*> _actionMap;   // cmdID → action
    QMap<int, QPixmap> _iconMap;      // cmdID → icon
    QMap<int, QPixmap> _disabledIconMap;
    QVector<int> _hiddenActions;       // actions shown in popup
    ToolBarStatusType _state = ToolBarStatusType::TB_SMALL;
    bool _isDarkMode = false;
    int _currentIconSize = 16;
};


// ReBar — toolbar container with draggable bands (rebar-like)
class ReBar : public QWidget
{
    Q_OBJECT

public:
    ReBar(QWidget* parent = nullptr);
    ~ReBar() override;

    void addWidget(QWidget* widget, int id = -1);
    void removeWidget(QWidget* widget);
    void setBandVisible(int id, bool visible);
    bool isBandVisible(int id) const;

signals:
    void bandMoved(int id, const QRect& rect);

private:
    QMap<int, QWidget*> _bandWidgets;
    QVBoxLayout* _layout = nullptr;
};