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

#pragma once

#include <QHash>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QVector>

// IconList - Image list management
// Replaces Win32 ImageList with Qt QHash<int, QIcon>
class IconList
{
public:
    IconList() = default;
    ~IconList() = default;

    void init(int iconSize);
    void create(int iconSize, const QVector<int>& iconIDArray);
    
    void destroy();
    void removeAll();

    QIcon getIcon(int index) const;
    int getCount() const { return _icons.size(); }
    
    void addIcon(int iconID, int cx = 16, int cy = 16, int failIconID = -1);
    void addIcon(const QIcon& icon);
    bool changeIcon(size_t index, const QString& iconLocation);

private:
    int _iconSize = 16;
    QVector<QPixmap> _icons;
    QHash<int, QPixmap> _iconMap;  // Maps icon ID to pixmap
};

// ToolBar button unit
struct ToolBarButtonUnit {
    int _cmdID = 0;
    int _defaultIcon = -1;
    int _grayIcon = -1;
    int _defaultIcon2 = -1;
    int _grayIcon2 = -1;
    int _defaultDarkModeIcon = -1;
    int _grayDarkModeIcon = -1;
    int _defaultDarkModeIcon2 = -1;
    int _grayDarkModeIcon2 = -1;
    int _stdIcon = -1;
};

// Dynamic command icon bitmap
struct DynamicCmdIcoBmp {
    int _message = 0;
    QPixmap _hBmp;
    QIcon _hIcon;
    QIcon _hIcon_DM;
};

typedef QVector<ToolBarButtonUnit> ToolBarIconIDs;

enum ToolbarIconList {
    HLIST_DEFAULT,
    HLIST_DISABLE,
    HLIST_DEFAULT2,
    HLIST_DISABLE2,
    HLIST_DEFAULT_DM,
    HLIST_DISABLE_DM,
    HLIST_DEFAULT_DM2,
    HLIST_DISABLE_DM2
};

// ToolBarIcons - Toolbar icon management
class ToolBarIcons
{
public:
    ToolBarIcons() = default;
    ~ToolBarIcons() { destroy(); }

    void init(const QVector<ToolBarButtonUnit>& buttonUnitArray, const QVector<DynamicCmdIcoBmp>& cmds2add);
    void create(int iconSize);
    void destroy();
    void resizeIcon(int size);

    QIcon getDefaultIcon(size_t index) const;
    QIcon getDisableIcon(size_t index) const;
    QIcon getDefaultLstDM(size_t index) const;
    QIcon getDisableLstDM(size_t index) const;

    bool replaceIcon(size_t whichList, size_t iconIndex, const QString& iconLocation);

    int getStdIconAt(int i) const;

private:
    void reInit(int size);

    QVector<ToolBarButtonUnit> _tbiis;
    QVector<DynamicCmdIcoBmp> _moreCmds;
    
    QVector<IconList> _iconListVector;
    int _iconSize = 16;
};