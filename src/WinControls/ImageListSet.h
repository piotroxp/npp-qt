// Semantic Lift: Win32 ImageListSet → Qt6 ImageListSet
// Original: PowerEditor/src/WinControls/ImageListSet/ImageListSet.cpp
// Target: npp-qt/src/WinControls/ImageListSet.h + .cpp

#pragma once

#include <QIcon>
#include <QVector>
#include <QSize>
#include <QColor>
#include <QObject>
#include <QString>
#include <utility>

// Fluent toolbar color options mirroring the Win32 FluentColor enum.
enum class FluentColor {
    accent       = 0,
    red          = 1,
    green        = 2,
    blue         = 3,
    purple       = 4,
    cyan         = 5,
    olive        = 6,
    yellow       = 7,
    custom       = 8,
    defaultColor = 9
};

// -----------------------------------------------------------------------
// IconList — manages a flat list of icons (mirrors Win32 HIMAGELIST)
// -----------------------------------------------------------------------
class IconList
{
public:
    IconList() = default;
    ~IconList() = default;

    // Initialize with a size. Clears any existing icons.
    void init(const QSize& iconSize);

    // Create and populate from a resource ID array.
    void create(const QSize& iconSize, const QVector<int>& iconIDArray);

    // Add an icon from a resource ID.
    // On failure loads IDI_ICONABSENT as fallback if failIconID is -1.
    void addIcon(int iconID, const QSize& desiredSize, int failIconID = -1, bool applyFluentColor = false);

    // Add an icon from an already-loaded QIcon.
    void addIcon(const QIcon& icon);

    // Replace icon at index with one loaded from a file path.
    bool replaceIcon(size_t index, const QString& iconLocation);

    // Apply Fluent-style color substitution to a HICON-compatible icon.
    // Maps cOld → cNew (with accent fallback when cNew == 0) for each pixel
    // within tolerance of cOld.
    bool applyFluentColor(QIcon* icon,
                          const QVector<std::pair<QColor, QColor>>& colorMappings,
                          int tolerance = 10);

    // Apply Fluent color using the current NPP dark/light mode color scheme.
    bool applyFluentColor(QIcon* icon);

    // Remove all icons.
    void removeAll();

    // Returns the number of icons.
    size_t size() const { return _icons.size(); }

    // Returns icon at index (returns null icon if out of range).
    const QIcon& icon(size_t index) const;

    const QSize& iconSize() const { return _iconSize; }

private:
    QVector<QIcon> _icons;
    QSize _iconSize{16, 16};
};

// -----------------------------------------------------------------------
// IconLists — manages 8 icon lists (default, disable, dark mode, etc.)
// HLIST_DEFAULT, HLIST_DISABLE, HLIST_DEFAULT2, HLIST_DISABLE2,
// HLIST_DEFAULT_DM, HLIST_DISABLE_DM, HLIST_DEFAULT_DM2, HLIST_DISABLE_DM2
// -----------------------------------------------------------------------
enum ToolBarIconList {
    HLIST_DEFAULT       = 0,
    HLIST_DISABLE       = 1,
    HLIST_DEFAULT2      = 2,
    HLIST_DISABLE2      = 3,
    HLIST_DEFAULT_DM    = 4,
    HLIST_DISABLE_DM    = 5,
    HLIST_DEFAULT_DM2   = 6,
    HLIST_DISABLE_DM2   = 7,
    HLIST_COUNT         = 8
};

struct IconListButtonUnit {
    int _defaultIcon         = -1;
    int _grayIcon            = -1;
    int _defaultIcon2        = -1;
    int _grayIcon2           = -1;
    int _defaultDarkModeIcon = -1;
    int _grayDarkModeIcon    = -1;
    int _defaultDarkModeIcon2 = -1;
    int _grayDarkModeIcon2   = -1;
    int _stdIcon             = -1;
    const wchar_t* _defText  = nullptr;
    const wchar_t* _iconName = nullptr;
    int _cmdID                = -1;
};

struct DynamicIconCmd {
    int _message = 0;
    QIcon _icon;
    QIcon _iconDM;
    QIcon _absentIcon;
};

class IconLists
{
public:
    IconLists() = default;
    ~IconLists() = default;

    // Build the 8-icon-list set from button units and dynamic commands.
    void init(const QVector<IconListButtonUnit>& buttonUnits,
              const QVector<DynamicIconCmd>& dynamicCommands);

    // Create with an instance handle and icon size.
    void create(const QSize& iconSize);

    // Tear down.
    void destroy();

    // Rebuild icons at a new size (e.g., DPI change).
    void reInit(const QSize& newSize);

    // Returns a reference to one of the 8 icon lists.
    IconList& list(int index) { return _iconListVector[index]; }
    const IconList& list(int index) const { return _iconListVector[index]; }

    IconList& getDefaultList()         { return _iconListVector[HLIST_DEFAULT]; }
    IconList& getDisableList()         { return _iconListVector[HLIST_DISABLE]; }
    IconList& getDefaultListSet2()     { return _iconListVector[HLIST_DEFAULT2]; }
    IconList& getDisableListSet2()     { return _iconListVector[HLIST_DISABLE2]; }
    IconList& getDefaultListDM()       { return _iconListVector[HLIST_DEFAULT_DM]; }
    IconList& getDisableListDM()       { return _iconListVector[HLIST_DISABLE_DM]; }
    IconList& getDefaultListSetDM2()   { return _iconListVector[HLIST_DEFAULT_DM2]; }
    IconList& getDisableListSetDM2()   { return _iconListVector[HLIST_DISABLE_DM2]; }

    const IconList& getDefaultList()    const { return _iconListVector[HLIST_DEFAULT]; }
    const IconList& getDisableList()    const { return _iconListVector[HLIST_DISABLE]; }
    const IconList& getDefaultListSet2() const { return _iconListVector[HLIST_DEFAULT2]; }
    const IconList& getDisableListSet2() const { return _iconListVector[HLIST_DISABLE2]; }
    const IconList& getDefaultListSetDM2() const { return _iconListVector[HLIST_DEFAULT_DM2]; }
    const IconList& getDisableListSetDM2() const { return _iconListVector[HLIST_DISABLE_DM2]; }

private:
    QVector<IconList> _iconListVector;
    QVector<IconListButtonUnit> _buttonUnits;
    QVector<DynamicIconCmd> _moreCommands;
};
