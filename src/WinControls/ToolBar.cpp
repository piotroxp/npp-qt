// Semantic Lift: Win32 Toolbar → Qt6 QToolBar Implementation
// Original: PowerEditor/src/WinControls/ToolBar/ToolBar.cpp
// Target: npp-qt/src/WinControls/ToolBar.cpp

#include "ToolBar.h"
#include <QApplication>
#include <QMenu>

ToolBar::ToolBar(QWidget* parent) : QToolBar(parent)
{
    setIconSize(QSize(16, 16));
    setMovable(true);
    setFloatable(true);
}

ToolBar::~ToolBar() = default;

bool ToolBar::init(QApplication* app, QWidget* parent, ToolBarStatusType type,
                    const ToolBarButtonUnit* buttonUnitArray, int arraySize)
{
    setParent(parent);
    for (int i = 0; i < arraySize; ++i) _buttonUnits.append(buttonUnitArray[i]);
    for (const auto& unit : _buttonUnits) {
        QAction* action = new QAction(this);
        action->setData(unit._cmdID);
        action->setText(unit._defText);
        connect(action, &QAction::triggered, this, &ToolBar::onActionTriggered);
        addAction(action);
        _actions.append(action);
    }
    setState(type);
    return true;
}

void ToolBar::destroy() { clear(); _actions.clear(); _buttonUnits.clear(); }

void ToolBar::enable(int cmdID, bool doEnable) const
{
    if (QAction* action = findAction(cmdID)) action->setEnabled(doEnable);
}

int ToolBar::getWidth() const { return sizeHint().width(); }
int ToolBar::getHeight() const { return sizeHint().height(); }

void ToolBar::reduce() { setState(ToolBarStatusType::TB_SMALL); }
void ToolBar::enlarge() { setState(ToolBarStatusType::TB_LARGE); }
void ToolBar::reduceToSet2() { setState(ToolBarStatusType::TB_SMALL2); }
void ToolBar::enlargeToSet2() { setState(ToolBarStatusType::TB_LARGE2); }
void ToolBar::setToBmpIcons() { setState(ToolBarStatusType::TB_STANDARD); }

bool ToolBar::getCheckState(int ID2Check) const
{
    if (QAction* action = findAction(ID2Check)) return action->isChecked();
    return false;
}

void ToolBar::setCheck(int ID2Check, bool willBeChecked)
{
    if (QAction* action = findAction(ID2Check)) {
        action->setCheckable(true);
        action->setChecked(willBeChecked);
    }
}

void ToolBar::setState(ToolBarStatusType state)
{
    _state = state;
    QSize iconSize;
    switch (state) {
        case ToolBarStatusType::TB_SMALL:
        case ToolBarStatusType::TB_SMALL2: iconSize = QSize(16, 16); break;
        case ToolBarStatusType::TB_LARGE:
        case ToolBarStatusType::TB_LARGE2: iconSize = QSize(32, 32); break;
        case ToolBarStatusType::TB_STANDARD: iconSize = QSize(24, 24); break;
    }
    setIconSize(iconSize);
    emit toolBarStyleChanged(state);
}

bool ToolBar::changeIcons(int whichLst, int iconIndex, const QString& iconLocation)
{
    QIcon icon(iconLocation);
    return !icon.isNull();
}

void ToolBar::registerDynBtn(int message, const QIcon& icon, const QIcon& absentIcon)
{
    DynamicCmdIcoBmp dynBtn;
    dynBtn._message = message;
    dynBtn._icon = icon;
    dynBtn._absentIcon = absentIcon;
    _vDynBtnReg.append(dynBtn);
}

void ToolBar::registerDynBtnDM(int message, const QIcon& icon, const QIcon& iconDM, const QIcon& absentIcon)
{
    DynamicCmdIcoBmp dynBtn;
    dynBtn._message = message;
    dynBtn._icon = icon;
    dynBtn._iconDM = iconDM;
    dynBtn._absentIcon = absentIcon;
    _vDynBtnReg.append(dynBtn);
}

void ToolBar::doPopup(const QPoint& chevPoint)
{
    QMenu popup(this);
    for (QAction* action : _actions) {
        if (!action->isVisible()) popup.addAction(action);
    }
    popup.exec(chevPoint);
}

void ToolBar::resizeIconsDpi(int dpi)
{
    _currentDpi = dpi;
    qreal scaleFactor = dpi / 96.0;
    QSize baseSize = (_state == ToolBarStatusType::TB_LARGE) ? QSize(32, 32) : QSize(16, 16);
    setIconSize(QSize(baseSize.width() * scaleFactor, baseSize.height() * scaleFactor));
}

void ToolBar::onActionTriggered()
{
    if (QAction* action = qobject_cast<QAction*>(sender()))
        emit buttonClicked(action->data().toInt());
}

QAction* ToolBar::findAction(int cmdID) const
{
    for (QAction* action : _actions) {
        if (action->data().toInt() == cmdID) return action;
    }
    return nullptr;
}

ReBar::ReBar(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

ReBar::~ReBar() = default;

void ReBar::init(QApplication* app, QWidget* parent) { Q_UNUSED(app); Q_UNUSED(parent); }

void ReBar::destroy()
{
    for (auto* action : _bandActions.values()) delete action;
    _bandActions.clear();
    _usedIDs.clear();
}

bool ReBar::addBand(ReBarBandInfo* bandInfo, bool useID)
{
    int id = useID ? bandInfo->_id : getNewID();
    if (id == -1) return false;
    bandInfo->_id = id;
    ToolBar* toolbar = new ToolBar(this);
    toolbar->setTitle(bandInfo->_text);
    layout()->addWidget(toolbar);
    _bandActions[id] = new QAction(this);
    _bandActions[id]->setVisible(bandInfo->_visible);
    return true;
}

void ReBar::reNew(int id, ReBarBandInfo* bandInfo)
{
    if (_bandActions.contains(id)) {
        _bandActions[id]->setText(bandInfo->_text);
        _bandActions[id]->setVisible(bandInfo->_visible);
    }
}

void ReBar::removeBand(int id)
{
    if (_bandActions.contains(id)) {
        delete _bandActions[id];
        _bandActions.remove(id);
        releaseID(id);
    }
}

void ReBar::setIDVisible(int id, bool show)
{
    if (_bandActions.contains(id)) _bandActions[id]->setVisible(show);
}

bool ReBar::getIDVisible(int id) const
{
    return _bandActions.contains(id) && _bandActions[id]->isVisible();
}

void ReBar::setGrayBackground(int id) { Q_UNUSED(id); }

int ReBar::getNewID()
{
    for (int i = 0; i < 1000; ++i) {
        if (!_usedIDs.contains(i)) { _usedIDs.append(i); return i; }
    }
    return -1;
}

void ReBar::releaseID(int id) { _usedIDs.removeOne(id); }

bool ToolBarIcons::replaceIcon(int listIdx, int iconIdx, const QString& iconPath)
{
    QIcon icon(iconPath);
    if (icon.isNull()) return false;
    if (listIdx == 0 && iconIdx < _defaultList.size()) {
        _defaultList[iconIdx] = icon;
        return true;
    }
    return false;
}


