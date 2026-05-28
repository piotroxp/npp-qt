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

#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QCheckBox>
#include <QSettings>
#include <memory>

// CustomFileDialog - Customizable file dialog
// Replaces COM-based IFileDialog with Qt QFileDialog
// Filter entry for file dialogs
struct FilterEntry {
    QString name;
    QStringList extensions;
};

class CustomFileDialog
{
public:
    explicit CustomFileDialog(QWidget* parent);
    ~CustomFileDialog();

    void setTitle(const QString& title);
    void setExtFilter(const QString& text, const QString& ext);
    void setExtFilter(const QString& text, std::initializer_list<const QString> exts);
    void setDefExt(const QString& ext);
    void setDefFileName(const QString& fn);
    void setFolder(const QString& folder);
    void setCheckbox(const QString& text, bool isActive = true);
    void setExtIndex(int extTypeIndex);
    void setSaveAsCopy(bool isSavingAsCopy);
    bool getOpenTheCopyAfterSaveAsCopy();

    void enableFileTypeCheckbox(const QString& text, bool value);
    bool getFileTypeCheckboxValue() const;

    // Execute dialogs
    QString doSaveDlg();
    QString pickFolder();
    QString doOpenSingleFileDlg();
    QList<QString> doOpenMultiFilesDlg();

    bool getCheckboxState() const;
    bool isReadOnly() const;

private:
    QWidget* _parent = nullptr;
    QString _title;
    QString _defExt;
    QString _defFileName;
    QString _folder;
    QString _checkboxText;
    bool _checkboxState = true;
    bool _saveAsCopy = false;
    
    QVector<FilterEntry> _filters;
    int _currentFilterIndex = 0;
};

// Helper function to convert filter to Qt format
inline QString convertToQtFilter(const QString& name, const QStringList& exts)
{
    QString extStr;
    for (const QString& ext : exts) {
        if (!extStr.isEmpty()) extStr += " ";
        extStr += ext;
    }
    return QString("%1 (*.%2)").arg(name).arg(extStr);
}

// Helper function to convert all filters to Qt format
inline QString convertAllFilters(const QVector<FilterEntry>& filters)
{
    QStringList result;
    for (const auto& f : filters) {
        result << convertToQtFilter(f.name, f.extensions);
    }
    return result.join(";;");
}