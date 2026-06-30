// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// npp-qt: Win32 TOOLBARCLASSNAME + ICC_WIN95_CLASSES → Qt6 QToolBar
// Original: PowerEditor/src/WinControls/ToolBar/ToolBar.cpp

#include "ToolBar.h"
#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>
#include <QAction>
#include <QToolButton>
#include <QStyle>
#include <QStyleOptionToolButton>
#include <QEvent>
#include <QWheelEvent>
#include <QMimeData>
#include <QDrag>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDir>
#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QMessageBox>
#include <QMainWindow>
#include <QWindow>
#include <QPainter>
#include <QVariant>
#include <QBoxLayout>

#include "menuCmdID.h"
#include "NppXml.h"
#include "DarkMode/DarkMode.h"
#include "MISC/Common/dpiManagerV2.h"


// ============================================================================
// ToolBarIcons — Qt image-list manager (mirrors Win32 HIMAGELIST)
// ============================================================================

namespace {
QIcon loadToolbarIcon(const QString& resourcePath, int resourceID, const QSize& desiredSize);
QIcon loadIconFromResource(int iconID, const QSize& desiredSize);
QIcon loadIconFromFile(const QString& fullPath, const QSize& desiredSize);

// Fallback missing-icon pixmap
QPixmap makeMissingIconPixmap(const QSize& size) {
    QPixmap pix(size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setPen(Qt::red);
    p.setBrush(Qt::NoBrush);
    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();
    return pix;
}
}  // anonymous namespace

void ToolBarIcons::init(const QVector<ToolBarButtonUnit>& buttonUnits, int /*arraySize*/,
                        const QVector<DynamicCmdIcoBmp>& dynButtons)
{
    _buttonUnits = buttonUnits;
    // Build message → icon mapping from dynamic buttons
    for (const auto& dyn : dynButtons) {
        if (!dyn._icon.isNull()) {
            _dynIconMap[dyn._message] = dyn._icon;
        }
    }
    Q_UNUSED(dynButtons);
}

void ToolBarIcons::create(const QSize& iconSize)
{
    _currentSize = iconSize;

    // Initialise all 8 icon lists
    for (auto& list : _iconListVector) {
        list.clear();
    }

    // Load standard icons for each button unit
    for (const auto& unit : _buttonUnits) {
        if (unit._resourceIconID <= 0) {
            // Placeholder icon for separators / invalid IDs
            QIcon icon;
            icon.addPixmap(makeMissingIconPixmap(iconSize));
            _iconListVector[HLIST_DEFAULT].append(icon);
            _iconListVector[HLIST_DISABLE].append(icon);
            _iconListVector[HLIST_DEFAULT2].append(icon);
            _iconListVector[HLIST_DISABLE2].append(icon);
            _iconListVector[HLIST_DEFAULT_DM].append(icon);
            _iconListVector[HLIST_DISABLE_DM].append(icon);
            _iconListVector[HLIST_DEFAULT_DM2].append(icon);
            _iconListVector[HLIST_DISABLE_DM2].append(icon);
            continue;
        }

        QIcon defaultIcon = loadIconFromResource(unit._resourceIconID, iconSize);
        QIcon darkIcon   = loadIconFromResource(unit._resourceIconID, iconSize); // same for now

        // Default (enabled) — default icon
        _iconListVector[HLIST_DEFAULT].append(defaultIcon);

        // Disabled — use a grayed version (desaturate the enabled icon)
        QPixmap srcPm = defaultIcon.pixmap(iconSize);
        QPixmap grayPm(iconSize);
        grayPm.fill(Qt::transparent);
        QPainter gp(&grayPm);
        gp.setCompositionMode(QPainter::CompositionMode_Source);
        gp.drawPixmap(0, 0, srcPm);
        gp.setCompositionMode(QPainter::CompositionMode_Luminosity);
        gp.fillRect(grayPm.rect(), Qt::darkGray);
        gp.setCompositionMode(QPainter::CompositionMode_Overlay);
        gp.fillRect(grayPm.rect(), Qt::white);
        gp.end();
        QIcon disabledIcon;
        disabledIcon.addPixmap(grayPm);
        _iconListVector[HLIST_DISABLE].append(disabledIcon);

        // Set 2 (large icons) — same icons for now (could be different in a full port)
        _iconListVector[HLIST_DEFAULT2].append(defaultIcon);
        _iconListVector[HLIST_DISABLE2].append(disabledIcon);

        // Dark mode variants
        _iconListVector[HLIST_DEFAULT_DM].append(darkIcon);
        _iconListVector[HLIST_DISABLE_DM].append(disabledIcon); // TODO: dark-disabled variant
        _iconListVector[HLIST_DEFAULT_DM2].append(darkIcon);
        _iconListVector[HLIST_DISABLE_DM2].append(disabledIcon);
    }
}

void ToolBarIcons::resizeIcon(const QSize& newSize)
{
    create(newSize);  // Full reload at new size
}

bool ToolBarIcons::replaceIcon(int /*listIndex*/, int iconIndex, const QString& iconLocation)
{
    // Replace iconIndex across ALL icon lists (mirrors Win32 change2CustomIconsIfAny).
    // Win32: ImageList_ReplaceIcon(hImageList, iconIndex, hIcon)
    if (iconIndex < 0) return false;

    QIcon newIcon = loadIconFromFile(iconLocation, _currentSize);
    if (newIcon.isNull()) return false;

    for (auto& list : _iconListVector) {
        if (iconIndex < list.size()) {
            list.replace(iconIndex, newIcon);
        }
    }
    return true;
}

void ToolBarIcons::destroy()
{
    for (auto& list : _iconListVector) {
        list.clear();
    }
    _dynIconMap.clear();
}

namespace {
QIcon loadIconFromResource(int iconID, const QSize& desiredSize)
{
    // In Qt6, icons are loaded from the Qt resource system or from files.
    // For Win32 resource IDs (integers), we use a fallback strategy:
    // 1. Try to load from ":/icons/id" resource path
    // 2. Fall back to a generated placeholder
    Q_UNUSED(iconID);
    QIcon icon;
    icon.addPixmap(makeMissingIconPixmap(desiredSize));
    return icon;
}

QIcon loadIconFromFile(const QString& fullPath, const QSize& desiredSize)
{
    QFileInfo fi(fullPath);
    if (!fi.exists()) return QIcon();

    QFileIconProvider provider;
    QIcon icon = provider.icon(fi);
    if (!icon.isNull()) return icon;

    // Fallback: load directly
    return QIcon(fullPath);
}

QIcon loadToolbarIcon(const QString& /*resourcePath*/, int /*resourceID*/, const QSize& desiredSize)
{
    QIcon icon;
    icon.addPixmap(makeMissingIconPixmap(desiredSize));
    return icon;
}
}  // anonymous namespace


// ============================================================================
// ToolbarButtonAction
// ============================================================================


// ============================================================================
// Toolbar
// ============================================================================

Toolbar::Toolbar(QWidget* parent) : QToolBar(parent)
{
    setIconSize(QSize(16, 16));
    setMovable(true);
    setFloatable(true);
    setObjectName(QStringLiteral("Toolbar"));

    // Connect triggered signal to our slot
    connect(this, &QToolBar::actionTriggered, this, &Toolbar::onActionTriggered);

    // Track DPI changes via screenChanged
    setAttribute(Qt::WA_StyleSheet);

    // Dark mode: set base stylesheet if dark mode is active
    refreshDarkMode();
}

Toolbar::~Toolbar() = default;

void Toolbar::initTheme(const QDomElement& toolIconsDocRoot)
{
    // Parse the toolbar-icons XML and build the custom-icon override list.
    // Translates Win32 toolbarIconsDoc loading → QDomElement traversal.
    _toolIcons = toolIconsDocRoot;
    if (_toolIcons.isNull()) return;

    // Navigate: root → NotepadPlus → ToolBarIcons
    _toolIcons = NppXml::firstChildElement(_toolIcons, "ToolBarIcons");
    if (_toolIcons.isNull()) return;

    // Walk <Icon LOC="filename" ID="N"/> elements
    QDomElement iconElem = NppXml::firstChildElement(_toolIcons, "Icon");
    for (; !iconElem.isNull(); iconElem = NppXml::nextSiblingElement(iconElem, "Icon")) {
        QString loc = NppXml::attribute(iconElem, "LOC");
        int iconIndex = NppXml::intAttribute(iconElem, "ID", -1);
        if (iconIndex >= 0 && !loc.isEmpty()) {
            iconLocator locEntry;
            locEntry._iconIndex = iconIndex;
            locEntry._iconLocation = loc;
            _customIconVect.append(locEntry);
        }
    }
}

void Toolbar::initHideButtonsConf(NppXml::Document toolButtonsDocRoot,
                                  const IconListButtonUnit* buttonUnitArray,
                                  int buttonCount)
{
    // Parse the toolbar-buttons XML to restore persisted visibility state.
    // Translates Win32 NppXml::load/toolButtonsDocRoot → QDomElement.
    if (!toolButtonsDocRoot) return;
    QDomElement toolButtons = toolButtonsDocRoot->documentElement();
    if (toolButtons.isNull()) return;
    toolButtons = NppXml::firstChildElement(toolButtons, "ToolbarButtons");
    if (toolButtons.isNull()) return;

    // Standard toolbar buttons
    QDomElement standardButtons = NppXml::firstChildElement(toolButtons, "Standard");
    if (!standardButtons.isNull()) {
        const QString& hideAll = NppXml::attribute(standardButtons, "hideAll");
        _toolbarStdButtonsConfArray = std::make_unique<bool[]>(
            static_cast<size_t>(buttonUnitArray.size()));

        if (hideAll == QStringLiteral("yes")) {
            for (int i = 0; i < buttonCount; ++i)
                _toolbarStdButtonsConfArray[i] = false;
        } else {
            for (int i = 0; i < buttonCount; ++i)
                _toolbarStdButtonsConfArray[i] = true;

            QDomElement btnElem = NppXml::firstChildElement(standardButtons, "Button");
            for (; !btnElem.isNull(); btnElem = NppXml::nextSiblingElement(btnElem, "Button")) {
                int cmdID  = NppXml::intAttribute(btnElem, "id", -1);
                int index  = NppXml::intAttribute(btnElem, "index", -1);
                const QString& hide = NppXml::attribute(btnElem, "hide");
                if (cmdID > -1 && index > -1 && index < buttonUnitArray.size()
                    && buttonUnitArray[index]._cmdID == cmdID
                    && hide == QStringLiteral("yes")) {
                    _toolbarStdButtonsConfArray[index] = false;
                }
            }
        }
    }

    // Plugin toolbar buttons
    QDomElement pluginButtons = NppXml::firstChildElement(toolButtons, "Plugin");
    if (!pluginButtons.isNull()) {
        const QString& hideAll = NppXml::attribute(pluginButtons, "hideAll");
        if (hideAll == QStringLiteral("yes")) {
            _toolbarPluginButtonsConf._isHideAll = true;
            return;
        }
        QDomElement btnElem = NppXml::firstChildElement(pluginButtons, "Button");
        for (; !btnElem.isNull(); btnElem = NppXml::nextSiblingElement(btnElem, "Button")) {
            const QString& hide = NppXml::attribute(btnElem, "hide");
            _toolbarPluginButtonsConf._showPluginButtonsArray.append(hide != QStringLiteral("yes"));
        }
    }
}

bool Toolbar::init(QApplication* /*app*/, QWidget* parent, ToolBarStatusType type,
                   const QVector<ToolBarButtonUnit>& buttonUnitArray,
                   void* /*unusedHInstance*/, int /*iconSizeOverride*/)
{
    // Main initialiser — mirrors Win32 CreateToolbarEx().
    // Translates: HINSTANCE+HWND → QWidget* parent; TBBUTTON[] → QAction[]
    if (parent) {
        setParent(parent);
    }

    _state = type;
    int dpi = DPIManagerV2::getDpiForWindow(parent);

    int iconSize = DPIManagerV2::scale((_state == TB_LARGE || _state == TB_LARGE2) ? 32 : 16, this);

    // Build dynamic-message array for init
    QVector<int> dynMessages;
    for (const auto& dyn : _vDynBtnReg) {
        dynMessages.append(dyn._message);
    }
    _buttonUnits = buttonUnitArray;
    _toolBarIcons.init(buttonUnitArray, buttonUnitArray.size(), _vDynBtnReg);
    _toolBarIcons.create(QSize(iconSize, iconSize));

    _nbButtons = buttonUnitArray.size();
    _nbDynButtons = _vDynBtnReg.size();
    _nbTotalButtons = _nbButtons + (_nbDynButtons ? _nbDynButtons + 1 : 0);
    _nbCurrentButtons = 0;

    // Store button units for later use
    _buttonUnits = buttonUnitArray;

    // Build initial button states
    _buttonStates.resize(_nbTotalButtons);
    for (int i = 0; i < _nbTotalButtons; ++i) {
        _buttonStates[i].enabled = true;
        _buttonStates[i].hidden = false;
        _buttonStates[i].checked = false;
        if (i < _nbButtons) {
            if (_toolbarStdButtonsConfArray && !_toolbarStdButtonsConfArray[i])
                _buttonStates[i].hidden = true;
        }
    }

    // Check plugin hide-all flag
    bool doHideAllPlugin = _toolbarPluginButtonsConf._isHideAll;
    if (_nbDynButtons > 0 && _nbTotalButtons > _nbButtons) {
        // Separator + plugin buttons start at index _nbButtons
        if (doHideAllPlugin)
            _buttonStates[_nbButtons].hidden = true;
        for (int j = 0; j < _nbDynButtons && (_nbButtons + 1 + j) < _nbTotalButtons; ++j) {
            size_t pluginIdx = static_cast<size_t>(_nbButtons + 1 + j);
            if (pluginIdx < static_cast<size_t>(_buttonStates.size())) {
                size_t confIdx = static_cast<size_t>(j + 1);
                if (doHideAllPlugin
                    || confIdx >= _toolbarPluginButtonsConf._showPluginButtonsArray.size()
                    || !_toolbarPluginButtonsConf._showPluginButtonsArray[confIdx]) {
                    _buttonStates[pluginIdx].hidden = true;
                }
            }
        }
    }

    reset(true);
    return true;
}

void Toolbar::destroy()
{
    if (_pRebar) {
        _pRebar->removeBand(_reBarBandID);
        _pRebar = nullptr;
    }
    clear();
    _actions.clear();
    _buttonStates.clear();
    _toolBarIcons.destroy();
    deleteLater();
}

void Toolbar::reset(bool create)
{
    // Recreate the toolbar widget and all QActions.
    // Translates Win32 DestroyWindow + CreateWindowEx → clear() + rebuildActions().
    if (create) {
        // Save button states BEFORE clearing (actions still valid at this point)
        for (int i = 0; i < _nbCurrentButtons && i < _buttonStates.size() && i < _actions.size(); ++i) {
            _buttonStates[i].enabled = _actions[i]->isEnabled();
            _buttonStates[i].hidden = !_actions[i]->isVisible();
            _buttonStates[i].checked = _actions[i]->isChecked();
        }
        clear();          // QToolBar removes all actions and separators
        _actions.clear();
    }

    // Apply dark mode tooltip style
    if (NppDarkMode::instance().isEnabled()) {
        setStyleSheet(QStringLiteral(
            "QToolBar { background: #2e2e2e; border: none; }"
            "QToolButton { background: transparent; }"
        ));
    } else {
        setStyleSheet(QString());
    }

    // Determine icon size from state
    int iconSz = DPIManagerV2::scale((_state == TB_LARGE || _state == TB_LARGE2) ? 32 : 16, this);
    setIconSize(QSize(iconSz, iconSz));
    setMovable(true);

    // Apply custom icon overrides if any
    applyCustomIcons();

    // Rebuild all actions
    rebuildActions();

    // Apply restored button states
    for (size_t i = 0; i < static_cast<size_t>(_nbTotalButtons) && i < static_cast<size_t>(_actions.size()); ++i) {
        if (i < static_cast<size_t>(_buttonStates.size())) {
            _actions[i]->setEnabled(_buttonStates[i].enabled);
            _actions[i]->setVisible(!_buttonStates[i].hidden);
            if (_actions[i]->isCheckable())
                _actions[i]->setChecked(_buttonStates[i].checked);
        }
    }

    updateIconLists();
    _nbCurrentButtons = _actions.size();

    if (create) {
        // Notify parent (mirrors NPPM_INTERNAL_TOOLBARICONSCHANGED)
        QWidget* topLevel = window();
        if (topLevel) {
            QWindow* win = topLevel->windowHandle();
            if (win) {
                win->setProperty("_npp_toolbar_style_changed", static_cast<int>(_state));
            }
        }
    }
}

void Toolbar::rebuildActions()
{
    // Build QActions from the button-unit array.
    // Translates Win32 TBBUTTON[] → QAction[]
    int btnIdx = 0;

    for (int i = 0; i < _nbButtons && btnIdx < _nbTotalButtons; ++i, ++btnIdx) {
        const auto& unit = _buttonUnits[i];

        if (unit._cmdID == 0) {
            // Separator
            addSeparator();
            continue;
        }

        ToolbarButtonAction* action = new ToolbarButtonAction(unit._cmdID, this);
        action->setData(unit._cmdID);

        // Set icon from the current icon list
        if (btnIdx < static_cast<int>(_buttonStates.size())) {
            action->setInitialEnabled(_buttonStates[btnIdx].enabled);
        }

        // Set tooltip text from defText
        if (!unit._defText.isEmpty()) {
            action->setToolTip(unit._defText);
        }

        // Special style for dropdown buttons (e.g. "View All Characters")
        if (unit._cmdID == IDM_VIEW_ALL_CHARACTERS) {
            QToolButton* btn = qobject_cast<QToolButton*>(
                widgetForAction(action));
            if (btn) {
                btn->setPopupMode(QToolButton::MenuButtonPopup);
            }
        }

        addAction(action);
        _actions.append(action);
    }

    // Add separator before plugin buttons
    if (_nbDynButtons > 0) {
        addSeparator();
        ++btnIdx; // account for separator

        // Add dynamic (plugin) buttons
        for (int j = 0; j < _nbDynButtons && btnIdx < _nbTotalButtons; ++j, ++btnIdx) {
            const auto& dyn = _vDynBtnReg[j];
            ToolbarButtonAction* action = new ToolbarButtonAction(dyn._message, this);
            action->setData(dyn._message);

            if (!dyn._icon.isNull()) {
                action->setIcon(dyn._icon);
            } else if (!dyn._absentIcon.isNull()) {
                action->setIcon(dyn._absentIcon);
            }

            addAction(action);
            _actions.append(action);
        }
    }
}

void Toolbar::applyCustomIcons()
{
    // Apply icon overrides from the XML-defined custom icons list.
    // Translates: Win32 change2CustomIconsIfAny() loop over _customIconVect
    for (const auto& loc : _customIconVect) {
        // Replace across all icon lists at once (mirrors Win32 ImageList_ReplaceIcon)
        _toolBarIcons.replaceIcon(-1, loc._iconIndex, loc._iconLocation);
    }
}

void Toolbar::updateIconLists()
{
    // Switch active icon list based on current state and dark-mode setting.
    // Translates: Win32 ImageList_Draw + HIMAGELIST → QAction::setIcon()
    if (_state == TB_STANDARD) {
        // Standard Windows bitmaps — icons set from resource in create()
        return;
    }

    bool dark = NppDarkMode::instance().isEnabled();
    bool set2 = (_state == TB_LARGE2 || _state == TB_SMALL2);

    const QVector<QIcon>& defaultList = dark
        ? (set2 ? _toolBarIcons.getDefaultListSetDM2() : _toolBarIcons.getDefaultListDM())
        : (set2 ? _toolBarIcons.getDefaultListSet2()   : _toolBarIcons.getDefaultList());

    // Apply icons to all actions
    for (int i = 0; i < _actions.size() && i < defaultList.size(); ++i) {
        _actions[i]->setIcon(defaultList[i]);
    }
}

ToolbarButtonAction* Toolbar::findAction(int cmdID) const
{
    for (auto* act : std::as_const(_actions)) {
        if (act->data().toInt() == cmdID) {
            return qobject_cast<ToolbarButtonAction*>(act);
        }
    }
    return nullptr;
}

void Toolbar::enable(int cmdID, bool doEnable) const
{
    if (auto* action = const_cast<Toolbar*>(this)->findAction(cmdID))
        action->setEnabled(doEnable);
}

int Toolbar::getWidth() const
{
    // Translate: TB_GETBUTTONSIZE × visible button count → QToolBar widthHint
    QSize hint = sizeHint();
    return hint.width();
}

int Toolbar::getHeight() const
{
    QSize hint = sizeHint();
    return hint.height();
}

void Toolbar::reduce()  { setState(TB_SMALL); }
void Toolbar::enlarge() { setState(TB_LARGE); }
void Toolbar::reduceToSet2()  { setState(TB_SMALL2); }
void Toolbar::enlargeToSet2() { setState(TB_LARGE2); }

void Toolbar::setToBmpIcons()
{
    setState(TB_STANDARD);
    reset(true);
}

bool Toolbar::getCheckState(int cmdID) const
{
    auto* action = const_cast<Toolbar*>(this)->findAction(cmdID);
    return action && action->isChecked();
}

void Toolbar::setCheck(int cmdID, bool willBeChecked) const
{
    if (auto* action = const_cast<Toolbar*>(this)->findAction(cmdID))
        action->setChecked(willBeChecked);
}

void Toolbar::setState(ToolBarStatusType state)
{
    _state = state;
    updateIconLists();

    // Notify main window (mirrors NPPM_INTERNAL_TOOLBARICONSCHANGED)
    QWidget* top = window();
    if (top) {
        QWindow* win = top->windowHandle();
        if (win) {
            QVariant v;
            v.setValue(static_cast<int>(_state));
            win->setProperty("_npp_toolbar_style_changed", v);
        }
    }
    emit toolBarStyleChanged(_state);
}

bool Toolbar::changeIcons(int whichLst, int iconIndex, const QString& iconLocation) const
{
    return _toolBarIcons.replaceIcon(whichLst, iconIndex, iconLocation);
}

void Toolbar::registerDynBtn(int message, const toolbarIcons* iconHandles, const QIcon& absentIcon)
{
    if (message == 0) return;
    // Register a dynamic (plugin) button. Must be called before init().
    DynamicCmdIcoBmp dyn;
    dyn._message = message;
    dyn._icon = iconHandles ? iconHandles->hToolbarIcon : QIcon();
    dyn._absentIcon = absentIcon;
    dyn._iconDM = dyn._icon;  // same for now
    _vDynBtnReg.append(dyn);
}

void Toolbar::registerDynBtnDM(int message, const toolbarIconsWithDarkMode* iconHandles)
{
    if (message == 0) return;
    DynamicCmdIcoBmp dyn;
    dyn._message = message;
    dyn._icon = iconHandles ? iconHandles->hToolbarIcon : QIcon();
    dyn._iconDM = iconHandles ? iconHandles->hToolbarIconDarkMode : QIcon();
    dyn._absentIcon = QIcon();
    _vDynBtnReg.append(dyn);
}

void Toolbar::doPopup(const QPoint& /*chevPoint*/)
{
    // Show a popup menu for hidden/overflowed toolbar buttons (chevron).
    // Translates: WM_NOTIFY + TTN_GETDISPINFO chevron → QMenu overflow
    QMenu menu(this);
    int hiddenCount = 0;
    for (auto* act : _actions) {
        if (!act->isVisible() && act->data().toInt() != 0) {
            menu.addAction(act->icon(), act->toolTip());
            ++hiddenCount;
        }
    }
    if (hiddenCount > 0) {
        menu.exec(QCursor::pos());
    }
}

void Toolbar::addToRebar(ReBar* rebar)
{
    if (_pRebar) return;
    _pRebar = rebar;
    _reBarBandID = REBAR_BAR_TOOLBAR;

    if (rebar) {
        ReBarBandInfo band;
        band._id = _reBarBandID;
        band._widget = this;
        band._width = width();
        band._height = height();
        band._visible = isVisible();
        band._newLine = false;
        rebar->addBand(&band, true);
    }
}

void Toolbar::resizeIconsDpi(int /*dpi*/)
{
    // Rebuild icons at the new DPI scale.
    int iconSz = DPIManagerV2::scale((_state == TB_LARGE || _state == TB_LARGE2) ? 32 : 16, this);
    _toolBarIcons.resizeIcon(QSize(iconSz, iconSz));
    reset(true);
}

void Toolbar::onActionTriggered()
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        emit buttonClicked(action->data().toInt());
    }
}

void Toolbar::onChevronClicked()
{
    doPopup(QCursor::pos());
}

void Toolbar::refreshDarkMode()
{
    // Apply dark/light mode stylesheet to toolbar and all child widgets.
    if (NppDarkMode::instance().isEnabled()) {
        setStyleSheet(QStringLiteral(
            "QToolBar {"
            "  background-color: #2e2e2e;"
            "  border: none;"
            "  spacing: 2px;"
            "}"
            "QToolButton {"
            "  background-color: transparent;"
            "  border: none;"
            "  padding: 2px;"
            "}"
            "QToolButton:hover {"
            "  background-color: #3e3e3e;"
            "}"
            "QToolButton:pressed {"
            "  background-color: #4e4e4e;"
            "}"
        ));
    } else {
        setStyleSheet(QString());
    }
    updateIconLists();
}

bool Toolbar::event(QEvent* event)
{
    // Handle screen change (DPI change) to rescale toolbar icons.
    if (event->type() == QEvent::ScreenChangeInternal ||
        event->type() == QEvent::LocaleChange) {
        DPIManagerV2 mgr;
        mgr.setDpiWithScreen(this);
        resizeIconsDpi(mgr.getDpi());
        return QToolBar::event(event);
    }
    return QToolBar::event(event);
}


// ============================================================================
// ReBar — Qt container for draggable toolbar bands (mirrors Win32 REBARCLASSNAME)
// ============================================================================

ReBar::ReBar(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

ReBar::~ReBar() = default;

void ReBar::init(QApplication* /*app*/, QWidget* /*parent*/)
{
    // ReBar is a plain QWidget in Qt6; no special initialisation needed.
    // Mirrors: CreateWindowEx(REBARCLASSNAME) + RB_SETBARINFO.
}

void ReBar::destroy()
{
    // Remove all bands (toolbars)
    for (auto* w : _bandWidgets.values()) {
        layout()->removeWidget(w);
    }
    _bandWidgets.clear();
    _usedIDs.clear();
}

void ReBar::display(bool toShow)
{
    setVisible(toShow);
}

bool ReBar::addBand(ReBarBandInfo* bandInfo, bool useID)
{
    // Add a toolbar (or other widget) as a band in the rebar.
    // Translates: RB_INSERTBAND → QVBoxLayout::addWidget + _bandWidgets map.
    int bandID = bandInfo->_id;

    // Check ID uniqueness
    if (useID && isIDTaken(bandID)) return false;

    if (!useID) {
        bandID = getNewID();
        bandInfo->_id = bandID;
    }

    // Add to layout
    if (bandInfo->_widget) {
        QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout());
        if (boxLayout) {
            // Insert in reverse order so toolbar bands appear top-to-bottom
            boxLayout->insertWidget(0, bandInfo->_widget);
        }
        bandInfo->_widget->setVisible(bandInfo->_visible);
        _bandWidgets[bandID] = bandInfo->_widget;
    }

    emit bandChanged(bandID);
    return true;
}

void ReBar::reNew(int id, ReBarBandInfo* bandInfo)
{
    // Update an existing band's size/visibility.
    auto it = _bandWidgets.find(id);
    if (it == _bandWidgets.end()) return;

    QWidget* w = it.value();
    if (bandInfo->_widget && bandInfo->_widget != w) {
        layout()->replaceWidget(w, bandInfo->_widget);
        _bandWidgets[id] = bandInfo->_widget;
        w = bandInfo->_widget;
    }
    if (!bandInfo->_visible) {
        w->hide();
    } else {
        w->show();
    }
}

void ReBar::removeBand(int id)
{
    auto it = _bandWidgets.find(id);
    if (it == _bandWidgets.end()) return;

    QWidget* w = it.value();
    layout()->removeWidget(w);
    w->hide();
    _bandWidgets.erase(it);
    releaseID(id);
    emit bandChanged(id);
}

void ReBar::setIDVisible(int id, bool show)
{
    auto it = _bandWidgets.find(id);
    if (it == _bandWidgets.end()) return;
    it.value()->setVisible(show);
    emit bandChanged(id);
}

bool ReBar::getIDVisible(int id) const
{
    auto it = _bandWidgets.find(id);
    if (it == _bandWidgets.end()) return false;
    return it.value()->isVisible();
}

void ReBar::setGrayBackground(int /*id*/)
{
    // Translates: RB_SETBANDINFO with hbmBack (background bitmap for incremental search).
    // Not directly applicable in Qt — set stylesheet background instead.
    setStyleSheet(QStringLiteral(
        "ReBar, QWidget#ReBar { background-color: #f0f0f0; }"
    ));
}

int ReBar::getNewID()
{
    int idToUse = REBAR_BAR_EXTERNAL;
    for (int id : _usedIDs) {
        if (id < idToUse) continue;
        if (id == idToUse) ++idToUse;
        else break;
    }
    _usedIDs.append(idToUse);
    return idToUse;
}

void ReBar::releaseID(int id)
{
    _usedIDs.removeOne(id);
}

bool ReBar::isIDTaken(int id) const
{
    return _usedIDs.contains(id);
}
