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

// Qt6 Translation - Parameters.h
// Translated from Win32 Registry to QSettings

#pragma once

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Forward declarations
class NativeLangSpeaker;
class NppXml;

// Color types (matching original)
using COLORREF = unsigned long;
constexpr COLORREF black = 0x000000;
constexpr COLORREF white = 0xFFFFFF;
constexpr COLORREF STYLE_NOT_USED = 0x1000000;

// Forward declarations for lexer/Scintilla
struct Style;
struct StyleArray;
struct LexerStyler;
struct LexerStylerArray;
struct GlobalOverride;
class Accelerator;
class ScintillaAccelerator;
struct ScintillaViewParams;

// EOL types
enum class EolType {
    osdefault = -1,
    windows = 0,  // CR LF
    unix = 1,     // LF
    mac = 2,      // CR
    unknown = 3
};

// Unicode modes
enum class UniMode {
    uni8Bit = 0,
    uniUTF8 = 1,
    uni16BE = 2,
    uni16LE = 3,
    uniCookie = 4,
    uniUTF8Bom = 5,
    uniEnd = 6
};

// Language types
enum class LangType {
    L_TEXT = 0,
    L_PHP = 1,
    L_C = 2,
    L_CPP = 3,
    L_CS = 4,
    L_OBJC = 5,
    L_JAVA = 6,
    L_RC = 7,
    L_HTML = 8,
    L_XML = 9,
    L_MAKEFILE = 10,
    L_PASCAL = 11,
    L_BATCH = 12,
    L_INI = 13,
    L_NFO = 14,
    L_USER = 15,
    L_ASP = 16,
    L_SQL = 17,
    L_PYTHON = 18,
    L_PERL = 19,
    L_JS = 20,
    L_USER2 = 21,
    L_USER3 = 22,
    L_USER4 = 23,
    L_FORTRAN = 24,
    L_BASH = 25,
    L_FLASH = 26,
    L_NSIS = 27,
    L_CAML = 28,
    L_ADA = 29,
    L_VERILOG = 30,
    L_MATLAB = 31,
    L_HASKELL = 32,
    L_INNO = 33,
    L_SEARCHRESULT = 34,
    L_CMAKE = 35,
    L_YAML = 36,
    L_COBOL = 37,
    L_GUI4CLI = 38,
    L_D = 39,
    L_POWERSHELL = 40,
    L_R = 41,
    L_JSP = 42,
    L_EXTERNAL = 100,
    L_EXTERNAL_LANG = 200
};

// Position structure
struct Position {
    intptr_t _firstVisibleLine = 0;
    intptr_t _startPos = 0;
    intptr_t _endPos = 0;
    intptr_t _xOffset = 0;
    intptr_t _selMode = 0;
    intptr_t _scrollWidth = 1;
    intptr_t _offset = 0;
    intptr_t _wrapCount = 0;
};

// Session file info
struct sessionFileInfo : public Position {
    sessionFileInfo() = default;
    sessionFileInfo(const wchar_t* fn, const wchar_t* ln, int encoding, 
                   bool userReadOnly, bool isPinned, bool isUntitleTabRenamed,
                   const Position& pos, const wchar_t* backupFilePath,
                   uint64_t originalFileLastModifTimestamp,
                   void* mapPos) noexcept;
    
    explicit sessionFileInfo(const std::wstring& fn) noexcept;
    
    std::wstring _fileName;
    std::wstring _langName;
    std::vector<size_t> _marks;
    std::vector<size_t> _foldStates;
    int _encoding = -1;
    bool _isUserReadOnly = false;
    bool _isMonitoring = false;
    int _individualTabColour = -1;
    bool _isRTL = false;
    bool _isPinned = false;
    bool _isUntitledTabRenamed = false;
    std::wstring _backupFilePath;
    uint64_t _originalFileLastModifTimestamp = 0;
    void* _mapPos = nullptr;
};

// Session structure
struct Session {
    size_t nbMainFiles() const { return _mainViewFiles.size(); }
    size_t nbSubFiles() const { return _subViewFiles.size(); }
    
    size_t _activeView = 0;
    size_t _activeMainIndex = 0;
    size_t _activeSubIndex = 0;
    bool _includeFileBrowser = false;
    std::wstring _fileBrowserSelectedItem;
    std::vector<sessionFileInfo> _mainViewFiles;
    std::vector<sessionFileInfo> _subViewFiles;
    std::vector<std::wstring> _fileBrowserRoots;
};

// Command line parameters
struct CmdLineParams {
    bool _isNoPlugin = false;
    bool _isReadOnly = false;
    bool _isFullReadOnly = false;
    bool _isFullReadOnlySavingForbidden = false;
    bool _isNoSession = false;
    bool _isNoTab = false;
    bool _isPreLaunch = false;
    bool _showLoadingTime = false;
    bool _alwaysOnTop = false;
    bool _displayCmdLineArgs = false;
    intptr_t _line2go = -1;
    intptr_t _column2go = -1;
    intptr_t _pos2go = -1;
    int _pointX = 0;
    int _pointY = 0;
    bool _isPointXValid = false;
    bool _isPointYValid = false;
    bool _isSessionFile = false;
    bool _isRecursive = false;
    bool _openFoldersAsWorkspace = false;
    bool _monitorFiles = false;
    int _langType = 0;
    std::wstring _localizationPath;
    std::wstring _udlName;
    std::wstring _pluginMessage;
    std::wstring _easterEggName;
    unsigned char _quoteType = 0;
    int _ghostTypingSpeed = -1;
    
    bool isPointValid() const {
        return _isPointXValid && _isPointYValid;
    }
};

// Style structure
struct Style {
    int _styleID = STYLE_NOT_USED;
    std::wstring _styleDesc;
    COLORREF _fgColor = STYLE_NOT_USED;
    COLORREF _bgColor = STYLE_NOT_USED;
    int _colorStyle = 0;
    bool _isFontEnabled = false;
    std::wstring _fontName;
    int _fontStyle = STYLE_NOT_USED;
    int _fontSize = STYLE_NOT_USED;
    int _nesting = 0;
    int _keywordClass = STYLE_NOT_USED;
    std::string _keywords;
};

// Global override
struct GlobalOverride {
    bool enableFg = false;
    bool enableBg = false;
    bool enableFont = false;
    bool enableFontSize = false;
    bool enableBold = false;
    bool enableItalic = false;
    bool enableUnderLine = false;
    
    bool isEnable() const {
        return enableFg || enableBg || enableFont || enableFontSize || 
               enableBold || enableItalic || enableUnderLine;
    }
};

// Style array
struct StyleArray {
    auto begin() const { return _styleVect.begin(); }
    auto end() const { return _styleVect.end(); }
    auto begin() { return _styleVect.begin(); }
    auto end() { return _styleVect.end(); }
    void clear() { _styleVect.clear(); }
    
    Style& getStyler(size_t index);
    void addStyler(int styleID, const std::wstring& styleName);
    Style* findByID(int id);
    Style* findByName(const std::wstring& name);
    
protected:
    std::vector<Style> _styleVect;
};

// Lexer styler
struct LexerStyler : public StyleArray {
    void setLexerName(const wchar_t* lexerName);
    void setLexerDesc(const wchar_t* lexerDesc);
    void setLexerUserExt(const wchar_t* lexerUserExt);
    const wchar_t* getLexerName() const { return _lexerName.c_str(); }
    const wchar_t* getLexerDesc() const { return _lexerDesc.c_str(); }
    const wchar_t* getLexerUserExt() const { return _lexerUserExt.c_str(); }
    
private:
    std::wstring _lexerName;
    std::wstring _lexerDesc;
    std::wstring _lexerUserExt;
};

// Lexer styler array
struct LexerStylerArray {
    size_t getNbLexer() const { return _lexerStylerVect.size(); }
    void clear() { _lexerStylerVect.clear(); }
    LexerStyler& getLexerFromIndex(size_t index);
    const wchar_t* getLexerNameFromIndex(size_t index) const;
    const wchar_t* getLexerDescFromIndex(size_t index) const;
    LexerStyler* getLexerStylerByName(const wchar_t* lexerName);
    void addLexerStyler(const char* lexerName, const char* lexerDesc, 
                        const char* lexerUserExt);
    void sort();
    
private:
    std::vector<LexerStyler> _lexerStylerVect;
};

// New document default settings
struct NewDocDefaultSettings {
    EolType _format = EolType::osdefault;
    UniMode _unicodeMode = UniMode::uniUTF8;
    bool _openAnsiAsUtf8 = true;
    int _lang = 0;
    int _codepage = -1;
    bool _addNewDocumentOnStartup = false;
    bool _useContentAsTabName = false;
};

// Language menu item
struct LangMenuItem {
    int _langType = 0;
    int _cmdID = -1;
    std::wstring _langName;
};

// Print settings
struct PrintSettings {
    bool _printLineNumber = true;
    int _printOption = 0;
    std::wstring _headerLeft;
    std::wstring _headerMiddle;
    std::wstring _headerRight;
    std::wstring _headerFontName;
    int _headerFontStyle = 0;
    int _headerFontSize = 0;
    std::wstring _footerLeft;
    std::wstring _footerMiddle;
    std::wstring _footerRight;
    std::wstring _footerFontName;
    int _footerFontStyle = 0;
    int _footerFontSize = 0;
    int _margeLeft = 0;
    int _margeTop = 0;
    int _margeRight = 0;
    int _margeBottom = 0;
    
    bool isHeaderPresent() const;
    bool isFooterPresent() const;
};

// Dark mode colors
struct DarkModeColors {
    COLORREF background = 0;
    COLORREF softerBackground = 0;
    COLORREF hotBackground = 0;
    COLORREF pureBackground = 0;
    COLORREF errorBackground = 0;
    COLORREF text = 0;
    COLORREF darkerText = 0;
    COLORREF disabledText = 0;
    COLORREF linkText = 0;
    COLORREF edge = 0;
    COLORREF hotEdge = 0;
    COLORREF disabledEdge = 0;
};

// Toolbar icon info
struct TbIconInfo {
    int _tbIconSet = 0;
    COLORREF _tbCustomColor = 0;
    bool _tbUseMono = false;
};

// Advanced options
struct AdvancedOptions {
    std::wstring _darkDefaults;
    TbIconInfo _darkTbIconInfo;
    int _darkTabIconSet = 2;
    bool _darkTabUseTheme = false;
    
    std::wstring _lightDefaults;
    TbIconInfo _lightTbIconInfo;
    int _lightTabIconSet = 0;
    bool _lightTabUseTheme = true;
    
    bool _enableWindowsMode = false;
};

// Dark mode configuration
struct DarkModeConf {
    bool _isEnabled = false;
    bool _isEnabledPlugin = true;
    int _colorTone = 0;
    DarkModeColors _customColors;
    AdvancedOptions _advOptions;
};

// Large file restriction
struct LargeFileRestriction {
    int64_t _largeFileSizeDefInByte = 0;
    bool _isEnabled = true;
    bool _deactivateWordWrap = true;
    bool _allowBraceMatch = false;
    bool _allowAutoCompletion = false;
    bool _allowSmartHilite = false;
    bool _allowClickableLink = false;
    bool _suppress2GBWarning = false;
};

// NppGUI - GUI configuration
struct NppGUI {
    TbIconInfo _tbIconInfo;
    bool _toolbarShow = true;
    bool _statusBarShow = true;
    bool _menuBarShow = true;
    
    int _tabStatus = 0;
    bool _forceTabbarVisible = false;
    int _tabCompactLabelLen = 0;
    
    bool _splitterPos = false;
    int _userDefineDlgStatus = 0;
    
    int _tabSize = 4;
    bool _tabReplacedBySpace = false;
    bool _backspaceUnindent = false;
    
    bool _finderLinesAreCurrentlyWrapped = false;
    bool _finderPurgeBeforeEverySearch = false;
    bool _finderShowOnlyOneEntryPerFoundLine = true;
    bool _fif_ignoreunsavedChangesInOpenedFiles = false;
    
    int _fileAutoDetection = 0;
    bool _checkHistoryFiles = false;
    
    int _appPosLeft = 0;
    int _appPosTop = 0;
    int _appPosRight = 1024;
    int _appPosBottom = 700;
    
    int _findWindowPosLeft = 0;
    int _findWindowPosTop = 0;
    int _findWindowPosRight = 0;
    int _findWindowPosBottom = 0;
    bool _findWindowLessMode = false;
    
    bool _isMaximized = false;
    int _isMinimizedToTray = 0;
    bool _rememberLastSession = true;
    bool _keepSessionAbsentFileEntries = false;
    bool _isCmdlineNosessionActivated = false;
    bool _isFullReadOnly = false;
    bool _isFullReadOnlySavingForbidden = false;
    bool _detectEncoding = true;
    bool _saveAllConfirm = true;
    bool _setSaveDlgExtFiltToAllTypes = false;
    bool _doTaskList = true;
    int _maintainIndent = 0;
    bool _enableSmartHilite = true;
    
    bool _smartHiliteCaseSensitive = false;
    bool _smartHiliteWordOnly = true;
    bool _smartHiliteUseFindSettings = false;
    bool _smartHiliteOnAnotherView = false;
    
    bool _markAllCaseSensitive = false;
    bool _markAllWordOnly = true;
    
    bool _disableSmartHiliteTmp = false;
    bool _enableTagsMatchHilite = true;
    bool _enableTagAttrsHilite = true;
    bool _enableHiliteNonHTMLZone = false;
    bool _styleMRU = true;
    char _leftmostDelimiter = '(';
    char _rightmostDelimiter = ')';
    bool _delimiterSelectionOnEntireDocument = false;
    bool _backSlashIsEscapeCharacterForSql = true;
    bool _fillFindFieldWithSelected = true;
    bool _fillFindFieldSelectCaret = true;
    bool _monospacedFontFindDlg = false;
    bool _findDlgAlwaysVisible = false;
    bool _confirmReplaceInAllOpenDocs = true;
    bool _replaceStopsWithoutFindingNext = false;
    int _inSelectionAutocheckThreshold = 0;
    int _fillFindWhatThreshold = 0;
    bool _fillDirFieldFromActiveDoc = false;
    bool _muteSounds = false;
    bool _enableFoldCmdToggable = false;
    bool _hideMenuRightShortcuts = false;
    int _writeTechnologyEngine = 0;
    bool _isWordCharDefault = true;
    std::string _customWordChars;
    int _styleURL = 0;
    std::wstring _uriSchemes;
    
    NewDocDefaultSettings _newDocDefaultSettings;
    std::wstring _dateTimeFormat;
    bool _dateTimeReverseDefaultOrder = false;
    
    void setTabReplacedBySpace(bool b) { _tabReplacedBySpace = b; }
    const NewDocDefaultSettings& getNewDocDefaultSettings() const { return _newDocDefaultSettings; }
    
    std::vector<LangMenuItem> _excludedLangList;
    bool _isLangMenuCompact = true;
    
    PrintSettings _printSettings;
    int _backup = 0;
    bool _useDir = false;
    std::wstring _backupDir;
    void* _dockingData = nullptr;  // DockingManagerData
    GlobalOverride _globalOverride;
    int _autocStatus = 0;
    unsigned int _autocFromLen = 1;
    bool _autocIgnoreNumbers = true;
    bool _autocInsertSelectedUseENTER = true;
    bool _autocInsertSelectedUseTAB = true;
    bool _autocBrief = false;
    bool _funcParams = true;
    void* _matchedPairConf = nullptr;  // MatchedPairConf
    
    std::wstring _definedSessionExt;
    std::wstring _definedWorkspaceExt;
    std::wstring _commandLineInterpreter;
    
    struct AutoUpdateOptions {
        int _doAutoUpdate = 0;
        int _intervalDays = 15;
        void* _nextUpdateDate = nullptr;
    } _autoUpdateOpt;
    
    bool _doesExistUpdater = false;
    int _caretBlinkRate = 600;
    int _caretWidth = 1;
    
    bool _shortTitlebar = false;
    
    int _openSaveDir = 0;
    wchar_t _defaultDir[260] = {};
    wchar_t _defaultDirExp[260] = {};
    wchar_t _lastUsedDir[260] = {};
    
    std::wstring _themeName;
    int _multiInstSetting = 0;
    bool _clipboardHistoryPanelKeepState = false;
    bool _docListKeepState = false;
    bool _charPanelKeepState = false;
    bool _fileBrowserKeepState = false;
    bool _projectPanelKeepState = false;
    bool _docMapKeepState = false;
    bool _funcListKeepState = false;
    bool _pluginPanelKeepState = false;
    bool _fileSwitcherWithoutExtColumn = false;
    int _fileSwitcherExtWidth = 50;
    bool _fileSwitcherWithoutPathColumn = true;
    int _fileSwitcherPathWidth = 50;
    bool _fileSwitcherDisableListViewGroups = false;
    
    bool isSnapshotMode() const { return _isSnapshotMode && _rememberLastSession && !_isCmdlineNosessionActivated; }
    bool _isSnapshotMode = true;
    size_t _snapshotBackupTiming = 7000;
    std::wstring _cloudPath;
    unsigned char _availableClouds = 0;
    
    int _searchEngineChoice = 0;
    std::wstring _searchEngineCustom;
    
    bool _isFolderDroppedOpenFiles = false;
    bool _isDocPeekOnTab = false;
    bool _isDocPeekOnMap = false;
    bool _shouldSortFunctionList = false;
    
    DarkModeConf _darkmode;
    LargeFileRestriction _largeFileRestriction;
};

// Scintilla view parameters
struct ScintillaViewParams {
    bool _lineNumberMarginShow = true;
    bool _lineNumberMarginDynamicWidth = true;
    bool _bookMarkMarginShow = true;
    bool _isChangeHistoryMarginEnabled = true;
    bool _isChangeHistoryIndicatorEnabled = false;
    int _folderStyle = 0;
    int _lineWrapMethod = 0;
    bool _foldMarginShow = true;
    bool _indentGuideLineShow = true;
    int _currentLineHiliteMode = 0;
    unsigned char _currentLineFrameWidth = 1;
    bool _wrapSymbolShow = false;
    bool _doWrap = false;
    bool _isEdgeBgMode = false;
    std::vector<size_t> _edgeMultiColumnPos;
    intptr_t _zoom = 0;
    intptr_t _zoom2 = 0;
    bool _zoomSync = false;
    bool _whiteSpaceShow = false;
    bool _eolShow = false;
    int _eolMode = 0;
    bool _npcShow = false;
    int _npcMode = 0;
    bool _npcCustomColor = false;
    bool _npcIncludeCcUniEol = false;
    bool _ccUniEolShow = true;
    bool _npcNoInputC0 = true;
    int _borderWidth = 2;
    bool _virtualSpace = false;
    bool _scrollBeyondLastLine = true;
    bool _rightClickKeepsSelection = false;
    bool _selectedTextForegroundSingleColor = false;
    bool _disableAdvancedScrolling = false;
    bool _doSmoothFont = false;
    bool _showBorderEdge = true;
    unsigned char _paddingLeft = 0;
    unsigned char _paddingRight = 0;
    unsigned char _distractionFreeDivPart = 4;
    bool _lineCopyCutWithoutSelection = true;
    bool _disableSelectedTextDragDrop = false;
    bool _multiSelection = true;
    bool _columnSel2MultiEdit = true;
    
    int getDistractionFreePadding(int editViewWidth) const;
};

// Language structure
struct Lang {
    int _langID = 0;
    std::wstring _langName;
    std::wstring _defaultExtList;
    std::string _langKeyWordList[15];
    std::string _pCommentLineSymbol;
    std::string _pCommentStart;
    std::string _pCommentEnd;
    int _tabSize = -1;
    bool _isTabReplacedBySpace = false;
    bool _isBackspaceUnindent = false;
    
    void setTabInfo(int tabInfo);
    void setWords(const char* words, int index);
    const char* getWords(int index) const;
    int getTabInfo() const;
    const wchar_t* getDefaultExtList() const;
    int getLangID() const { return _langID; }
    const wchar_t* getLangName() const { return _langName.c_str(); }
};

// User language container
class UserLangContainer {
public:
    UserLangContainer() noexcept;
    explicit UserLangContainer(const wchar_t* name, const wchar_t* ext, 
                                const char* udlVer, bool isDarkModeTheme) noexcept;
    
    const wchar_t* getName() const { return _name.c_str(); }
    const wchar_t* getExtention() const { return _ext.c_str(); }
    const char* getUdlVersion() const { return _udlVersion.c_str(); }
    
private:
    StyleArray _styles;
    std::wstring _name;
    std::wstring _ext;
    std::string _udlVersion;
    std::string _keywordLists[15];
    bool _isPrefix[15] = {false};
    int _forcePureLC = 0;
    int _decimalSeparator = 0;
    bool _isCaseIgnored = false;
    bool _allowFoldOfComments = false;
    bool _foldCompact = false;
    bool _isDarkModeTheme = false;
};

// External language container
class ExternalLangContainer {
public:
    std::string _name;
    void* _fnCL = nullptr;  // Lexilla::CreateLexerFn
    int _autoIndentMode = 0;
};

// Find history
struct FindHistory {
    int _isSearch2ButtonsMode = false;
    int _nbMaxFindHistoryPath = 10;
    int _nbMaxFindHistoryFilter = 10;
    int _nbMaxFindHistoryFind = 10;
    int _nbMaxFindHistoryReplace = 10;
    
    std::vector<std::wstring> _findHistoryPaths;
    std::vector<std::wstring> _findHistoryFilters;
    std::vector<std::wstring> _findHistoryFinds;
    std::vector<std::wstring> _findHistoryReplaces;
    
    bool _isMatchWord = false;
    bool _isMatchCase = false;
    bool _isWrap = true;
    bool _isDirectionDown = true;
    bool _dotMatchesNewline = false;
    bool _isFifRecursive = true;
    bool _isFifInHiddenFolder = false;
    bool _isFifProjectPanel_1 = false;
    bool _isFifProjectPanel_2 = false;
    bool _isFifProjectPanel_3 = false;
    
    int _searchMode = 0;
    int _transparencyMode = 0;
    int _transparency = 150;
    
    bool _isFilterFollowDoc = false;
    bool _isBookmarkLine = false;
    bool _isPurge = false;
    bool _regexBackward4PowerUser = false;
};

// Column editor parameters
struct ColumnEditorParam {
    enum class LeadingChoice : unsigned char { noneLeading, zeroLeading, spaceLeading };
    enum class NumBase : unsigned char { BASE_10 = 0, BASE_16 = 1, BASE_8 = 2, BASE_2 = 3, BASE_16_UPPER = 4 };
    
    bool _mainChoice = false;
    std::wstring _insertedTextContent;
    int _initialNum = -1;
    int _increaseNum = -1;
    int _repeatNum = -1;
    NumBase _formatChoice = NumBase::BASE_10;
    LeadingChoice _leadingChoice = LeadingChoice::noneLeading;
};

// Localization switcher
class LocalizationSwitcher {
public:
    bool addLanguageFromXml(const std::wstring& xmlFullPath);
    std::wstring getLangFromXmlFileName(const wchar_t* fn) const;
    std::wstring getXmlFilePathFromLangName(const wchar_t* langName) const;
    bool switchToLang(const wchar_t* lang2switch) const;
    size_t size() const { return _localizationList.size(); }
    std::pair<std::wstring, std::wstring> getElementFromIndex(size_t index) const;
    void setFileName(const char* fn);
    const std::string& getFileName() const { return _fileName; }
    
private:
    std::vector<std::pair<std::wstring, std::wstring>> _localizationList;
    std::wstring _nativeLangPath;
    std::string _fileName;
};

// Theme switcher
class ThemeSwitcher {
public:
    void addThemeFromXml(const std::wstring& xmlFullPath);
    void addDefaultThemeFromXml(const std::wstring& xmlFullPath);
    std::wstring getThemeFromXmlFileName(const wchar_t* xmlFullPath) const;
    std::wstring getXmlFilePathFromThemeName(const wchar_t* themeName) const;
    bool themeNameExists(const wchar_t* themeName);
    size_t size() const { return _themeList.size(); }
    std::pair<std::wstring, std::wstring>& getElementFromIndex(size_t index);
    void setThemeDirPath(const std::wstring& themeDirPath);
    const std::wstring& getThemeDirPath() const { return _themeDirPath; }
    const std::wstring& getDefaultThemeLabel() const { return _defaultThemeLabel; }
    std::wstring getSavePathFrom(const std::wstring& path) const;
    void addThemeStylerSavePath(const std::wstring& key, const std::wstring& val);
    
private:
    std::vector<std::pair<std::wstring, std::wstring>> _themeList;
    std::map<std::wstring, std::wstring> _themeStylerSavePath;
    std::wstring _themeDirPath;
    const std::wstring _defaultThemeLabel = L"Default (stylers.xml)";
    std::wstring _stylesXmlPath;
};

// HLS Color
struct HLSColour {
    unsigned short _hue = 0;
    unsigned short _lightness = 0;
    unsigned short _saturation = 0;
    
    HLSColour() = default;
    HLSColour(unsigned short hue, unsigned short lightness, unsigned short saturation);
    explicit HLSColour(COLORREF rgb) noexcept;
    
    void loadFromRGB(COLORREF rgb);
    COLORREF toRGB() const;
};

// Dynamic menu
class DynamicMenu {
public:
    bool attach(QMenu* hMenu, unsigned int posBase, int lastCmd, const std::wstring& lastCmdLabel);
    bool createMenu() const;
    bool clearMenu() const;
    int getTopLevelItemNumber() const;
    void push_back(const void* m);
    void emplace_back(unsigned long cmdID, const std::wstring& itemName, 
                      const std::wstring& parentFolderName);
    unsigned int getPosBase() const { return _posBase; }
    const std::wstring& getLastCmdLabel() const { return _lastCmdLabel; }
    
private:
    std::vector<void*> _menuItems;
    QMenu* _hMenu = nullptr;
    unsigned int _posBase = 0;
    int _lastCmd = 0;
    std::wstring _lastCmdLabel;
};

// NppParameters singleton
class NppParameters {
private:
    static NppParameters* getInstancePointer() {
        static NppParameters* instance = new NppParameters;
        return instance;
    }
    
    struct XmlDocPath {
        void* _doc = nullptr;  // NppXml::Document
        std::wstring _path;
    };
    
public:
    static NppParameters& getInstance() {
        return *getInstancePointer();
    }
    
    static int getLangIDFromStr(const wchar_t* langName);
    static std::wstring getLocPathFromStr(const std::wstring& localizationCode);
    
    bool load();
    bool reloadLang();
    bool reloadStylers(const wchar_t* stylePath = nullptr);
    void destroyInstance();
    std::wstring getSettingsFolder() const;
    
    bool _isTaskListRBUTTONUP_Active = false;
    int L_END = 0;
    
    NppGUI& getNppGUI() { return _nppGUI; }
    const ScintillaViewParams& getSVP() const { return _svp; }
    const char* getWordList(int langID, int typeIndex) const;
    
    Lang* getLangFromID(int langID) const;
    Lang* getLangFromIndex(size_t i) const;
    int getNbLang() const { return _nbLang; }
    int getLangFromExt(const wchar_t* ext);
    const wchar_t* getLangExtFromName(const wchar_t* langName) const;
    const wchar_t* getLangExtFromLangType(int langType) const;
    
    int getNbLRFile() const { return _nbRecentFile; }
    std::wstring* getLRFile(int index) const;
    void setNbMaxRecentFile(unsigned int nb);
    unsigned int getNbMaxRecentFile() const { return _nbMaxRecentFile; }
    void setPutRecentFileInSubMenu(bool doSubmenu);
    bool putRecentFileInSubMenu() const { return _putRecentFileInSubMenu; }
    void setRecentFileCustomLength(int len);
    int getRecentFileCustomLength() const { return _recentFileCustomLength; }
    
    bool writeRecentFileHistorySettings(int nbMaxFile = -1);
    bool writeHistory(const wchar_t* fullpath);
    bool writeProjectPanelsSettings();
    bool writeColumnEditorSettings();
    bool writeFileBrowserSettings(const std::vector<std::wstring>& rootPaths, 
                                  const std::wstring& latestSelectedItemPath);
    
    bool writeScintillaParams();
    void createXmlTreeFromGUIParams();
    std::wstring writeStyles(LexerStylerArray& lexersStylers, StyleArray& globalStylers);
    bool insertTabInfo(const wchar_t* langName, int tabInfo, bool backspaceUnindent);
    
    LexerStylerArray& getLStylerArray() { return _lexerStylerVect; }
    StyleArray& getGlobalStylers() { return _widgetStyleArray; }
    StyleArray& getMiscStylerArray() { return _widgetStyleArray; }
    GlobalOverride& getGlobalOverrideStyle() { return _nppGUI._globalOverride; }
    
    COLORREF getCurLineHilitingColour();
    void setCurLineHilitingColour(COLORREF colour2Set);
    
    void setFontList(QWidget* hWnd);
    bool isInFontList(const std::wstring& fontName2Search) const;
    const std::vector<std::wstring>& getFontList() const { return _fontlist; }
    
    int getNbUserLang() const { return _nbUserLang; }
    UserLangContainer* getULCFromIndex(size_t i);
    const UserLangContainer* getULCFromName(const wchar_t* userLangName) const;
    
    int getNbExternalLang() const { return _nbExternalLang; }
    ExternalLangContainer* getELCFromIndex(int i);
    bool ExternalLangHasRoom() const { return _nbExternalLang < 64; }
    
    void getExternalLexerFromXmlTree(void* externalLexerDoc);
    std::vector<XmlDocPath>* getExternalLexerDoc();
    
    void writeDefaultUDL();
    void writeNonDefaultUDL();
    void writeNeed2SaveUDL();
    void writeShortcuts();
    void writeSession(const Session& session, const wchar_t* fileName = nullptr) const;
    bool writeFindHistory();
    
    bool isExistingUserLangName(const wchar_t* newName) const;
    const wchar_t* getUserDefinedLangNameFromExt(const wchar_t* ext, 
                                                   const wchar_t* fullName) const;
    int addUserLangToEnd(const UserLangContainer* userLang, const wchar_t* newName);
    void removeUserLang(size_t index);
    bool isExistingExternalLangName(const char* newName) const;
    int addExternalLangToEnd(std::unique_ptr<ExternalLangContainer> externalLang);
    
    void* getNativeLang() const { return _pXmlNativeLangDoc; }
    void* getCustomizedToolButtons() const { return _pXmlToolButtonsConfDoc; }
    
    bool isTransparentAvailable() const;
    static void setTransparent(QWidget* hwnd, int percent);
    static void removeTransparent(QWidget* hwnd);
    
    void setCmdlineParam(const void* cmdLineParams);
    const void* getCmdLineParams() const { return &_cmdLineParams; }
    const std::wstring& getCmdLineString() const { return _cmdLineString; }
    void setCmdLineString(const std::wstring& str);
    
    void setFileSaveDlgFilterIndex(int ln);
    int getFileSaveDlgFilterIndex() const { return _fileSaveDlgFilterIndex; }
    
    bool isRemappingShortcut() const { return !_shortcuts.empty(); }
    std::vector<void*>& getUserShortcuts() { return _shortcuts; }
    void addUserModifiedIndex(size_t index);
