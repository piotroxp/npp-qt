// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
#include <QDomElement>
#include <memory>

// ToolBarDisplayType / ToolBarStatusType — maps to icon set selection
enum class ToolBarDisplayType { TB_SMALL, TB_LARGE, TB_SMALL2, TB_LARGE2, TB_STANDARD };
using ToolBarStatusType = ToolBarDisplayType;  // backward compat alias

// iconLocator — tracks per-icon-list override entries (custom user icons)
struct iconLocator {
    int _listIndex = 0;
    int _iconIndex = 0;
    QString _iconLocation;

    iconLocator() = default;
    iconLocator(int listIdx, int iconIdx, const QString& iconLoc)
        : _listIndex(listIdx), _iconIndex(iconIdx), _iconLocation(iconLoc) {}
};

// ToolbarPluginButtonsConf — persisted plugin-button visibility state
struct ToolbarPluginButtonsConf {
    bool _isHideAll = false;
    QVector<bool> _showPluginButtonsArray;
};

// ToolBarButtonUnit — per-button definition (mirrors Win32 resource array)
struct ToolBarButtonUnit {
    int _cmdID = 0;
    QString _defText;
    QString _iconName;
    int _resourceIconID = -1;
};

// ToolBarIcons — manages all toolbar image lists (default, disabled, dark mode, sets 1 & 2)
// Wraps IconLists from ImageListSet.h and adds toolbar-specific helpers.
#include "ImageListSet.h"
class ToolBarIcons {
public:
    ToolBarIcons() = default;
    ~ToolBarIcons() = default;

    // Build image lists from button-unit array and dynamic-button registry.
    void init(const QVector<ToolBarButtonUnit>& buttonUnits, int arraySize,
              const QVector<DynamicIconCmd>& dynButtons);

    // Create icons at a given size (DPI-scaled).
    void create(const QSize& iconSize);

    // Resize icons to a new size (DPI change).
    void resizeIcon(const QSize& newSize);

    // Replace icon at (listIndex, iconIndex) with one loaded from file path.
    bool replaceIcon(int listIndex, int iconIndex, const QString& iconLocation);

    // Destroy all image lists.
    void destroy();

    // Access individual icon lists.
    QVector<QIcon>& getDefaultList()          { return _iconListVector[0]; }
    QVector<QIcon>& getDisableList()          { return _iconListVector[1]; }
    QVector<QIcon>& getDefaultListSet2()      { return _iconListVector[2]; }
    QVector<QIcon>& getDisableListSet2()      { return _iconListVector[3]; }
    QVector<QIcon>& getDefaultListDM()         { return _iconListVector[4]; }
    QVector<QIcon>& getDisableListDM()         { return _iconListVector[5]; }
    QVector<QIcon>& getDefaultListSetDM2()    { return _iconListVector[6]; }
    QVector<QIcon>& getDisableListSetDM2()    { return _iconListVector[7]; }

    const QVector<QIcon>& getDefaultList()    const { return _iconListVector[0]; }
    const QVector<QIcon>& getDisableList()    const { return _iconListVector[1]; }
    const QVector<QIcon>& getDefaultListSet2() const { return _iconListVector[2]; }
    const QVector<QIcon>& getDisableListSet2() const { return _iconListVector[3]; }
    const QVector<QIcon>& getDefaultListSetDM2() const { return _iconListVector[6]; }
    const QVector<QIcon>& getDisableListSetDM2() const { return _iconListVector[7]; }

private:
    // 8 icon lists: default, disabled, default2, disabled2, DM, DM-disabled, DM2, DM2-disabled
    QVector<QVector<QIcon>> _iconListVector = QVector<QVector<QIcon>>(8);
    QVector<ToolBarButtonUnit> _buttonUnits;
    QVector<int> _dynMessages;
    QSize _currentSize{16, 16};
    // Message → icon map for dynamic (plugin) buttons
    QMap<int, QIcon> _dynIconMap;
};

// DynamicIconCmd — dynamically registered plugin button icons
// (was DynamicCmdIcoBmp in Win32)
struct DynamicIconCmd;

// toolbarIcons — Win32 plugin toolbar icon handles (HBITMAP + HICON)
// Qt port: QPixmap + QIcon
struct toolbarIcons {
    QPixmap hToolbarBmp;
    QIcon   hToolbarIcon;
};

// toolbarIconsWithDarkMode — Win32 plugin toolbar icon handles with dark mode
// Qt port: QPixmap + QIcon light + QIcon dark
struct toolbarIconsWithDarkMode {
    QPixmap hToolbarBmp;
    QIcon   hToolbarIcon;
    QIcon   hToolbarIconDarkMode;
};

// Forward declaration
class DPIManagerV2;

// ============================================================================
// ToolbarButtonAction — QAction subclass that stores the Win32 command ID
// and current button state so Toolbar can map Qt signals back to NPP commands.
// ============================================================================
class ToolbarButtonAction : public QAction {
    Q_OBJECT
public:
    ToolbarButtonAction(int cmdID, QObject* parent = nullptr)
        : QAction(parent), _cmdID(cmdID) {}

    int cmdID() const { return _cmdID; }

    // Track the fsState bits from the original TBBUTTON
    void setInitialEnabled(bool enabled) { _wasInitiallyEnabled = enabled; }
    bool wasInitiallyEnabled() const { return _wasInitiallyEnabled; }

private:
    int _cmdID = 0;
    bool _wasInitiallyEnabled = true;
};

// ============================================================================
// Toolbar — QToolBar subclass
// Translates Win32 Common Controls Toolbar API → Qt6 QToolBar.
// ============================================================================
class Toolbar : public QToolBar
{
    Q_OBJECT

public:
    Toolbar(QWidget* parent = nullptr);
    ~Toolbar() override;

    // Load theme/custom-icon XML and apply any user overrides.
    void initTheme(const QDomElement& toolIconsDocRoot);

    // Load persisted toolbar button visibility state from config XML.
    void initHideButtonsConf(void* toolButtonsDocRoot,
                             const IconListButtonUnit* buttonUnitArray,
                             int buttonCount);

    // Main initialiser — mirrors Win32 CreateToolbarEx().
    // app is unused on Qt (retained for signature compat).
    bool init(QApplication* /*app*/, QWidget* parent,
             ToolBarStatusType type,
             const QVector<ToolBarButtonUnit>& buttonUnitArray,
             void* /*unusedHInstance*/ = nullptr, int /*iconSizeOverride*/ = 0);

    // Tear down: remove from ReBar, destroy icon lists, delete self.
    void destroy();

    // Enable or disable a button by command ID (TB_ENABLEBUTTON).
    void enable(int cmdID, bool doEnable) const;

    // Current total width (sum of all visible item widths).
    int getWidth() const;

    // Toolbar height including padding (TB_GETBUTTONSIZE + TB_GETPADDING).
    int getHeight() const;

    // Icon-size state transitions (reduce/enlarge, sets 1 & 2).
    void reduce();
    void enlarge();
    void reduceToSet2();
    void enlargeToSet2();
    void setToBmpIcons();

    // Toggle check state (TB_CHECKBUTTON).
    bool getCheckState(int cmdID) const;
    void setCheck(int cmdID, bool willBeChecked) const;

    // Current icon-size state.
    ToolBarStatusType getState() const { return _state; }

    // Replace icon at a specific image-list index (custom user icons).
    bool changeIcons(int whichLst, int iconIndex, const QString& iconLocation);

    // Register a dynamically-added plugin button (must be called before init()).
    // Translates: Win32 NPPM_ADDTOOLBARICON_DEPRECATED → Qt
    void registerDynBtn(int message, const toolbarIcons* iconHandles, const QIcon& absentIcon);
    // Translates: Win32 NPPM_ADDTOOLBARICON_FORDARKMODE → Qt
    void registerDynBtnDM(int message, const toolbarIconsWithDarkMode* iconHandles);

    // Show popup menu for hidden toolbar buttons (chevron overflow).
    void doPopup(const QPoint& chevPoint);

    // Add this toolbar as a band inside a ReBar container.
    void addToRebar(class ReBar* rebar);

    // Rebuild icons at new DPI (TB_SETBITMAPSIZE equivalent).
    void resizeIconsDpi(int dpi);

Q_SIGNALS:
    // Emitted when any toolbar button is clicked — carries Win32 command ID.
    void buttonClicked(int cmdID);
    void toolBarStyleChanged(ToolBarStatusType newState);

public Q_SLOTS:
    // Apply dark/light mode palette to toolbar and all actions.
    void refreshDarkMode();

protected:
    // Catch screen change to auto-rescale icons on DPI change.
    bool event(QEvent* event) override;

private Q_SLOTS:
    void onActionTriggered();
    void onChevronClicked();

private:
    // Recreate the toolbar widget and all actions.
    void reset(bool create = false);

    // Apply icon-size state and notify parent window.
    void setState(ToolBarStatusType state);

    // Find a QAction by its Win32 command ID.
    ToolbarButtonAction* findAction(int cmdID) const;

    // Apply custom icon overrides from _customIconVect.
    void applyCustomIcons();

    // Set the appropriate icon list based on current dark-mode and icon-set state.
    void updateIconLists();

    // Rebuild all toolbar QActions from the button-unit array.
    void rebuildActions();

    // Toolbar icons (8 image lists).
    ToolBarIcons _toolBarIcons;

    // Current icon-size state.
    ToolBarStatusType _state = ToolBarDisplayType::TB_SMALL;

    // Dynamic (plugin) buttons registered before init().
    QVector<DynamicIconCmd> _vDynBtnReg;

    // Total button count: static + separators + dynamic.
    int _nbButtons = 0;
    int _nbDynButtons = 0;
    int _nbTotalButtons = 0;
    int _nbCurrentButtons = 0;

    // Custom user icon overrides (from theme XML).
    QVector<iconLocator> _customIconVect;

    // Parsed toolbar icons XML (set by initTheme)
    QDomElement _toolIcons;

    // Per-button initial visibility (from config XML).
    std::unique_ptr<bool[]> _toolbarStdButtonsConfArray;

    // Plugin button visibility state.
    ToolbarPluginButtonsConf _toolbarPluginButtonsConf;

    // DPI-aware scaling helper.
    DPIManagerV2* _dpiManager = nullptr;

    // ReBar container reference (may be nullptr).
    class ReBar* _pRebar = nullptr;

    // Cached REBARBANDINFO.wID for removal on destroy.
    int _reBarBandID = 0;

    // All toolbar QActions (mirrors _pTBB array in Win32 version).
    QVector<ToolbarButtonAction*> _actions;
    // Local copy of button units needed for rebuildActions()
    QVector<ToolBarButtonUnit> _buttonUnits;

    // Win32-style button state snapshot for reset() — only fsState is preserved.
    struct BtnState {
        bool enabled = true;
        bool hidden = false;
        bool checked = false;
    };
    QVector<BtnState> _buttonStates;
};

// ============================================================================
// ReBar — QWidget-based rebar/band container
// Translates Win32 ReBar (COMCTL32) → Qt layout container.
// ============================================================================
class ReBar : public QWidget
{
    Q_OBJECT

public:
    explicit ReBar(QWidget* parent = nullptr);
    ~ReBar() override;

    void init(QApplication* /*app*/, QWidget* parent);
    void destroy();

    // Add a new band; if useID is true the id field of bandInfo is used,
    // otherwise a new ID is allocated.
    bool addBand(class ReBarBandInfo* bandInfo, bool useID);

    // Update an existing band's size/visibility.
    void reNew(int id, class ReBarBandInfo* bandInfo);

    // Remove a band by ID.
    void removeBand(int id);

    // Show or hide a band by ID.
    void setIDVisible(int id, bool show);
    bool getIDVisible(int id) const;

    // Apply a custom background bitmap to a band (incremental search UI).
    void setGrayBackground(int id);

    void display(bool toShow = true);

    int getHeight() const { return rect().height(); }

Q_SIGNALS:
    void bandChanged(int id);

private:
    int getNewID();
    void releaseID(int id);
    bool isIDTaken(int id) const;

    // All band IDs currently in use.
    QVector<int> _usedIDs;

    // Maps band ID → contained toolbar widget.
    QMap<int, QWidget*> _bandWidgets;
};


// ============================================================================
// ReBarBandInfo — Qt equivalent of Win32 REBARBANDINFO
// ============================================================================
class ReBarBandInfo {
public:
    int _id = 0;
    QString _text;
    QWidget* _widget = nullptr;
    int _width = 0;
    int _height = 0;
    bool _visible = true;
    bool _newLine = true;
};
