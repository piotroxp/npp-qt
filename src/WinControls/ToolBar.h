// Semantic Lift: Win32 Toolbar → Qt6 QToolBar
// Original: PowerEditor/src/WinControls/ToolBar/ToolBar.h
// Target: npp-qt/src/WinControls/ToolBar.h

#pragma once

#include <QToolBar>
#include <QAction>
#include <QVector>
#include <QMap>
#include <QIcon>
#include <QSize>
#include <QString>
#include <QDomDocument>

// Note: ToolBarStatusType (button state machine) is defined in NppConstants.h.
// This is the toolbar display style (small icons, large icons, etc.)
enum class ToolBarDisplayType { TB_SMALL, TB_LARGE, TB_SMALL2, TB_LARGE2, TB_STANDARD };
using ToolBarStatusType = ToolBarDisplayType;  // backward compat alias

// ToolBar-local types (Qt6 QString versions)
// If ImageListSet.h is included first, these types are already defined
#ifndef TOOLBAR_TYPES_DEFINED
#define TOOLBAR_TYPES_DEFINED

struct ToolBarButtonUnit {
    int _cmdID;
    QString _defText;
    QString _iconName;
    int _resourceIconID;
};

struct IconLocator {
    int _listIndex = 0;
    int _iconIndex = 0;
    QString _iconLocation;
    IconLocator(int listIdx, int iconIdx, const QString& iconLoc)
        : _listIndex(listIdx), _iconIndex(iconIdx), _iconLocation(iconLoc) {}
};

class ToolBarIcons {
public:
    ToolBarIcons() = default;
    void setDefaultList(const QVector<QIcon>& icons) { _defaultList = icons; }
    const QVector<QIcon>& getDefaultList() const { return _defaultList; }
    bool replaceIcon(int listIdx, int iconIdx, const QString& iconPath);
private:
    QVector<QIcon> _defaultList;
    QVector<QIcon> _disableList;
    QVector<QIcon> _defaultListSet2;
    QVector<QIcon> _disableListSet2;
    QVector<QIcon> _defaultListDM;
    QVector<QIcon> _disableListDM;
    QVector<QIcon> _defaultListSetDM2;
    QVector<QIcon> _disableListSetDM2;
};

struct DynamicCmdIcoBmp {
    int _message;
    QIcon _icon;
    QIcon _iconDM;
    QIcon _absentIcon;
};

#endif // TOOLBAR_TYPES_DEFINED

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar(QWidget* parent = nullptr);
    ~ToolBar() override;

    bool init(QApplication* app, QWidget* parent, ToolBarStatusType type,
              const ToolBarButtonUnit* buttonUnitArray, int arraySize);
    void destroy();
    void initTheme(QDomDocument* = nullptr) { /* Stubbed on Linux — no dark mode theme from .theme files */ }
    void initHideButtonsConf(QDomDocument* = nullptr, const void* = nullptr, size_t = 0) { /* Stubbed — no persisted hidden button state */ }

    void enable(int cmdID, bool doEnable) const;
    int getWidth() const;
    int getHeight() const;

    void reduce();
    void enlarge();
    void reduceToSet2();
    void enlargeToSet2();
    void setToBmpIcons();

    bool getCheckState(int ID2Check) const;
    void setCheck(int ID2Check, bool willBeChecked);
    ToolBarStatusType getState() const { return _state; }

    bool changeIcons(int whichLst, int iconIndex, const QString& iconLocation);

    void registerDynBtn(int message, const QIcon& icon, const QIcon& absentIcon);
    void registerDynBtnDM(int message, const QIcon& icon, const QIcon& iconDM, const QIcon& absentIcon);

    void doPopup(const QPoint& chevPoint);
    void resizeIconsDpi(int dpi);

signals:
    void buttonClicked(int cmdID);
    void toolBarStyleChanged(ToolBarStatusType newState);

private slots:
    void onActionTriggered();

private:
    void setState(ToolBarStatusType state);
    QAction* findAction(int cmdID) const;

    QVector<QAction*> _actions;
    QVector<ToolBarButtonUnit> _buttonUnits;
    ToolBarIcons _toolBarIcons;
    ToolBarStatusType _state = ToolBarStatusType::TB_SMALL;
    QVector<DynamicCmdIcoBmp> _vDynBtnReg;
    QVector<IconLocator> _customIconVect;
    int _nbButtons = 0;
    int _nbDynButtons = 0;
    int _currentDpi = 96;
    QSize _iconSizeSmall;
    QSize _iconSizeLarge;

    friend class ReBar;
};

struct ReBarBandInfo {
    int _id = 0;
    QString _text;
    QWidget* _widget = nullptr;
    int _width = 0;
    int _height = 0;
    bool _visible = true;
    bool _newLine = true;
    QColor _backgroundColor;
};

class ReBar : public QWidget
{
    Q_OBJECT

public:
    ReBar(QWidget* parent = nullptr);
    ~ReBar() override;
    void init(QApplication* app, QWidget* parent);
    void destroy();
    bool addBand(ReBarBandInfo* bandInfo, bool useID);
    void reNew(int id, ReBarBandInfo* bandInfo);
    void removeBand(int id);
    void setIDVisible(int id, bool show);
    bool getIDVisible(int id) const;
    void setGrayBackground(int id);
    void display(bool toShow = true);

signals:
    void bandChanged(int id);

private:
    int getNewID();
    void releaseID(int id);
    QVector<int> _usedIDs;
    QMap<int, QAction*> _bandActions;
};
