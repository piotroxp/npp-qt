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

#include "ImageListSet.h"

#include <QFile>
#include <QImageReader>

void IconList::init(int iconSize)
{
    _iconSize = iconSize;
}

void IconList::create(int iconSize, const QVector<int>& iconIDArray)
{
    init(iconSize);
    _icons.reserve(iconIDArray.size());
    
    for (int iconID : iconIDArray) {
        // Would load from resources
        addIcon(iconID, iconSize, iconSize);
    }
}

void IconList::destroy()
{
    _icons.clear();
    _iconMap.clear();
}

void IconList::removeAll()
{
    _icons.clear();
}

QIcon IconList::getIcon(int index) const
{
    if (index >= 0 && index < _icons.size()) {
        return QIcon(_icons[index]);
    }
    return QIcon();
}

void IconList::addIcon(int iconID, int cx, int cy, int failIconID)
{
    Q_UNUSED(failIconID);
    
    // Would load icon from resources using DPIManagerV2::loadIcon
    // For now, create a placeholder
    QPixmap pix(cx, cy);
    pix.fill(Qt::transparent);
    _icons.append(pix);
    _iconMap[iconID] = pix;
}

void IconList::addIcon(const QIcon& icon)
{
    QPixmap pix = icon.pixmap(_iconSize, _iconSize);
    _icons.append(pix);
}

bool IconList::changeIcon(size_t index, const QString& iconLocation)
{
    if (index >= static_cast<size_t>(_icons.size())) return false;
    
    QPixmap pix;
    if (pix.load(iconLocation)) {
        _icons[static_cast<int>(index)] = pix;
        return true;
    }
    return false;
}

// =====================================================
// ToolBarIcons implementation
// =====================================================

void ToolBarIcons::init(const QVector<ToolBarButtonUnit>& buttonUnitArray, 
                        const QVector<DynamicCmdIcoBmp>& cmds2add)
{
    _tbiis = buttonUnitArray;
    _moreCmds = cmds2add;
    
    // Initialize 8 icon lists (light/dark x default/gray x set1/set2)
    _iconListVector.resize(8);
}

void ToolBarIcons::create(int iconSize)
{
    _iconSize = iconSize;
    
    for (int i = 0; i < _iconListVector.size(); ++i) {
        _iconListVector[i].init(iconSize);
    }
}

void ToolBarIcons::destroy()
{
    for (auto& list : _iconListVector) {
        list.destroy();
    }
    _iconListVector.clear();
}

void ToolBarIcons::resizeIcon(int size)
{
    reInit(size);
}

void ToolBarIcons::reInit(int size)
{
    _iconSize = size;
    destroy();
    create(size);
}

QIcon ToolBarIcons::getDefaultIcon(size_t index) const
{
    return _iconListVector[HLIST_DEFAULT].getIcon(static_cast<int>(index));
}

QIcon ToolBarIcons::getDisableIcon(size_t index) const
{
    return _iconListVector[HLIST_DISABLE].getIcon(static_cast<int>(index));
}

QIcon ToolBarIcons::getDefaultLstDM(size_t index) const
{
    return _iconListVector[HLIST_DEFAULT_DM].getIcon(static_cast<int>(index));
}

QIcon ToolBarIcons::getDisableLstDM(size_t index) const
{
    return _iconListVector[HLIST_DISABLE_DM].getIcon(static_cast<int>(index));
}

bool ToolBarIcons::replaceIcon(size_t whichList, size_t iconIndex, const QString& iconLocation)
{
    if (whichList >= static_cast<size_t>(_iconListVector.size())) return false;
    
    return _iconListVector[static_cast<int>(whichList)].changeIcon(iconIndex, iconLocation);
}

int ToolBarIcons::getStdIconAt(int i) const
{
    if (i >= 0 && i < _tbiis.size()) {
        return _tbiis[i]._stdIcon;
    }
    return -1;
}