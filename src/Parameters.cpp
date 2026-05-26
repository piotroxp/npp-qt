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

// Qt6 Translation - Parameters.cpp
// Translated from Win32 Registry to QSettings

#include "Parameters.h"
#include "NppXml.h"
#include "NppDarkMode.h"

#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>

// Static settings file location
static const QString settingsFileName = QStringLiteral("notepad++.ini");
static const QString stylesFileName = QStringLiteral("stylers.xml");
static const QString sessionFileName = QStringLiteral("session.xml");
static const QString shortcutsFileName = QStringLiteral("shortcuts.xml");
static const QString userDefinedLangFileName = QStringLiteral("userDefineLang.xml");
static const QString foundHistoryFileName = QStringLiteral("found_history.xml");

// NppParameters implementation
NppParameters::NppParameters() {
    // Initialize default values
    _nbLang = 0;
    _nbRecentFile = 0;
    _nbMaxRecentFile = 10;
    _putRecentFileInSubMenu = false;
    _recentFileCustomLength = 0;
    _nbUserLang = 0;
    _nbExternalLang = 0;
    _fileSaveDlgFilterIndex = 0;
    _pXmlNativeLangDoc = nullptr;
    _pXmlToolButtonsConfDoc = nullptr;
    
    // Get settings folder
    QString settingsFolder = getSettingsFolderQ();
}

NppParameters::~NppParameters() {
    // Cleanup
}

std::wstring NppParameters::getSettingsFolder() const {
    return getSettingsFolderQ().toStdWString();
}

QString NppParameters::getSettingsFolderQ() const {
    // Try local config first (for portable installations)
    QDir appDir = QCoreApplication::applicationDirPath();
    QString localConfig = appDir.filePath(QStringLiteral("config"));
    
    if (QFile::exists(localConfig) || QDir(appDir).exists()) {
        if (!QFile::exists(localConfig)) {
            QDir(appDir).mkdir(QStringLiteral("config"));
        }
        return localConfig;
    }
    
    // Fall back to standard config location
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(configPath).exists()) {
        QDir().mkpath(configPath);
    }
    return configPath;
}

QString NppParameters::getSettingsPath(const QString& fileName) const {
    return QDir(getSettingsFolderQ()).filePath(fileName);
}

bool NppParameters::load() {
    QString settingsPath = getSettingsPath(settingsFileName);
    QSettings settings(settingsPath, QSettings::IniFormat);
    
    // Load recent files
    int nbRecentFiles = settings.value(QStringLiteral("RecentFiles/nbRecentFiles"), 0).toInt();
    _nbRecentFile = qMin(nbRecentFiles, 20);
    
    for (int i = 0; i < _nbRecentFile; ++i) {
        QString key = QStringLiteral("RecentFiles/file%1").arg(i);
        QString recentFile = settings.value(key).toString();
        if (!recentFile.isEmpty()) {
            // Convert to wstring and store
            _LRFileList.push_back(std::make_unique<std::wstring>(recentFile.toStdWString()));
        }
    }
    
    // Load GUI settings
    QSettings guiSettings(settingsPath, QSettings::IniFormat);
    _nppGUI._toolbarShow = guiSettings.value(QStringLiteral("GUI/ToolbarShow"), true).toBool();
    _nppGUI._statusBarShow = guiSettings.value(QStringLiteral("GUI/StatusBarShow"), true).toBool();
    _nppGUI._menuBarShow = guiSettings.value(QStringLiteral("GUI/MenuBarShow"), true).toBool();
    _nppGUI._tabStatus = guiSettings.value(QStringLiteral("GUI/TabStatus"), 
                                           TAB_DRAWTOPBAR | TAB_DRAWINACTIVETAB | TAB_DRAGNDROP | TAB_REDUCE | TAB_CLOSEBUTTON | TAB_PINBUTTON).toInt();
    _nppGUI._splitterPos = guiSettings.value(QStringLiteral("GUI/SplitterPos"), true).toInt() == POS_VERTICAL;
    _nppGUI._tabSize = guiSettings.value(QStringLiteral("Editing/TabSize"), 4).toInt();
    _nppGUI._tabReplacedBySpace = guiSettings.value(QStringLiteral("Editing/TabReplacedBySpace"), false).toBool();
    _nppGUI._backspaceUnindent = guiSettings.value(QStringLiteral("Editing/BackspaceUnindent"), false).toBool();
    
    // Load window position
    _nppGUI._appPosLeft = guiSettings.value(QStringLiteral("GUI/PosLeft"), 100).toInt();
    _nppGUI._appPosTop = guiSettings.value(QStringLiteral("GUI/PosTop"), 100).toInt();
    _nppGUI._appPosRight = guiSettings.value(QStringLiteral("GUI/PosRight"), 1024).toInt();
    _nppGUI._appPosBottom = guiSettings.value(QStringLiteral("GUI/PosBottom"), 700).toInt();
    _nppGUI._isMaximized = guiSettings.value(QStringLiteral("GUI/Maximized"), false).toBool();
    
    // Load new document settings
    QVariant newDocFormat = guiSettings.value(QStringLiteral("NewDoc/Format"));
    if (newDocFormat.isValid()) {
        _nppGUI._newDocDefaultSettings._format = static_cast<EolType>(newDocFormat.toInt());
    }
    
    QVariant newDocEncoding = guiSettings.value(QStringLiteral("NewDoc/Encoding"));
    if (newDocEncoding.isValid()) {
        _nppGUI._newDocDefaultSettings._unicodeMode = static_cast<UniMode>(newDocEncoding.toInt());
    }
    
    // Load scintilla view parameters
    QSettings svpSettings(settingsPath, QSettings::IniFormat);
    _svp._lineNumberMarginShow = svpSettings.value(QStringLiteral("ScintillaView/LineNumberMarginShow"), true).toBool();
    _svp._bookMarkMarginShow = svpSettings.value(QStringLiteral("ScintillaView/BookMarkMarginShow"), true).toBool();
    _svp._indentGuideLineShow = svpSettings.value(QStringLiteral("ScintillaView/IndentGuideLineShow"), true).toBool();
    _svp._doWrap = svpSettings.value(QStringLiteral("ScintillaView/Wrap"), false).toBool();
    _svp._zoom = svpSettings.value(QStringLiteral("ScintillaView/Zoom"), 0).toInt();
    _svp._whiteSpaceShow = svpSettings.value(QStringLiteral("ScintillaView/WhiteSpaceShow"), false).toBool();
    _svp._eolShow = svpSettings.value(QStringLiteral("ScintillaView/EolShow"), false).toBool();
    _svp._folderStyle = svpSettings.value(QStringLiteral("ScintillaView/FolderStyle"), 0).toInt();
    _svp._lineWrapMethod = svpSettings.value(QStringLiteral("ScintillaView/LineWrapMethod"), 0).toInt();
    _svp._scrollBeyondLastLine = svpSettings.value(QStringLiteral("ScintillaView/ScrollBeyondLastLine"), true).toBool();
    _svp._doSmoothFont = svpSettings.value(QStringLiteral("ScintillaView/SmoothFont"), false).toBool();
    
    // Load dark mode settings
    QSettings darkSettings(settingsPath, QSettings::IniFormat);
    _nppGUI._darkmode._isEnabled = darkSettings.value(QStringLiteral("DarkMode/Enabled"), false).toBool();
    _nppGUI._darkmode._isEnabledPlugin = darkSettings.value(QStringLiteral("DarkMode/EnablePlugins"), true).toBool();
    _nppGUI._darkmode._colorTone = darkSettings.value(QStringLiteral("DarkMode/ColorTone"), 0).toInt();
    
    // Load theme
    _nppGUI._themeName = settings.value(QStringLiteral("Theme/ThemeName")).toString().toStdWString();
    
    // Load backup settings
    _nppGUI._backup = settings.value(QStringLiteral("Backup/Backup"), 0).toInt();
    _nppGUI._useDir = settings.value(QStringLiteral("Backup/UseDir"), false).toBool();
    _nppGUI._backupDir = settings.value(QStringLiteral("Backup/Dir")).toString().toStdWString();
    
    // Load recent file settings
    _nppGUI._checkHistoryFiles = settings.value(QStringLiteral("RecentFiles/CheckHistoryFiles"), false).toBool();
    _putRecentFileInSubMenu = settings.value(QStringLiteral("RecentFiles/PutRecentFileInSubMenu"), false).toBool();
    
    // Load shortcuts
    loadShortcuts();
    
    // Load user defined languages
    loadUserLanguages();
    
    // Load find history
    loadFindHistory();
    
    return true;
}

void NppParameters::loadShortcuts() {
    QString shortcutsPath = getSettingsPath(shortcutsFileName);
    QFile file(shortcutsPath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd() && !xml.hasError()) {
        if (xml.readNext() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("RegisteredCommands")) {
                // Parse shortcuts
            }
        }
    }
    
    file.close();
}

void NppParameters::loadUserLanguages() {
    QString udlPath = getSettingsPath(userDefinedLangFileName);
    QFile file(udlPath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd() && !xml.hasError()) {
        if (xml.readNext() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("UserLang")) {
                QString name = xml.attributes().value(QStringLiteral("name")).toString();
                QString ext = xml.attributes().value(QStringLiteral("ext")).toString();
                
                // Create user language container
                if (!name.isEmpty()) {
                    auto ulc = std::make_unique<UserLangContainer>(
                        name.toStdWString().c_str(),
                        ext.toStdWString().c_str(),
                        "v1.0",
                        false
                    );
                    _userLangArray.push_back(std::move(ulc));
                }
            }
        }
    }
    
    file.close();
    _nbUserLang = _userLangArray.size();
}

void NppParameters::loadFindHistory() {
    QString historyPath = getSettingsPath(foundHistoryFileName);
    QFile file(historyPath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd() && !xml.hasError()) {
        if (xml.readNext() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("FindHistory")) {
                _findHistory._isMatchWord = xml.attributes().value(QStringLiteral("matchWord")).toInt() != 0;
                _findHistory._isMatchCase = xml.attributes().value(QStringLiteral("matchCase")).toInt() != 0;
                _findHistory._isWrap = xml.attributes().value(QStringLiteral("wrap")).toInt() != 0;
                _findHistory._isDirectionDown = xml.attributes().value(QStringLiteral("directionDown")).toInt() != 0;
            }
        }
    }
    
    file.close();
}

bool NppParameters::writeRecentFileHistorySettings(int nbMaxFile) {
    QString settingsPath = getSettingsPath(settingsFileName);
    QSettings settings(settingsPath, QSettings::IniFormat);
    
    settings.beginGroup(QStringLiteral("RecentFiles"));
    
    int filesToWrite = nbMaxFile >= 0 ? qMin(nbMaxFile, static_cast<int>(_LRFileList.size())) 
                                        : _LRFileList.size();
    
    settings.setValue(QStringLiteral("nbRecentFiles"), filesToWrite);
    
    for (int i = 0; i < filesToWrite && i < _nbRecentFile; ++i) {
        if (_LRFileList[i]) {
            settings.setValue(QStringLiteral("file%1").arg(i), 
                           QString::fromStdWString(*_LRFileList[i]));
        }
    }
    
    settings.endGroup();
    
    return true;
}

bool NppParameters::writeHistory(const wchar_t* fullpath) {
    if (!fullpath) return false;
    
    QString historyPath = getSettingsPath(settingsFileName);
    QSettings settings(historyPath, QSettings::IniFormat);
    
    // Add to recent files list
    std::wstring filePath(fullpath);
    
    // Remove if already exists
    for (auto it = _LRFileList.begin(); it != _LRFileList.end(); ) {
        if (*it && **it == filePath) {
            it = _LRFileList.erase(it);
        } else {
            ++it;
        }
    }
    
    // Add at beginning
    _LRFileList.insert(_LRFileList.begin(), std::make_unique<std::wstring>(filePath));
    
    // Trim to max
    while (_LRFileList.size() > _nbMaxRecentFile) {
        _LRFileList.pop_back();
    }
    
    _nbRecentFile = _LRFileList.size();
    
    // Write to settings
    writeRecentFileHistorySettings();
    
    return true;
}

bool NppParameters::writeScintillaParams() {
    QString settingsPath = getSettingsPath(settingsFileName);
    QSettings settings(settingsPath, QSettings::IniFormat);
    
    settings.beginGroup(QStringLiteral("ScintillaView"));
    settings.setValue(QStringLiteral("LineNumberMarginShow"), _svp._lineNumberMarginShow);
    settings.setValue(QStringLiteral("BookMarkMarginShow"), _svp._bookMarkMarginShow);
    settings.setValue(QStringLiteral("IndentGuideLineShow"), _svp._indentGuideLineShow);
    settings.setValue(QStringLiteral("Wrap"), _svp._doWrap);
    settings.setValue(QStringLiteral("Zoom"), static_cast<int>(_svp._zoom));
    settings.setValue(QStringLiteral("WhiteSpaceShow"), _svp._whiteSpaceShow);
    settings.setValue(QStringLiteral("EolShow"), _svp._eolShow);
    settings.setValue(QStringLiteral("FolderStyle"), _svp._folderStyle);
    settings.setValue(QStringLiteral("LineWrapMethod"), _svp._lineWrapMethod);
    settings.setValue(QStringLiteral("ScrollBeyondLastLine"), _svp._scrollBeyondLastLine);
    settings.setValue(QStringLiteral("SmoothFont"), _svp._doSmoothFont);
    settings.endGroup();
    
    return true;
}

void NppParameters::createXmlTreeFromGUIParams() {
    QString settingsPath = getSettingsPath(settingsFileName);
    QSettings settings(settingsPath, QSettings::IniFormat);
    
    settings.beginGroup(QStringLiteral("GUI"));
    settings.setValue(QStringLiteral("ToolbarShow"), _nppGUI._toolbarShow);
    settings.setValue(QStringLiteral("StatusBarShow"), _nppGUI._statusBarShow);
    settings.setValue(QStringLiteral("MenuBarShow"), _nppGUI._menuBarShow);
    settings.setValue(QStringLiteral("TabStatus"), _nppGUI._tabStatus);
    settings.setValue(QStringLiteral("SplitterPos"), _nppGUI._splitterPos ? POS_VERTICAL : POS_HORIZONTAL);
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("Editing"));
    settings.setValue(QStringLiteral("TabSize"), _nppGUI._tabSize);
    settings.setValue(QStringLiteral("TabReplacedBySpace"), _nppGUI._tabReplacedBySpace);
    settings.setValue(QStringLiteral("BackspaceUnindent"), _nppGUI._backspaceUnindent);
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("Window"));
    settings.setValue(QStringLiteral("PosLeft"), _nppGUI._appPosLeft);
    settings.setValue(QStringLiteral("PosTop"), _nppGUI._appPosTop);
    settings.setValue(QStringLiteral("PosRight"), _nppGUI._appPosRight);
    settings.setValue(QStringLiteral("PosBottom"), _nppGUI._appPosBottom);
    settings.setValue(QStringLiteral("Maximized"), _nppGUI._isMaximized);
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("NewDoc"));
    settings.setValue(QStringLiteral("Format"), static_cast<int>(_nppGUI._newDocDefaultSettings._format));
    settings.setValue(QStringLiteral("Encoding"), static_cast<int>(_nppGUI._newDocDefaultSettings._unicodeMode));
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("Backup"));
    settings.setValue(QStringLiteral("Backup"), _nppGUI._backup);
    settings.setValue(QStringLiteral("UseDir"), _nppGUI._useDir);
    settings.setValue(QStringLiteral("Dir"), QString::fromStdWString(_nppGUI._backupDir));
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("Theme"));
    settings.setValue(QStringLiteral("ThemeName"), QString::fromStdWString(_nppGUI._themeName));
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("DarkMode"));
    settings.setValue(QStringLiteral("Enabled"), _nppGUI._darkmode._isEnabled);
    settings.setValue(QStringLiteral("EnablePlugins"), _nppGUI._darkmode._isEnabledPlugin);
    settings.setValue(QStringLiteral("ColorTone"), _nppGUI._darkmode._colorTone);
    settings.endGroup();
    
    settings.beginGroup(QStringLiteral("RecentFiles"));
    settings.setValue(QStringLiteral("CheckHistoryFiles"), _nppGUI._checkHistoryFiles);
    settings.setValue(QStringLiteral("PutRecentFileInSubMenu"), _putRecentFileInSubMenu);
    settings.endGroup();
    
    writeScintillaParams();
}

void NppParameters::writeSession(const Session& session, const wchar_t* fileName) const {
    QString sessionPath;
    
    if (fileName) {
        sessionPath = QString::fromStdWString(std::wstring(fileName));
    } else {
        sessionPath = getSettingsPath(sessionFileName);
    }
    
    QFile file(sessionPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("NotepadPlus"));
    xml.writeStartElement(QStringLiteral("Session"));
    
    xml.writeAttribute(QStringLiteral("activeView"), QString::number(session._activeView));
    
    // Write main view files
    xml.writeStartElement(QStringLiteral("MainView"));
    for (const auto& fileInfo : session._mainViewFiles) {
        xml.writeStartElement(QStringLiteral("File"));
        xml.writeAttribute(QStringLiteral("filename"), QString::fromStdWString(fileInfo._fileName));
        xml.writeAttribute(QStringLiteral("lang"), QString::fromStdWString(fileInfo._langName));
        xml.writeAttribute(QStringLiteral("encoding"), QString::number(fileInfo._encoding));
        xml.writeAttribute(QStringLiteral("active"), 
                         fileInfo._isUserReadOnly ? QStringLiteral("yes") : QStringLiteral("no"));
        xml.writeEndElement(); // File
    }
    xml.writeEndElement(); // MainView
    
    // Write sub view files
    xml.writeStartElement(QStringLiteral("SecondaryView"));
    for (const auto& fileInfo : session._subViewFiles) {
        xml.writeStartElement(QStringLiteral("File"));
        xml.writeAttribute(QStringLiteral("filename"), QString::fromStdWString(fileInfo._fileName));
        xml.writeAttribute(QStringLiteral("lang"), QString::fromStdWString(fileInfo._langName));
        xml.writeAttribute(QStringLiteral("encoding"), QString::number(fileInfo._encoding));
        xml.writeEndElement(); // File
    }
    xml.writeEndElement(); // SecondaryView
    
    xml.writeEndElement(); // Session
    xml.writeEndElement(); // NotepadPlus
    xml.writeEndDocument();
    
    file.close();
}

bool NppParameters::writeFindHistory() {
    QString historyPath = getSettingsPath(foundHistoryFileName);
    QFile file(historyPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("FindHistory"));
    
    xml.writeAttribute(QStringLiteral("matchWord"), _findHistory._isMatchWord ? QStringLiteral("1") : QStringLiteral("0"));
    xml.writeAttribute(QStringLiteral("matchCase"), _findHistory._isMatchCase ? QStringLiteral("1") : QStringLiteral("0"));
    xml.writeAttribute(QStringLiteral("wrap"), _findHistory._isWrap ? QStringLiteral("1") : QStringLiteral("0"));
    xml.writeAttribute(QStringLiteral("directionDown"), _findHistory._isDirectionDown ? QStringLiteral("1") : QStringLiteral("0"));
    
    // Write find history entries
    xml.writeStartElement(QStringLiteral("FindWhats"));
    for (const auto& find : _findHistory._findHistoryFinds) {
        xml.writeTextElement(QStringLiteral("Find"), QString::fromStdWString(find));
    }
    xml.writeEndElement(); // FindWhats
    
    xml.writeStartElement(QStringLiteral("ReplaceWiths"));
    for (const auto& replace : _findHistory._findHistoryReplaces) {
        xml.writeTextElement(QStringLiteral("Replace"), QString::fromStdWString(replace));
    }
    xml.writeEndElement(); // ReplaceWiths
    
    xml.writeEndElement(); // FindHistory
    xml.writeEndDocument();
    
    file.close();
    return true;
}

void NppParameters::writeShortcuts() {
    QString shortcutsPath = getSettingsPath(shortcutsFileName);
    QFile file(shortcutsPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("NotepadPlus"));
    xml.writeStartElement(QStringLiteral("RegisteredCommands"));
    
    // Write registered commands
    for (const auto& shortcut : _shortcuts) {
        xml.writeStartElement(QStringLiteral("Command"));
        // Write shortcut attributes
        xml.writeEndElement();
    }
    
    xml.writeEndElement(); // RegisteredCommands
    xml.writeEndElement(); // NotepadPlus
    xml.writeEndDocument();
    
    file.close();
}

void NppParameters::destroyInstance() {
    // Write all settings before destruction
    writeScintillaParams();
    createXmlTreeFromGUIParams();
    writeFindHistory();
    writeShortcuts();
}

// Get font list from system
void NppParameters::setFontList(QWidget* hWnd) {
    Q_UNUSED(hWnd);
    
    // Get system font list
    QFontDatabase fonts;
    QStringList fontFamilies = fonts.families();
    
    _fontlist.clear();
    for (const QString& family : fontFamilies) {
        _fontlist.push_back(family.toStdString());
    }
    
    // Sort alphabetically
    std::sort(_fontlist.begin(), _fontlist.end());
}

bool NppParameters::isInFontList(const std::wstring& fontName2Search) const {
    return std::find(_fontlist.begin(), _fontlist.end(), fontName2Search) != _fontlist.end();
}

COLORREF NppParameters::getCurLineHilitingColour() {
    // Return default caret line highlight color
    return 0xFFFF00; // Yellow
}

void NppParameters::setCurLineHilitingColour(COLORREF colour2Set) {
    // Set caret line highlight color
    Q_UNUSED(colour2Set);
}

// Helper function implementations
Style& StyleArray::getStyler(size_t index) {
    if (index >= _styleVect.size()) {
        throw std::out_of_range("Styler index out of range");
    }
    return _styleVect[index];
}

void StyleArray::addStyler(int styleID, const std::wstring& styleName) {
    _styleVect.emplace_back();
    Style& s = _styleVect.back();
    s._styleID = styleID;
    s._styleDesc = styleName;
    s._fgColor = black;
    s._bgColor = white;
}

Style* StyleArray::findByID(int id) {
    auto it = std::find_if(_styleVect.begin(), _styleVect.end(),
        [&id](const Style& s) { return s._styleID == id; });
    return (it != _styleVect.end()) ? &(*it) : nullptr;
}

Style* StyleArray::findByName(const std::wstring& name) {
    auto it = std::find_if(_styleVect.begin(), _styleVect.end(),
        [&name](const Style& s) { return s._styleDesc == name; });
    return (it != _styleVect.end()) ? &(*it) : nullptr;
}

LexerStyler& LexerStylerArray::getLexerFromIndex(size_t index) {
    assert(index < _lexerStylerVect.size());
    return _lexerStylerVect[index];
}

const wchar_t* LexerStylerArray::getLexerNameFromIndex(size_t index) const {
    return _lexerStylerVect[index].getLexerName();
}

const wchar_t* LexerStylerArray::getLexerDescFromIndex(size_t index) const {
    return _lexerStylerVect[index].getLexerDesc();
}

LexerStyler* LexerStylerArray::getLexerStylerByName(const wchar_t* lexerName) {
    if (!lexerName) return nullptr;
    auto it = std::find_if(_lexerStylerVect.begin(), _lexerStylerVect.end(),
        [lexerName](const LexerStyler& ls) {
            return wcscmp(ls.getLexerName(), lexerName) == 0;
        });
    return (it != _lexerStylerVect.end()) ? &(*it) : nullptr;
}

int ScintillaViewParams::getDistractionFreePadding(int editViewWidth) const {
    const int defaultDiviser = 4;
    int diviser = _distractionFreeDivPart > 2 ? _distractionFreeDivPart : defaultDiviser;
    int paddingLen = editViewWidth / diviser;
    if (paddingLen <= 0) {
        paddingLen = editViewWidth / defaultDiviser;
    }
    return paddingLen;
}
