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

#include "CustomFileDialog.h"

#include <QDir>
#include <QFileInfo>

CustomFileDialog::CustomFileDialog(QWidget* parent)
    : _parent(parent)
{
}

CustomFileDialog::~CustomFileDialog() = default;

void CustomFileDialog::setTitle(const QString& title)
{
    _title = title;
}

void CustomFileDialog::setExtFilter(const QString& text, const QString& ext)
{
    FilterEntry entry;
    entry.name = text;
    entry.extensions = ext.split(';', Qt::SkipEmptyParts);
    _filters.append(entry);
}

void CustomFileDialog::setExtFilter(const QString& text, std::initializer_list<const QString> exts)
{
    FilterEntry entry;
    entry.name = text;
    for (const QString& ext : exts) {
        entry.extensions.append(ext);
    }
    _filters.append(entry);
}

void CustomFileDialog::setDefExt(const QString& ext)
{
    _defExt = ext;
}

void CustomFileDialog::setDefFileName(const QString& fn)
{
    _defFileName = fn;
}

void CustomFileDialog::setFolder(const QString& folder)
{
    _folder = folder;
}

void CustomFileDialog::setCheckbox(const QString& text, bool isActive)
{
    _checkboxText = text;
    _checkboxState = isActive;
}

void CustomFileDialog::setExtIndex(int extTypeIndex)
{
    _currentFilterIndex = extTypeIndex;
}

void CustomFileDialog::setSaveAsCopy(bool isSavingAsCopy)
{
    _saveAsCopy = isSavingAsCopy;
}

bool CustomFileDialog::getOpenTheCopyAfterSaveAsCopy() const
{
    // Would return based on checkbox state
    return false;
}

void CustomFileDialog::enableFileTypeCheckbox(const QString& text, bool value)
{
    Q_UNUSED(text);
    Q_UNUSED(value);
}

bool CustomFileDialog::getFileTypeCheckboxValue() const
{
    return _checkboxState;
}

QString CustomFileDialog::doSaveDlg()
{
    QString filterStr = convertAllFilters(_filters);
    
    QString selectedFilter;
    if (_currentFilterIndex >= 0 && _currentFilterIndex < _filters.size()) {
        selectedFilter = convertToQtFilter(_filters[_currentFilterIndex].name, 
                                           _filters[_currentFilterIndex].extensions);
    }
    
    QString result = QFileDialog::getSaveFileName(
        _parent,
        _title.isEmpty() ? "Save" : _title,
        _folder.isEmpty() ? QDir::homePath() : _folder,
        filterStr,
        &selectedFilter
    );
    
    // Add default extension if needed
    if (!result.isEmpty() && !_defExt.isEmpty()) {
        QFileInfo fi(result);
        if (fi.suffix().isEmpty()) {
            result += "." + _defExt;
        }
    }
    
    return result;
}

QString CustomFileDialog::pickFolder()
{
    QString result = QFileDialog::getExistingDirectory(
        _parent,
        _title.isEmpty() ? "Select Folder" : _title,
        _folder.isEmpty() ? QDir::homePath() : _folder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    return result;
}

QString CustomFileDialog::doOpenSingleFileDlg()
{
    QString filterStr = convertAllFilters(_filters);
    
    QString selectedFilter;
    if (_currentFilterIndex >= 0 && _currentFilterIndex < _filters.size()) {
        selectedFilter = convertToQtFilter(_filters[_currentFilterIndex].name, 
                                           _filters[_currentFilterIndex].extensions);
    }
    
    QString result = QFileDialog::getOpenFileName(
        _parent,
        _title.isEmpty() ? "Open" : _title,
        _folder.isEmpty() ? QDir::homePath() : _folder,
        filterStr,
        &selectedFilter
    );
    
    return result;
}

QList<QString> CustomFileDialog::doOpenMultiFilesDlg()
{
    QString filterStr = convertAllFilters(_filters);
    
    QString selectedFilter;
    if (_currentFilterIndex >= 0 && _currentFilterIndex < _filters.size()) {
        selectedFilter = convertToQtFilter(_filters[_currentFilterIndex].name, 
                                           _filters[_currentFilterIndex].extensions);
    }
    
    QList<QString> result = QFileDialog::getOpenFileNames(
        _parent,
        _title.isEmpty() ? "Open" : _title,
        _folder.isEmpty() ? QDir::homePath() : _folder,
        filterStr,
        &selectedFilter
    );
    
    return result;
}

bool CustomFileDialog::getCheckboxState() const
{
    return _checkboxState;
}

bool CustomFileDialog::isReadOnly() const
{
    return false;
}