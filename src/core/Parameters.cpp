// Parameters.cpp - Settings manager implementation for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#include "Parameters.h"
#include "Application.h"
#include "SessionManager.h"
#include "LanguageManager.h"
#include "common/FileHelper.h"
#include "common/StringHelper.h"
#include "common/Types.h"
#include <QCoreApplication>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <fstream>

// ============================================================================
// Static singleton instance
// ============================================================================
static NppParameters* s_instance = nullptr;

NppParameters& NppParameters::getInstance() {
    if (!s_instance) {
        s_instance = new NppParameters(qApp);
    }
    return *s_instance;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
NppParameters::NppParameters(QObject* parent)
    : QObject(parent)
    , _settings(std::make_unique<QSettings>("notepad--qt", "config"))
{
}

NppParameters::~NppParameters() = default;

// ============================================================================
// Public API
// ============================================================================
void NppParameters::load() {
    qDebug("[NppParameters] Loading settings...");
    loadSettings();
    loadNppGUI();
    loadScintillaViewParams();
    loadFindHistory();
    loadRecentFiles();
    loadShortcuts();
    loadThemes();
    loadSearchPaths();
    qDebug("[NppParameters] Settings loaded.");
}

void NppParameters::save() {
    qDebug("[NppParameters] Saving settings...");
    saveSettings();
    saveNppGUI();
    saveScintillaViewParams();
    saveFindHistory();
    saveRecentFiles();
    saveShortcuts();
    qDebug("[NppParameters] Settings saved.");
}

void NppParameters::resetToDefaults() {
    _nppGUI = NppGUI();
    _scintillaViewParams = ScintillaViewParams();
    _findHistory = FindHistory();
    _themes.clear();
    _shortcuts.clear();
    _maxRecentFiles = 10;
    _language = "en";
    _themeName = "default";
    _settings->clear();
    save();
}

// ============================================================================
// Language
// ============================================================================
void NppParameters::setLanguage(const QString& lang) {
    _language = lang;
    _settings->setValue("General/Language", lang);
    emit languageChanged(lang);
}

void NppParameters::reloadLang() {
    emit languageChanged(_language);
}

// ============================================================================
// Themes
// ============================================================================
void NppParameters::setTheme(const QString& themeName) {
    _themeName = themeName;
    _settings->setValue("Theme/Name", themeName);
    emit themeChanged(themeName);
}

void NppParameters::reloadStylers() {
    loadThemes();
    emit settingsChanged();
}

NppTheme* NppParameters::findTheme(const QString& name) {
    for (auto& theme : _themes) {
        if (theme.name == name) return &theme;
    }
    return nullptr;
}

bool NppParameters::writeStyles() {
    if (_themeName.isEmpty()) return false;
    NppTheme* theme = findTheme(_themeName);
    if (!theme) return false;

    QString path = getStylersThemePath();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("NotepadPlus");
    xml.writeStartElement("LexerStyles");
    xml.writeAttribute("ThemeName", theme->name);

    for (const auto& style : theme->styles) {
        xml.writeStartElement("LexerType");
        xml.writeAttribute("name", style.name);
        xml.writeAttribute("fgColor", QString::number(style.fgColor, 16));
        xml.writeAttribute("bgColor", QString::number(style.bgColor, 16));
        xml.writeAttribute("fontStyle", QString::number(style.fontStyle));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();
    return true;
}

// ============================================================================
// Shortcuts
// ============================================================================
void NppParameters::setShortcuts(const std::vector<ShortcutDef>& shortcuts) {
    _shortcuts = shortcuts;
    _shortcutIndex.clear();
    for (size_t i = 0; i < shortcuts.size(); ++i) {
        _shortcutIndex[QString::fromStdString(shortcuts[i].name)] = static_cast<int>(i);
    }
    saveShortcuts();
}

ShortcutDef* NppParameters::findShortcut(const QString& name) {
    auto it = _shortcutIndex.find(name);
    if (it != _shortcutIndex.end() && it->second < static_cast<int>(_shortcuts.size())) {
        return &_shortcuts[it->second];
    }
    return nullptr;
}

// ============================================================================
// Recent files
// ============================================================================
QStringList NppParameters::getRecentFiles() const {
    return _settings->value("RecentFiles/List", QStringList()).toStringList();
}

void NppParameters::addRecentFile(const QString& filePath) {
    QStringList recent = getRecentFiles();
    recent.removeAll(filePath);
    recent.prepend(filePath);
    while (recent.size() > _maxRecentFiles) {
        recent.removeLast();
    }
    _settings->setValue("RecentFiles/List", recent);
    emit recentFilesChanged();
}

void NppParameters::clearRecentFiles() {
    _settings->setValue("RecentFiles/List", QStringList());
    emit recentFilesChanged();
}

void NppParameters::setMaxRecentFiles(int max) {
    _maxRecentFiles = max;
    _settings->setValue("RecentFiles/Max", max);
}

// ============================================================================
// Paths
// ============================================================================
QString NppParameters::getConfigDir() const {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString NppParameters::getStylersThemePath() const {
    return getConfigDir() + "/themes/" + _themeName + ".xml";
}

QString NppParameters::getUserDefineLangPath() const {
    return _userDefineLangDir.isEmpty()
        ? getConfigDir() + "/userDefineLang"
        : _userDefineLangDir;
}

QString NppParameters::getShortcutsPath() const {
    return getConfigDir() + "/shortcuts.xml";
}

QString NppParameters::getUserDefineLangDir() const {
    if (!_userDefineLangDir.isEmpty()) return _userDefineLangDir;
    return getConfigDir() + "/userDefineLang";
}

void NppParameters::setUserDefineLangDir(const QString& dir) {
    _userDefineLangDir = dir;
    _settings->setValue("Paths/UserDefineLang", dir);
}

QStringList NppParameters::getSearchPaths() const {
    return _searchPaths;
}

void NppParameters::addSearchPath(const QString& path) {
    if (!_searchPaths.contains(path)) {
        _searchPaths.append(path);
        _settings->setValue("Search/Paths", _searchPaths);
    }
}

// ============================================================================
// Private: Load settings groups
// ============================================================================
void NppParameters::loadSettings() {
    _language = _settings->value("General/Language", "en").toString();
    _themeName = _settings->value("Theme/Name", "default").toString();
    _maxRecentFiles = _settings->value("RecentFiles/Max", 10).toInt();
    _userDefineLangDir = _settings->value("Paths/UserDefineLang", "").toString();
    _workingDir = QDir::currentPath();
}

void NppParameters::loadNppGUI() {
    _settings->beginGroup("NppGUI");
    _nppGUI.toolbarShow = _settings->value("toolbarShow", true).toBool();
    _nppGUI.toolbarLines = _settings->value("toolbarLines", 1).toInt();
    _nppGUI.showStatusBar = _settings->value("statusBarShow", true).toBool();
    _nppGUI.showMenuBar = _settings->value("menuBarShow", true).toBool();
    _nppGUI.autoUpdate = _settings->value("autoUpdate", true).toBool();
    _nppGUI.tabBarHide = _settings->value("tabBarHide", false).toBool();
    _nppGUI.tabBarHideShow = _settings->value("tabBarHideShow", false).toBool();
    _nppGUI.multiInst = _settings->value("multiInst", false).toBool();
    _nppGUI.smartHighLight = _settings->value("smartHighLight", true).toBool();
    _nppGUI.highlightToken = _settings->value("highlightToken", true).toBool();
    _nppGUI.matchCase = _settings->value("matchCase", false).toBool();
    _nppGUI.matchWholeWord = _settings->value("matchWholeWord", false).toBool();
    _nppGUI.autoIndent = _settings->value("autoIndent", true).toBool();
    _nppGUI.backspaceUnindents = _settings->value("backspaceUnindents", false).toBool();
    _nppGUI.tabSize = _settings->value("tabSize", 4).toInt();
    _nppGUI.useSpaces = _settings->value("useSpaces", false).toBool();
    _nppGUI.showEol = _settings->value("showEol", false).toBool();
    _nppGUI.showSpaceAndTab = _settings->value("showSpaceAndTab", false).toBool();
    _nppGUI.showWrapSymbol = _settings->value("showWrapSymbol", false).toBool();
    _nppGUI.edgeMode = _settings->value("edgeMode", 0).toInt();
    _nppGUI.edgeColumn = _settings->value("edgeColumn", 80).toInt();
    _nppGUI.wrapMode = _settings->value("wrapMode", false).toBool();
    _nppGUI.folderStyle = _settings->value("folderStyle", 0).toInt();
    _nppGUI.showFileTree = _settings->value("showFileTree", true).toBool();
    _nppGUI.titleBarShort = _settings->value("titleBarShort", false).toBool();
    _nppGUI.titleBarAdd = _settings->value("titleBarAdd", "").toString();
    _settings->endGroup();
}

void NppParameters::loadScintillaViewParams() {
    _settings->beginGroup("ScintillaView");
    _scintillaViewParams.displayeEol = _settings->value("displayeEol", false).toBool();
    _scintillaViewParams.showIndentGuide = _settings->value("showIndentGuide", true).toBool();
    _scintillaViewParams.wrap = _settings->value("wrap", false).toBool();
    _scintillaViewParams.svp._lineNumberMarginShow = _settings->value("lineNumberMarginShow", true).toBool();
    _scintillaViewParams.svp._bookmarkMarginShow = _settings->value("bookmarkMarginShow", true).toBool();
    _scintillaViewParams.svp._folderMarginShow = _settings->value("folderMarginShow", true).toBool();
    _scintillaViewParams.svp._currentLineHilightingShow = _settings->value("currentLineHilightingShow", true).toBool();
    _scintillaViewParams.svp._currentLineFrameShow = _settings->value("currentLineFrameShow", false).toBool();
    _scintillaViewParams.svp._marginFont = _settings->value("marginFont", false).toBool();
    _scintillaViewParams.svp._vrSymbol = _settings->value("vrSymbol", 0).toInt();
    _scintillaViewParams.svp._vpSymbol = _settings->value("vpSymbol", 0).toInt();
    _scintillaViewParams.svp._whitespaceSymbol = _settings->value("whitespaceSymbol", 0).toInt();
    _scintillaViewParams.svp._smoothFont = _settings->value("smoothFont", true).toBool();
    _scintillaViewParams.svp._braceAnalyser = _settings->value("braceAnalyser", 1).toInt();
    _scintillaViewParams.svp._currentLineColor = _settings->value("currentLineColor", 0xFF0000).toInt();
    _scintillaViewParams.svp._highlightCurrentBlock = _settings->value("highlightCurrentBlock", true).toBool();
    _scintillaViewParams.svp._highlightOtherBlock = _settings->value("highlightOtherBlock", true).toBool();
    _scintillaViewParams.svp._eolMode = _settings->value("eolMode", 0).toInt();
    _settings->endGroup();
}

void NppParameters::loadFindHistory() {
    _settings->beginGroup("FindHistory");
    _findHistory._isSearchDialogChecked = _settings->value("isSearchDialogChecked", false).toBool();
    _findHistory._isFilterChecked = _settings->value("isFilterChecked", false).toBool();
    _findHistory._isInSubFolderChecked = _settings->value("isInSubFolderChecked", false).toBool();
    _findHistory._isInHiddenFolderChecked = _settings->value("isInHiddenFolderChecked", false).toBool();
    _findHistory._searchResultSelected = _settings->value("searchResultSelected", false).toBool();
    _findHistory._transparencyMode = _settings->value("transparencyMode", 0).toInt();
    _findHistory._transparency = _settings->value("transparency", 100).toInt();
    _findHistory._dotMatchesNewline = _settings->value("dotMatchesNewline", false).toBool();
    _findHistory._isSearch2ButtonsMode = _settings->value("isSearch2ButtonsMode", false).toBool();
    _findHistory._regexBackward4PowerUser = _settings->value("regexBackward4PowerUser", false).toBool();
    _findHistory._isBookmarkLine = _settings->value("isBookmarkLine", false).toBool();
    _findHistory._matchWord = _settings->value("matchWord", false).toBool();
    _findHistory._matchCase = _settings->value("matchCase", false).toBool();
    _findHistory._searchMode = _settings->value("searchMode", 0).toInt();
    QStringList counters = _settings->value("counters", QStringList()).toStringList();
    if (counters.size() >= 3) {
        _findHistory._counter1 = counters[0].toInt();
        _findHistory._counter2 = counters[1].toInt();
        _findHistory._counter3 = counters[2].toInt();
    }
    _settings->endGroup();
}

void NppParameters::loadRecentFiles() {
    // Recent files loaded via getRecentFiles()
}

void NppParameters::loadShortcuts() {
    QString path = getShortcutsPath();
    if (QFile::exists(path)) {
        loadShortcutsFromXml(path);
    }
}

void NppParameters::loadShortcutsFromXml(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QXmlStreamReader xml(&file);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name().toString() == QStringLiteral("Shortcut")) {
            ShortcutDef sc;
            sc.name = xml.attributes().value("name").toString().toStdString();
            sc.id = xml.attributes().value("id").toString().toInt();
            sc.ctrl = xml.attributes().value("ctrl").toString().toInt();
            sc.alt = xml.attributes().value("alt").toString().toInt();
            sc.shift = xml.attributes().value("shift").toString().toInt();
            sc.key = xml.attributes().value("key").toString().toInt();
            _shortcuts.push_back(sc);
        }
    }
    setShortcuts(_shortcuts);
}

void NppParameters::loadThemes() {
    _themes.clear();
    QString themesDir = getConfigDir() + "/themes";
    QDir dir(themesDir);
    if (!dir.exists()) {
        dir.mkpath(themesDir);
        return;
    }

    for (const QString& fileName : dir.entryList({"*.xml"})) {
        QString fullPath = themesDir + "/" + fileName;
        QFile file(fullPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        NppTheme theme;
        theme.name = fileName.chopped(4); // remove .xml
        theme.path = fullPath;

        QXmlStreamReader xml(&file);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name().toString() == QStringLiteral("LexerType")) {
                NppStyle style;
                style.name = xml.attributes().value("name").toString();
                style.fgColor = xml.attributes().value("fgColor").toInt(nullptr, 16);
                style.bgColor = xml.attributes().value("bgColor").toInt(nullptr, 16);
                style.fontStyle = xml.attributes().value("fontStyle").toInt();
                style.fontSize = xml.attributes().value("fontSize").toInt();
                style.fontName = xml.attributes().value("fontName").toString();
                theme.styles.push_back(style);
            }
        }
        _themes.push_back(theme);
    }
}

void NppParameters::loadSearchPaths() {
    _searchPaths = _settings->value("Search/Paths", QStringList()).toStringList();
}

// ============================================================================
// Private: Save settings groups
// ============================================================================
void NppParameters::saveSettings() {
    _settings->setValue("General/Language", _language);
    _settings->setValue("Theme/Name", _themeName);
    _settings->setValue("RecentFiles/Max", _maxRecentFiles);
    _settings->setValue("Paths/UserDefineLang", _userDefineLangDir);
}

void NppParameters::saveNppGUI() {
    _settings->beginGroup("NppGUI");
    _settings->setValue("toolbarShow", _nppGUI.toolbarShow);
    _settings->setValue("toolbarLines", _nppGUI.toolbarLines);
    _settings->setValue("statusBarShow", _nppGUI.showStatusBar);
    _settings->setValue("menuBarShow", _nppGUI.showMenuBar);
    _settings->setValue("autoUpdate", _nppGUI.autoUpdate);
    _settings->setValue("tabBarHide", _nppGUI.tabBarHide);
    _settings->setValue("tabBarHideShow", _nppGUI.tabBarHideShow);
    _settings->setValue("multiInst", _nppGUI.multiInst);
    _settings->setValue("smartHighLight", _nppGUI.smartHighLight);
    _settings->setValue("highlightToken", _nppGUI.highlightToken);
    _settings->setValue("matchCase", _nppGUI.matchCase);
    _settings->setValue("matchWholeWord", _nppGUI.matchWholeWord);
    _settings->setValue("autoIndent", _nppGUI.autoIndent);
    _settings->setValue("backspaceUnindents", _nppGUI.backspaceUnindents);
    _settings->setValue("tabSize", _nppGUI.tabSize);
    _settings->setValue("useSpaces", _nppGUI.useSpaces);
    _settings->setValue("showEol", _nppGUI.showEol);
    _settings->setValue("showSpaceAndTab", _nppGUI.showSpaceAndTab);
    _settings->setValue("showWrapSymbol", _nppGUI.showWrapSymbol);
    _settings->setValue("edgeMode", _nppGUI.edgeMode);
    _settings->setValue("edgeColumn", _nppGUI.edgeColumn);
    _settings->setValue("wrapMode", _nppGUI.wrapMode);
    _settings->setValue("folderStyle", _nppGUI.folderStyle);
    _settings->setValue("showFileTree", _nppGUI.showFileTree);
    _settings->setValue("titleBarShort", _nppGUI.titleBarShort);
    _settings->setValue("titleBarAdd", _nppGUI.titleBarAdd);
    _settings->endGroup();
}

void NppParameters::saveScintillaViewParams() {
    _settings->beginGroup("ScintillaView");
    _settings->setValue("displayeEol", _scintillaViewParams.displayeEol);
    _settings->setValue("showIndentGuide", _scintillaViewParams.showIndentGuide);
    _settings->setValue("wrap", _scintillaViewParams.wrap);
    _settings->setValue("lineNumberMarginShow", _scintillaViewParams.svp._lineNumberMarginShow);
    _settings->setValue("bookmarkMarginShow", _scintillaViewParams.svp._bookmarkMarginShow);
    _settings->setValue("folderMarginShow", _scintillaViewParams.svp._folderMarginShow);
    _settings->setValue("currentLineHilightingShow", _scintillaViewParams.svp._currentLineHilightingShow);
    _settings->setValue("currentLineFrameShow", _scintillaViewParams.svp._currentLineFrameShow);
    _settings->setValue("marginFont", _scintillaViewParams.svp._marginFont);
    _settings->setValue("vrSymbol", _scintillaViewParams.svp._vrSymbol);
    _settings->setValue("vpSymbol", _scintillaViewParams.svp._vpSymbol);
    _settings->setValue("whitespaceSymbol", _scintillaViewParams.svp._whitespaceSymbol);
    _settings->setValue("smoothFont", _scintillaViewParams.svp._smoothFont);
    _settings->setValue("braceAnalyser", _scintillaViewParams.svp._braceAnalyser);
    _settings->setValue("currentLineColor", _scintillaViewParams.svp._currentLineColor);
    _settings->setValue("highlightCurrentBlock", _scintillaViewParams.svp._highlightCurrentBlock);
    _settings->setValue("highlightOtherBlock", _scintillaViewParams.svp._highlightOtherBlock);
    _settings->setValue("eolMode", _scintillaViewParams.svp._eolMode);
    _settings->endGroup();
}

void NppParameters::saveFindHistory() {
    _settings->beginGroup("FindHistory");
    _settings->setValue("isSearchDialogChecked", _findHistory._isSearchDialogChecked);
    _settings->setValue("isFilterChecked", _findHistory._isFilterChecked);
    _settings->setValue("isInSubFolderChecked", _findHistory._isInSubFolderChecked);
    _settings->setValue("isInHiddenFolderChecked", _findHistory._isInHiddenFolderChecked);
    _settings->setValue("searchResultSelected", _findHistory._searchResultSelected);
    _settings->setValue("transparencyMode", _findHistory._transparencyMode);
    _settings->setValue("transparency", _findHistory._transparency);
    _settings->setValue("dotMatchesNewline", _findHistory._dotMatchesNewline);
    _settings->setValue("isSearch2ButtonsMode", _findHistory._isSearch2ButtonsMode);
    _settings->setValue("regexBackward4PowerUser", _findHistory._regexBackward4PowerUser);
    _settings->setValue("isBookmarkLine", _findHistory._isBookmarkLine);
    _settings->setValue("matchWord", _findHistory._matchWord);
    _settings->setValue("matchCase", _findHistory._matchCase);
    _settings->setValue("searchMode", _findHistory._searchMode);
    _settings->setValue("counters", QStringList() << QString::number(_findHistory._counter1)
                                                  << QString::number(_findHistory._counter2)
                                                  << QString::number(_findHistory._counter3));
    _settings->endGroup();
}

void NppParameters::saveRecentFiles() {
    _settings->setValue("RecentFiles/List", getRecentFiles());
}

// ============================================================================
// Encoding conversion (Qt6-compatible)
// ============================================================================
QString NppParameters::convertEncoding(const QString& text, int fromSciCp, int toSciCp) {
    if (text.isEmpty()) return text;
    if (fromSciCp == toSciCp) return text;

    // QScintilla uses UTF-8 internally in Qt6, so text is already in UTF-8.
    // For simple cases (ASCII, Latin-1 ↔ UTF-8), handle directly.
    if (fromSciCp == 65001 || toSciCp == 65001) {
        // UTF-8 is a no-op (text is already in memory as UTF-8)
        return text;
    }
    if (fromSciCp == 0 && toSciCp == 65001) {
        // Latin-1 → UTF-8
        return QString::fromLatin1(text.toLatin1());
    }
    if (fromSciCp == 65001 && toSciCp == 0) {
        // UTF-8 → Latin-1 (lossy)
        return QString::fromLatin1(text.toUtf8());
    }
    // For other codepages, return as-is
    return text;
}

// ============================================================================
// Font list
// ============================================================================
QStringList NppParameters::getFontList() const {
    return QFontDatabase::families();
}

void NppParameters::saveShortcuts() {
    QString path = getShortcutsPath();
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("NotepadPlus");
    xml.writeStartElement("Shortcuts");

    for (const auto& sc : _shortcuts) {
        xml.writeStartElement("Shortcut");
        xml.writeAttribute("name", QString::fromStdString(sc.name));
        xml.writeAttribute("id", QString::number(sc.id));
        xml.writeAttribute("ctrl", QString::number(sc.ctrl));
        xml.writeAttribute("alt", QString::number(sc.alt));
        xml.writeAttribute("shift", QString::number(sc.shift));
        xml.writeAttribute("key", QString::number(sc.key));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
}
