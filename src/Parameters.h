#ifndef PARAMETERS_H
#define PARAMETERS_H

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

// Qt6 port — all Win32 types remapped via Window.h shims:
//   using HWND = QWidget*;   using HINSTANCE = QApplication*;
//   using HDC = QPaintDevice*;  using RECT = QRect;  using POINT = QPoint;
//   using COLORREF = QRgb;  using FILETIME = NppFileTime;
//   using WPARAM = quintptr;  using LPARAM = qintptr;  using LRESULT = qintptr;

#include "Window.h"
#include "MISC/Common/Common.h"

// Undef Scintilla.h macros that would otherwise poison npp_sci namespace constants
#undef SC_MOD_DELETETEXT
#undef SC_MOD_INSERTTEXT
#undef SC_MOD_CHANGEFIRST
#undef SC_MOD_CHANGELAST
#undef SC_MOD_CHANGEMARKER
#undef SC_MOD_CHANGEFOLD
#undef SC_MOD_PERFORMED_UNDO
#undef SC_MOD_PERFORMED_REDO
#undef SC_MOD_CHANGEINDICATOR
#undef SC_PERFORMED_UNDO
#undef SC_PERFORMED_REDO

#include <QtCore>
#include <QtGui>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <locale>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// NppConstants.h must be BEFORE Scintilla.h — its inline constexpr definitions
// (e.g. INDICATOR_CONTAINER) would be corrupted by Scintilla's #define macros otherwise.
#include "NppConstants.h"
#include "ContextMenu.h"
#include "DockingCont.h"
#include "Notepad_plus_msgs.h"
#include "Buffer.h"
#include "NppDarkMode.h"
using NppDarkMode::TbIconInfo;
#include "MISC/Common/keys.h"
#include "NppXml.h"
#include "ToolBar.h"
#include "colors.h"
#include "WinControls/ImageListSet.h"
#include "shortcut.h"
#include "menuCmdID.h"

// Scintilla headers — use NppSciCompat to avoid macro poisoning of Qsci
#include <Qsci/qsciscintilla.h>
#include "NppSciCompat.h"

// Linux equivalent of %COMSPEC% — use POSIX shell
// Note: This is a Qt6 port, so we use QString for cross-platform strings
static const QString kDefaultShell = QStringLiteral("/bin/sh");
#define CMD_INTERPRETER "/bin/sh"

class NativeLangSpeaker;

// Forward declarations
class NppParameters;
class Accelerator;
class ScintillaAccelerator;

// Cross-platform FILETIME replacement using Qt
struct NppFileTime
{
    uint64_t dwLowDateTime = 0;
    uint64_t dwHighDateTime = 0;

    NppFileTime() = default;
    NppFileTime(uint64_t low, uint64_t high) : dwLowDateTime(low), dwHighDateTime(high) {}
    explicit NppFileTime(const QDateTime& dt);
    QDateTime toQDateTime() const;
    bool isNull() const { return dwLowDateTime == 0 && dwHighDateTime == 0; }
    bool operator==(const NppFileTime& other) const {
        return dwLowDateTime == other.dwLowDateTime && dwHighDateTime == other.dwHighDateTime;
    }
};

/*!
** \brief Convert an int into a FormatType
** \param value An arbitrary int
** \param defvalue The default value to use if an invalid value is provided
*/
EolType convertIntToFormatType(int value, EolType defvalue = EolType::osdefault);

#define PURE_LC_NONE   0
#define PURE_LC_BOL    1
#define PURE_LC_WSP    2

void cutString(const wchar_t* str2cut, std::vector<std::wstring>& patternVect);
void cutStringBy(const wchar_t* str2cut, std::vector<std::wstring>& patternVect, wchar_t byChar, bool allowEmptyStr);

struct SessionPosition
{
    intptr_t _firstVisibleLine = 0;
    intptr_t _startPos = 0;
    intptr_t _endPos = 0;
    intptr_t _xOffset = 0;
    intptr_t _selMode = 0;
    intptr_t _scrollWidth = 1;
    intptr_t _offset = 0;
    intptr_t _wrapCount = 0;
};

struct SessionMapPosition
{
private:
    static constexpr intptr_t _maxPeekLenInKB = 512;
public:
    intptr_t _firstVisibleDisplayLine = -1;
    intptr_t _firstVisibleDocLine = -1;
    intptr_t _lastVisibleDocLine = -1;
    intptr_t _nbLine = -1;
    intptr_t _higherPos = -1;
    intptr_t _width = -1;
    intptr_t _height = -1;
    intptr_t _wrapIndentMode = -1;
    intptr_t _KByteInDoc = _maxPeekLenInKB;
    bool _isWrap = false;

    bool isValid() const { return (_firstVisibleDisplayLine != -1); }
    bool canScroll() const { return (_KByteInDoc < _maxPeekLenInKB); }
    static constexpr intptr_t getMaxPeekLenInKB() { return _maxPeekLenInKB; }
};

struct sessionFileInfo : public SessionPosition
{
    sessionFileInfo(const wchar_t* fn, const wchar_t* ln, int encoding, bool userReadOnly,
                    bool isPinned, bool isUntitleTabRenamed, const SessionPosition& pos,
                    const wchar_t* backupFilePath, NppFileTime originalFileLastModifTimestamp,
                    const SessionMapPosition& mapPos) noexcept
        : SessionPosition(pos), _fileName(fn ? fn : L""), _langName(ln ? ln : L"")
        , _encoding(encoding), _isUserReadOnly(userReadOnly), _isPinned(isPinned)
        , _isUntitledTabRenamed(isUntitleTabRenamed)
        , _backupFilePath(backupFilePath ? backupFilePath : L"")
        , _originalFileLastModifTimestamp(originalFileLastModifTimestamp)
        , _mapPos(mapPos)
    {}

    explicit sessionFileInfo(const std::wstring& fn) noexcept : _fileName(fn) {}

    std::wstring _fileName;
    std::wstring _langName;
    std::vector<size_t> _marks;
    std::vector<size_t> _foldStates;
    int  _encoding = -1;
    bool _isUserReadOnly = false;
    bool _isMonitoring = false;
    int _individualTabColour = -1;
    bool _isRTL = false;
    bool _isPinned = false;
    bool _isUntitledTabRenamed = false;
    std::wstring _backupFilePath;
    NppFileTime _originalFileLastModifTimestamp {};
    SessionMapPosition _mapPos;
};

struct Session
{
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

struct CmdLineParams
{
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

    QPoint _point;
    bool _isPointXValid = false;
    bool _isPointYValid = false;

    bool _isSessionFile = false;
    bool _isRecursive = false;
    bool _openFoldersAsWorkspace = false;
    bool _monitorFiles = false;

    LangType _langType = L_EXTERNAL;
    std::wstring _localizationPath;
    std::wstring _udlName;
    std::wstring _pluginMessage;
    std::wstring _easterEggName;
    unsigned char _quoteType = 0;
    int _ghostTypingSpeed = -1;

    bool isPointValid() const { return _isPointXValid && _isPointYValid; }
};

struct CmdLineParamsDTO
{
    bool _isReadOnly = false;
    bool _isNoSession = false;
    bool _isSessionFile = false;
    bool _isRecursive = false;
    bool _openFoldersAsWorkspace = false;
    bool _monitorFiles = false;

    intptr_t _line2go = 0;
    intptr_t _column2go = 0;
    intptr_t _pos2go = 0;

    LangType _langType = L_EXTERNAL;
    wchar_t _udlName[260] = {'\0'};
    wchar_t _pluginMessage[2048] = {'\0'};

    static CmdLineParamsDTO FromCmdLineParams(const CmdLineParams& params);
};

inline constexpr int FWI_PANEL_WH_DEFAULT = 100;
inline constexpr int DMD_PANEL_WH_DEFAULT = 200;

struct FloatingWindowInfo
{
    int _cont = 0;
    QRect _pos { 0, 0, FWI_PANEL_WH_DEFAULT, FWI_PANEL_WH_DEFAULT };
    explicit FloatingWindowInfo(int cont, int x, int y, int w, int h) noexcept
        : _cont(cont), _pos(x, y, w, h) {}
};

struct PluginDlgDockingInfo final
{
    std::wstring _name;
    int _internalID = -1;
    int _currContainer = -1;
    int _prevContainer = -1;
    bool _isVisible = false;

    PluginDlgDockingInfo(const wchar_t* pluginName, int id, int curr, int prev, bool isVis)
        : _name(pluginName), _internalID(id), _currContainer(curr), _prevContainer(prev), _isVisible(isVis) {}
    bool operator==(const PluginDlgDockingInfo& rhs) const { return _internalID == rhs._internalID && _name == rhs._name; }
};

struct ContainerTabInfo final
{
    int _cont = 0;
    int _activeTab = 0;
    ContainerTabInfo(int cont, int activeTab) : _cont(cont), _activeTab(activeTab) {}
};

struct DockingManagerData final
{
    int _leftWidth = DMD_PANEL_WH_DEFAULT;
    int _rightWidth = DMD_PANEL_WH_DEFAULT;
    int _topHeight = DMD_PANEL_WH_DEFAULT;
    int _bottomHeight = DMD_PANEL_WH_DEFAULT;
    LONG _minDockedPanelVisibility = HIGH_CAPTION;
    QSize _minFloatingPanelSize { (HIGH_CAPTION) * 6, HIGH_CAPTION };
    std::vector<FloatingWindowInfo> _floatingWindowInfo;
    std::vector<PluginDlgDockingInfo> _pluginDockInfo;
    std::vector<ContainerTabInfo> _containerTabInfo;

    bool getFloatingRCFrom(int floatCont, QRect& rc) const
    {
        for (const auto& fwi : _floatingWindowInfo)
        {
            if (fwi._cont == floatCont) { rc = fwi._pos; return true; }
        }
        return false;
    }
};

struct Style final
{
    int _styleID = STYLE_NOT_USED;
    std::wstring _styleDesc;
    QRgb _fgColor = static_cast<QRgb>(STYLE_NOT_USED);
    QRgb _bgColor = static_cast<QRgb>(STYLE_NOT_USED);
    int _colorStyle = COLORSTYLE_ALL;
    bool _isFontEnabled = false;
    std::wstring _fontName;
    int _fontStyle = STYLE_NOT_USED;
    int _fontSize = STYLE_NOT_USED;
    int _nesting = FONTSTYLE_NONE;
    int _keywordClass = STYLE_NOT_USED;
    std::string _keywords;
};

struct GlobalOverride final
{
    bool isEnable() const { return enableFg || enableBg || enableFont || enableFontSize || enableBold || enableItalic || enableUnderLine; }
    bool enableFg = false;
    bool enableBg = false;
    bool enableFont = false;
    bool enableFontSize = false;
    bool enableBold = false;
    bool enableItalic = false;
    bool enableUnderLine = false;
};

struct StyleArray
{
    auto begin() const { return _styleVect.begin(); }
    auto end() const { return _styleVect.end(); }
    auto begin() { return _styleVect.begin(); }
    auto end() { return _styleVect.end(); }
    void clear() { _styleVect.clear(); }
    Style& getStyler(size_t index) {
        if (index >= _styleVect.size()) throw std::out_of_range("Styler index out of range");
        return _styleVect[index];
    }
    void addStyler(int styleID, const NppXml::Element& styleNode);
    void addStyler(int styleID, const std::wstring& styleName) {
        _styleVect.emplace_back();
        Style& s = _styleVect.back();
        s._styleID = styleID; s._styleDesc = styleName;
        s._fgColor = qRgb(0,0,0); s._bgColor = qRgb(255,255,255);
    }
    Style* findByID(int id) {
        auto it = std::find_if(_styleVect.begin(), _styleVect.end(),
            [&id](const Style& s) { return s._styleID == id; });
        return (it != _styleVect.end()) ? &(*it) : nullptr;
    }
    Style* findByName(const std::wstring& name) {
        auto it = std::find_if(_styleVect.begin(), _styleVect.end(),
            [&name](const Style& s) { return s._styleDesc == name; });
        return (it != _styleVect.end()) ? &(*it) : nullptr;
    }
protected:
    std::vector<Style> _styleVect;
};

struct LexerStyler : public StyleArray
{
public:
    LexerStyler() noexcept = default;
    LexerStyler(const LexerStyler& ls) noexcept = default;
    LexerStyler& operator=(const LexerStyler& ls) {
        if (this != &ls) {
            this->_styleVect = ls._styleVect;
            this->_lexerName = ls._lexerName;
            this->_lexerDesc = ls._lexerDesc;
            this->_lexerUserExt = ls._lexerUserExt;
        }
        return *this;
    }
    void setLexerName(const wchar_t *ln) { _lexerName = ln; }
    void setLexerDesc(const wchar_t *ld) { _lexerDesc = ld; }
    void setLexerUserExt(const wchar_t *lue) { _lexerUserExt = lue; }
    const wchar_t* getLexerName() const { return _lexerName.c_str(); }
    const wchar_t* getLexerDesc() const { return _lexerDesc.c_str(); }
    const wchar_t* getLexerUserExt() const { return _lexerUserExt.c_str(); }
private:
    std::wstring _lexerName;
    std::wstring _lexerDesc;
    std::wstring _lexerUserExt;
};

struct SortLexersInAlphabeticalOrder {
    bool operator()(const LexerStyler& l, const LexerStyler& r) const {
        if (std::wcscmp(l.getLexerDesc(), L"Search result") == 0) return false;
        if (std::wcscmp(r.getLexerDesc(), L"Search result") == 0) return true;
        return std::wcscmp(l.getLexerDesc(), r.getLexerDesc()) < 0;
    }
};

struct LexerStylerArray
{
    size_t getNbLexer() const { return _lexerStylerVect.size(); }
    void clear() { _lexerStylerVect.clear(); }
    LexerStyler& getLexerFromIndex(size_t index) {
        assert(index < _lexerStylerVect.size());
        return _lexerStylerVect[index];
    }
    const wchar_t* getLexerNameFromIndex(size_t index) const { return _lexerStylerVect[index].getLexerName(); }
    const wchar_t* getLexerDescFromIndex(size_t index) const { return _lexerStylerVect[index].getLexerDesc(); }
    LexerStyler* getLexerStylerByName(const wchar_t* lexerName) {
        if (!lexerName) return nullptr;
        auto it = std::find_if(_lexerStylerVect.begin(), _lexerStylerVect.end(),
            [&lexerName](const LexerStyler& ls) { return std::wcscmp(ls.getLexerName(), lexerName) == 0; });
        return (it != _lexerStylerVect.end()) ? &(*it) : nullptr;
    }
    void addLexerStyler(const char* lexerName, const char* lexerDesc, const char* lexerUserExt, const NppXml::Element& lexerNode);
    void sort() { std::sort(_lexerStylerVect.begin(), _lexerStylerVect.end(), SortLexersInAlphabeticalOrder()); }
private:
    std::vector<LexerStyler> _lexerStylerVect;
};

struct NewDocDefaultSettings final
{
    EolType _format = EolType::osdefault;
    UniMode _unicodeMode = UniMode::uniUTF8_NoBOM;
    bool _openAnsiAsUtf8 = true;
    LangType _lang = L_TEXT;
    int _codepage = -1;
    bool _addNewDocumentOnStartup = false;
    bool _useContentAsTabName = false;
};

struct LangMenuItem final
{
    LangType _langType = L_TEXT;
    int _cmdID = -1;
    std::wstring _langName;
    explicit LangMenuItem(LangType lt, int cmdID = 0, const std::wstring& langName = L"") noexcept
        : _langType(lt), _cmdID(cmdID), _langName(langName) {}
    bool operator<(const LangMenuItem& rhs) const;
};

struct PrintSettings final {
    bool _printLineNumber = true;
    int _printOption = npp_sci::SC_PRINT_COLOURONWHITE;
    std::wstring _headerLeft, _headerMiddle, _headerRight;
    std::wstring _headerFontName;
    int _headerFontStyle = 0;
    int _headerFontSize = 0;
    std::wstring _footerLeft, _footerMiddle, _footerRight;
    std::wstring _footerFontName;
    int _footerFontStyle = 0;
    int _footerFontSize = 0;
    QRect _marge;
    PrintSettings() { _marge = QRect(0,0,0,0); }
    bool isHeaderPresent() const { return (!_headerLeft.empty() || !_headerMiddle.empty() || !_headerRight.empty()); }
    bool isFooterPresent() const { return (!_footerLeft.empty() || !_footerMiddle.empty() || !_footerRight.empty()); }
    bool isUserMargePresent() const { return ((_marge.left() != 0) || (_marge.top() != 0) || (_marge.right() != 0) || (_marge.bottom() != 0)); }
};

class Date final
{
public:
    Date() = default;
    Date(unsigned long year, unsigned long month, unsigned long day);
    explicit Date(const char* dateStr);
    explicit Date(int nbDaysFromNow) noexcept;
    void now();
    std::string toString() const;
    bool operator<(const Date& compare) const;
    bool operator>(const Date& compare) const;
    bool operator==(const Date& compare) const;
    bool operator!=(const Date& compare) const;
private:
    unsigned long _year = 2008, _month = 4, _day = 26;
};

class MatchedPairConf final
{
public:
    bool hasUserDefinedPairs() const { return !_matchedPairs.empty(); }
    bool hasDefaultPairs() const { return _doParentheses||_doBrackets||_doCurlyBrackets||_doQuotes||_doDoubleQuotes||_doHtmlXmlTag; }
    bool hasAnyPairsPair() const { return hasUserDefinedPairs() || hasDefaultPairs(); }
public:
    std::vector<std::pair<char, char>> _matchedPairs;
    bool _doHtmlXmlTag = false;
    bool _doParentheses = false;
    bool _doBrackets = false;
    bool _doCurlyBrackets = false;
    bool _doQuotes = false;
    bool _doDoubleQuotes = false;
};

// TbIconInfo struct (in NppDarkMode namespace, made accessible here via using)
struct AdvOptDefaults final
{
    std::wstring _xmlFileName;
    TbIconInfo _tbIconInfo{};
    int _tabIconSet = -1;
    bool _tabUseTheme = false;
};

struct AdvancedOptions final
{
    AdvOptDefaults _darkDefaults{
        L"DarkModeDefault.xml",
        TbIconInfo{ToolBarStatusType::TB_SMALL, FluentColor::defaultColor, 0, false},
        2,
        false
    };
    AdvOptDefaults _lightDefaults{
        L"",
        TbIconInfo{ToolBarStatusType::TB_STANDARD, FluentColor::defaultColor, 0, false},
        0,
        true
    };
    bool _enableWindowsMode = false;
};

struct DarkModeConf final
{
    bool _isEnabled = false;
    bool _isEnabledPlugin = true;
    NppDarkMode::ColorTone _colorTone = NppDarkMode::ColorTone::blackTone;
    NppDarkMode::Colors _customColors{};
    AdvancedOptions _advOptions{};
};

struct LargeFileRestriction final
{
    int64_t _largeFileSizeDefInByte = NPP_STYLING_FILESIZE_LIMIT_DEFAULT;
    bool _isEnabled = true;
    bool _deactivateWordWrap = true;
    bool _allowBraceMatch = false;
    bool _allowAutoCompletion = false;
    bool _allowSmartHilite = false;
    bool _allowClickableLink = false;
    bool _suppress2GBWarning = false;
};

struct NppGUI final
{
    TbIconInfo _tbIconInfo{ ToolBarStatusType::TB_STANDARD, FluentColor::defaultColor, 0, false };
    bool _toolbarShow = true;
    bool _statusBarShow = true;
    bool _menuBarShow = true;
    int _tabStatus = (TAB_DRAWTOPBAR | TAB_DRAWINACTIVETAB | TAB_DRAGNDROP | TAB_REDUCE | TAB_CLOSEBUTTON | TAB_PINBUTTON);
    bool _forceTabbarVisible = false;
    int _tabCompactLabelLen = 0;
    bool _splitterPos = POS_VERTICAL;
    int _userDefineDlgStatus = UDD_DOCKED;
    int _tabSize = 4;
    bool _tabReplacedBySpace = false;
    bool _backspaceUnindent = false;
    bool _finderLinesAreCurrentlyWrapped = false;
    bool _finderPurgeBeforeEverySearch = false;
    bool _finderShowOnlyOneEntryPerFoundLine = true;
    bool _fif_ignoreunsavedChangesInOpenedFiles = false;
    int _fileAutoDetection = CDENABLEDNEW;
    bool _checkHistoryFiles = false;
    QRect _appPos {0, 0, 1024, 700};
    QRect _findWindowPos;
    bool _findWindowLessMode = false;
    bool _isMaximized = false;
    int _isMinimizedToTray = sta_none;
    bool _rememberLastSession = true;
    bool _keepSessionAbsentFileEntries = false;
    bool _isCmdlineNosessionActivated = false;
    bool _isFullReadOnly = false;
    bool _isFullReadOnlySavingForbidden = false;
    bool _detectEncoding = true;
    bool _saveAllConfirm = true;
    bool _setSaveDlgExtFiltToAllTypes = false;
    bool _doTaskList = true;
    AutoIndentMode _maintainIndent = AutoIndentMode::AI_SMART;
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
    int _inSelectionAutocheckThreshold = FINDREPLACE_INSELECTION_THRESHOLD_DEFAULT;
    int _fillFindWhatThreshold = FILL_FINDWHAT_THRESHOLD_DEFAULT;
    bool _fillDirFieldFromActiveDoc = false;
    bool _muteSounds = false;
    bool _enableFoldCmdToggable = false;
    bool _hideMenuRightShortcuts = false;
    writeTechnologyEngine _writeTechnologyEngine = directWriteTechnology;
    bool _isWordCharDefault = true;
    std::string _customWordChars;
    urlMode _styleURL = urlMode::URL_HTML;
    std::wstring _uriSchemes = L"svn:// cvs:// git:// imap:// irc:// irc6:// ircs:// ldap:// ldaps:// news: telnet:// gopher:// ssh:// sftp:// smb:// skype: snmp:// spotify: steam:// sms: slack:// chrome:// bitcoin:";
    NewDocDefaultSettings _newDocDefaultSettings;
    std::wstring _dateTimeFormat = L"yyyy-MM-dd HH:mm:ss";
    bool _dateTimeReverseDefaultOrder = false;

    void setTabReplacedBySpace(bool b) { _tabReplacedBySpace = b; }
    const NewDocDefaultSettings& getNewDocDefaultSettings() const { return _newDocDefaultSettings; }
    std::vector<LangMenuItem> _excludedLangList;
    bool _isLangMenuCompact = true;
    PrintSettings _printSettings;
    BackupFeature _backup = BackupFeature::BackupNone;
    bool _useDir = false;
    std::wstring _backupDir;
    DockingManagerData _dockingData;
    GlobalOverride _globalOverride;
    enum AutocStatus { autoc_none, autoc_func, autoc_word, autoc_both };
    AutocStatus _autocStatus = autoc_both;
    int _autocFromLen = 1;
    bool _autocIgnoreNumbers = true;
    bool _autocInsertSelectedUseENTER = true;
    bool _autocInsertSelectedUseTAB = true;
    bool _autocBrief = false;
    bool _funcParams = true;
    MatchedPairConf _matchedPairConf;
    std::wstring _definedSessionExt;
    std::wstring _definedWorkspaceExt;
    QString _commandLineInterpreter = QStringLiteral("/bin/sh");

    enum AutoUpdateMode { autoupdate_disabled, autoupdate_on_startup, autoupdate_on_exit };
    struct AutoUpdateOptions {
        AutoUpdateMode _doAutoUpdate = autoupdate_on_startup;
        int _intervalDays = 15;
        Date _nextUpdateDate;
        AutoUpdateOptions() : _nextUpdateDate(Date()) {}
    };
    AutoUpdateOptions _autoUpdateOpt;

    bool _doesExistUpdater = false;
    int _caretBlinkRate = 600;
    int _caretWidth = 1;
    bool _shortTitlebar = false;
    OpenSaveDirSetting _openSaveDir = OpenSaveDirSetting::USER_DEFAULT_DIR;

    std::array<wchar_t, 260> _defaultDir{};
    std::array<wchar_t, 260> _defaultDirExp{};
    std::array<wchar_t, 260> _lastUsedDir{};
    std::wstring _themeName;

    enum class MultiInstSetting { MONO_INSTANCE, MULTI_INSTANCE };
    MultiInstSetting _multiInstSetting = MultiInstSetting::MONO_INSTANCE;
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
    unsigned char _availableClouds = '\0';
    enum SearchEngineChoice { se_custom = 0, se_duckDuckGo = 1, se_google = 2, se_bing = 3, se_yahoo = 4, se_stackoverflow = 5 };
    SearchEngineChoice _searchEngineChoice = se_google;
    std::wstring _searchEngineCustom;
    bool _isFolderDroppedOpenFiles = false;
    bool _isDocPeekOnTab = false;
    bool _isDocPeekOnMap = false;
    bool _shouldSortFunctionList = false;
    DarkModeConf _darkmode;
    LargeFileRestriction _largeFileRestriction;
};

struct ScintillaViewParams
{
    bool _lineNumberMarginShow = true;
    bool _lineNumberMarginDynamicWidth = true;
    bool _bookMarkMarginShow = true;
    bool _isChangeHistoryMarginEnabled = true;
    bool _isChangeHistoryIndicatorEnabled = false;
    changeHistoryState _isChangeHistoryEnabled4NextSession = changeHistoryState::CHG_HIST_ENABLED;
    folderStyle _folderStyle = folderStyle::FOLDER_STYLE_BOX;
    lineWrapMethod _lineWrapMethod = lineWrapMethod::WrapByWindow;
    bool _foldMarginShow = true;
    bool _indentGuideLineShow = true;
    lineHiliteMode _currentLineHiliteMode = LINEHILITE_HILITE;
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
    enum crlfMode { plainText = 0, roundedRectangleText = 1, plainTextCustomColor = 2, roundedRectangleTextCustomColor = 3 };
    crlfMode _eolMode = roundedRectangleText;
    bool _npcShow = false;
    enum npcMode { identity = 0, abbreviation = 1, codepoint = 2 };
    npcMode _npcMode = abbreviation;
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
    int getDistractionFreePadding(int editViewWidth) const {
        const int defaultDiviser = 4;
        int diviser = _distractionFreeDivPart > 2 ? _distractionFreeDivPart : defaultDiviser;
        int paddingLen = editViewWidth / diviser;
        if (paddingLen <= 0) paddingLen = editViewWidth / defaultDiviser;
        return paddingLen;
    }
    bool _lineCopyCutWithoutSelection = true;
    bool _disableSelectedTextDragDrop = false;
    bool _multiSelection = true;
    bool _columnSel2MultiEdit = true;
};

struct Lang final
{
    LangType _langID = L_TEXT;
    std::wstring _langName;
    std::wstring _defaultExtList;
    std::string _langKeyWordList[NB_LIST];
    std::string _pCommentLineSymbol;
    std::string _pCommentStart;
    std::string _pCommentEnd;
    int _tabSize = -1;
    bool _isTabReplacedBySpace = false;
    bool _isBackspaceUnindent = false;

    Lang() noexcept = default;
    Lang(LangType langID, const wchar_t* name, const wchar_t* extLst,
         const char* commentLine, const char* commentStart, const char* commentEnd,
         int tabInfo, bool isBackspaceUnindent) noexcept;
    ~Lang() = default;
    void setTabInfo(int tabInfo);
    const wchar_t* getDefaultExtList() const { return _defaultExtList.c_str(); }
    void setWords(const char* words, int index) { _langKeyWordList[index] = words; }
    const char* getWords(int index) const { return _langKeyWordList[index].c_str(); }
    LangType getLangID() const { return _langID; }
    const wchar_t* getLangName() const { return _langName.c_str(); }
    int getTabInfo() const;
};

class UserLangContainer final
{
public:
    UserLangContainer() noexcept : _name(L"new user define"), _ext(L""), _udlVersion(""), _isDarkModeTheme(false) {}
    explicit UserLangContainer(const wchar_t* name, const wchar_t* ext, const char* udlVer, bool isDarkModeTheme) noexcept;
    UserLangContainer(const UserLangContainer& ulc) noexcept;
    UserLangContainer& operator=(const UserLangContainer& ulc);
    const wchar_t* getName() const { return _name.c_str(); }
    const wchar_t* getExtention() const { return _ext.c_str(); }
    const char* getUdlVersion() const { return _udlVersion.c_str(); }
private:
    StyleArray _styles;
    std::wstring _name;
    std::wstring _ext;
    std::string _udlVersion;
    std::string _keywordLists[npp_sci::SCE_USER_KWLIST_TOTAL];
    bool _isPrefix[npp_sci::SCE_USER_TOTAL_KEYWORD_GROUPS] = { false };
    int _forcePureLC = PURE_LC_NONE;
    int _decimalSeparator = DECSEP_DOT;
    bool _isCaseIgnored = false;
    bool _allowFoldOfComments = false;
    bool _foldCompact = false;
    bool _isDarkModeTheme = false;
    friend class Notepad_plus;
    friend class ScintillaComponent;
    friend class NppParameters;
    friend class SharedParametersDialog;
    friend class FolderStyleDialog;
    friend class KeyWordsStyleDialog;
    friend class CommentStyleDialog;
    friend class SymbolsStyleDialog;
    friend class UserDefineDialog;
    friend class StylerDlg;
};

class ExternalLangContainer final
{
public:
    std::string _name;
    void* fnCL = nullptr; // Lexilla::CreateLexerFn — TODO: resolve on    // Linux. Use QLibrary.
    // For Notepad++
    ExternalLexerAutoIndentMode _autoIndentMode = ExternalLexerAutoIndentMode::Standard;
};

struct FindHistory final
{
    enum searchMode { normal, extended, regExpr };
    enum transparencyMode { none, onLosingFocus, persistent };

    bool _isSearch2ButtonsMode = false;
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
    searchMode _searchMode = normal;
    transparencyMode _transparencyMode = onLosingFocus;
    int _transparency = 150;
    bool _isFilterFollowDoc = false;
    bool _isBookmarkLine = false;
    bool _isPurge = false;
    bool _regexBackward4PowerUser = false;
};

// NumBase enum used by ColumnEditorParam (matches Parameters.cpp usage)
enum class NumBase {
    BASE_10 = 0,
    BASE_16 = 1,
    BASE_16_UPPERCASE = 2,
    BASE_08 = 3,
    BASE_02 = 4
};

struct ColumnEditorParam final
{
    enum class leadingChoice : unsigned char { noneLeading, zeroLeading, spaceLeading };
    bool _mainChoice = true; // activeNumeric equivalent
    std::wstring _insertedTextContent;
    int _initialNum = -1;
    int _increaseNum = -1;
    int _repeatNum = -1;
    NumBase _formatChoice = NumBase::BASE_10;
    leadingChoice _leadingChoice = leadingChoice::noneLeading;
};

class LocalizationSwitcher final
{
friend class NppParameters;
public:
    struct LocalizationDefinition
    {
        const wchar_t *_langName = nullptr;
        const wchar_t *_xmlFileName = nullptr;
    };
    bool addLanguageFromXml(const std::wstring& xmlFullPath);
    std::wstring getLangFromXmlFileName(const wchar_t* fn);
    std::wstring getXmlFilePathFromLangName(const wchar_t *langName) const;
    bool switchToLang(const wchar_t *lang2switch) const;
    size_t size() const { return _localizationList.size(); }
    std::pair<std::wstring, std::wstring> getElementFromIndex(size_t index) const;
    void setFileName(const QString& fn);
    const std::string& getFileName() const { return _fileName; }
private:
    std::vector<std::pair<std::wstring, std::wstring>> _localizationList;
    std::wstring _nativeLangPath;
    std::string _fileName;
};

class ThemeSwitcher final
{
friend class NppParameters;
public:
    void addThemeFromXml(const std::wstring& xmlFullPath);
    void addDefaultThemeFromXml(const std::wstring& xmlFullPath);
    static std::wstring getThemeFromXmlFileName(const wchar_t* xmlFullPath);
    std::wstring getXmlFilePathFromThemeName(const wchar_t *themeName) const;
    bool themeNameExists(const wchar_t *themeName);
    size_t size() const { return _themeList.size(); }
    std::pair<std::wstring, std::wstring>& getElementFromIndex(size_t index);
    void setThemeDirPath(const std::wstring& themeDirPath) { _themeDirPath = themeDirPath; }
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

struct HLSColour
{
    unsigned short _hue = 0;
    unsigned short _lightness = 0;
    unsigned short _saturation = 0;
    HLSColour() = default;
    HLSColour(unsigned short hue, unsigned short lightness, unsigned short saturation)
        : _hue(hue), _lightness(lightness), _saturation(saturation) {}
    explicit HLSColour(QRgb rgb);
    void loadFromRGB(QRgb rgb);
    QRgb toRGB() const;
    QRgb toRGB4DarkModeWithTuning(int lightnessMore, int saturationLess) const;
    QRgb toRGB4DarkMod() const { return toRGB4DarkModeWithTuning(50, 20); }
};

struct UdlXmlFileState final {
    NppXml::Document _udlXmlDoc = nullptr;
    std::wstring _path;
    bool _isDirty = false;
    bool _isInDefaultSharedContainer = false;
    std::pair<unsigned char, unsigned char> _indexRange;
    UdlXmlFileState(NppXml::Document doc, const std::wstring& path, bool isDirty,
                    bool isInDefaultSharedContainer, std::pair<unsigned char, unsigned char> range) noexcept
        : _udlXmlDoc(doc), _path(path), _isDirty(isDirty),
          _isInDefaultSharedContainer(isInDefaultSharedContainer), _indexRange(range) {}
};

inline constexpr int NB_LANG = 100;

class DynamicMenu final
{
public:
    bool attach(QMenu* hMenu, unsigned int posBase, int lastCmd, const std::wstring& lastCmdLabel);
    bool createMenu() const;
    bool clearMenu() const;
    int getTopLevelItemNumber() const;
    void push_back(const MenuItemUnit& m) { _menuItems.push_back(m); }
    void emplace_back(unsigned long cmdID, const std::wstring& itemName, const std::wstring& parentFolderName) {
        _menuItems.emplace_back(cmdID, itemName, parentFolderName);
    }
    MenuItemUnit& getItemFromIndex(size_t i) { return _menuItems[i]; }
    void erase(size_t i) { _menuItems.erase(_menuItems.begin() + i); }
    unsigned int getPosBase() const { return _posBase; }
    const std::wstring& getLastCmdLabel() const { return _lastCmdLabel; }
private:
    std::vector<MenuItemUnit> _menuItems;
    void* _hMenu = nullptr; // QMenu*
    unsigned int _posBase = 0;
    int _lastCmd = 0;
    std::wstring _lastCmdLabel;
};

struct LanguageNameInfo;

class NppParameters final
{
private:
    static NppParameters* getInstancePointer() {
        static NppParameters* instance = new NppParameters;
        return instance;
    }

    struct XmlDocPath final
    {
        NppXml::Document _doc;
        std::wstring _path;
    };

public:
    static NppParameters& getInstance() { return *getInstancePointer(); }
    static LangType getLangIDFromStr(const wchar_t *langName);
    static std::wstring getLocPathFromStr(const std::wstring & localizationCode);

    bool load();
    bool reloadLang();
    bool reloadStylers(const wchar_t *stylePath = nullptr);
    void destroyInstance();
    std::wstring getSettingsFolder() const;

    bool _isTaskListRBUTTONUP_Active = false;
    int L_END;

    NppGUI & getNppGUI() { return _nppGUI; }

    const char* getWordList(LangType langID, int typeIndex) const;
    Lang* getLangFromID(LangType langID) const;
    Lang* getLangFromIndex(size_t i) const;
    int getNbLang() const { return _nbLang; }
    LangType getLangFromExt(const wchar_t *ext);
    const wchar_t* getLangExtFromName(const wchar_t* langName) const;
    const wchar_t* getLangExtFromLangType(LangType langType) const;
    int getNbLRFile() const { return _nbRecentFile; }
    std::wstring* getLRFile(int index) const { return _LRFileList[index].get(); }
    void setNbMaxRecentFile(int nb) { _nbMaxRecentFile = nb; }
    int getNbMaxRecentFile() const { return _nbMaxRecentFile; }
    void setPutRecentFileInSubMenu(bool doSubmenu) { _putRecentFileInSubMenu = doSubmenu; }
    bool putRecentFileInSubMenu() const { return _putRecentFileInSubMenu; }
    void setRecentFileCustomLength(int len) { _recentFileCustomLength = len; }
    int getRecentFileCustomLength() const { return _recentFileCustomLength; }
    const ScintillaViewParams& getSVP() const { return _svp; }
    bool writeRecentFileHistorySettings(int nbMaxFile = -1);
    bool writeHistory(const wchar_t *fullpath);
    bool writeProjectPanelsSettings();
    bool writeColumnEditorSettings();
    bool writeFileBrowserSettings(const std::vector<std::wstring>& rootPaths, const std::wstring& latestSelectedItemPath);
    static NppXml::Element getChildElementByAttribute(const NppXml::Element& element, const char* childName, const char* attributeName, const char* attributeVal);
    bool writeScintillaParams();
    void createXmlTreeFromGUIParams();
    std::wstring writeStyles(LexerStylerArray & lexersStylers, StyleArray & globalStylers);
    bool insertTabInfo(const wchar_t* langName, int tabInfo, bool backspaceUnindent);

    LexerStylerArray& getLStylerArray() { return _lexerStylerVect; }
    StyleArray& getGlobalStylers() { return _widgetStyleArray; }
    StyleArray& getMiscStylerArray() { return _widgetStyleArray; }
    GlobalOverride& getGlobalOverrideStyle() { return _nppGUI._globalOverride; }

    QRgb getCurLineHilitingColour();
    void setCurLineHilitingColour(QRgb colour2Set);

    void setFontList(QWidget* hWnd);
    bool isInFontList(const std::wstring& fontName2Search) const;
    const std::vector<std::wstring>& getFontList() const { return _fontlist; }

    int getNbUserLang() const { return _nbUserLang; }
    UserLangContainer* getULCFromIndex(size_t i) { return _userLangArray[i].get(); }
    const UserLangContainer* getULCFromName(const wchar_t* userLangName) const;
    int getNbExternalLang() const { return _nbExternalLang; }
    int getExternalLangIndexFromName(const wchar_t *externalLangName) const;
    ExternalLangContainer* getELCFromIndex(int i) { return _externalLangArray[i].get(); }
    bool ExternalLangHasRoom() const { return _nbExternalLang < NB_MAX_EXTERNAL_LANG; }
    void getExternalLexerFromXmlTree(NppXml::Document externalLexerDoc);
    std::vector<XmlDocPath>* getExternalLexerDoc() { return &_pXmlExternalLexerDoc; }
    void writeDefaultUDL();
    void writeNonDefaultUDL();
    void writeNeed2SaveUDL();
    void writeShortcuts();
    void writeSession(const Session& session, const wchar_t* fileName = nullptr) const;
    bool writeFindHistory();
    bool isExistingUserLangName(const wchar_t* newName) const;
    const wchar_t* getUserDefinedLangNameFromExt(const wchar_t* ext, const wchar_t* fullName) const;
    int addUserLangToEnd(const UserLangContainer* userLang, const wchar_t *newName);
    void removeUserLang(size_t index);
    bool isExistingExternalLangName(const char* newName) const;
    int addExternalLangToEnd(std::unique_ptr<ExternalLangContainer> externalLang);
    NppXml::Document getNativeLang() const { return _pXmlNativeLangDoc; }
    NppXml::Document getCustomizedToolButtons() const { return _pXmlToolButtonsConfDoc; }

    bool isTransparentAvailable() const { return true; } // Qt always supports transparency

    // 0 <= percent < 256; 255 = opaque
    static void setTransparent(QWidget* widget, int percent);
    static void removeTransparent(QWidget* widget);

    void setCmdlineParam(const CmdLineParamsDTO & cmdLineParams) { _cmdLineParams = cmdLineParams; }
    const CmdLineParamsDTO& getCmdLineParams() const { return _cmdLineParams; }
    const std::wstring& getCmdLineString() const { return _cmdLineString; }
    void setCmdLineString(const std::wstring& str) { _cmdLineString = str; }
    void setFileSaveDlgFilterIndex(int ln) { _fileSaveDlgFilterIndex = ln; }
    int getFileSaveDlgFilterIndex() const { return _fileSaveDlgFilterIndex; }

    bool isRemappingShortcut() const { return !_shortcuts.empty(); }
    std::vector<CommandShortcut>& getUserShortcuts() { return _shortcuts; }
    void addUserModifiedIndex(size_t index);
    std::vector<QPointer<MacroShortcut>>& getMacroList() { return _macros; }
    std::vector<UserCommand>& getUserCommandList() { return _userCommands; }
    std::vector<PluginCmdShortcut>& getPluginCommandList() { return _pluginCommands; }
    std::vector<size_t>& getPluginModifiedKeyIndices() { return _pluginCustomizedCmds; }
    void addPluginModifiedIndex(size_t index);
    std::vector<ScintillaKeyMap>& getScintillaKeyList() { return _scintillaKeyCommands; }
    std::vector<int>& getScintillaModifiedKeyIndices() { return _scintillaModifiedKeyIndices; }
    void addScintillaModifiedIndex(int index);
    const Session& getSession() const { return _session; }
    std::vector<MenuItemUnit>& getContextMenuItems() { return _contextMenuItems; }
    std::vector<MenuItemUnit>& getTabContextMenuItems() { return _tabContextMenuItems; }
    DynamicMenu& getMacroMenuItems() { return _macroMenuItems; }
    DynamicMenu& getRunMenuItems() { return _runMenuItems; }
    bool hasCustomContextMenu() const { return !_contextMenuItems.empty(); }
    bool hasCustomTabContextMenu() const { return !_tabContextMenuItems.empty(); }
    void setAccelerator(Accelerator* pAccel) { _pAccelerator = pAccel; }
    Accelerator* getAccelerator() { return _pAccelerator; }
    void setScintillaAccelerator(ScintillaAccelerator* pScintAccel) { _pScintAccelerator = pScintAccel; }
    ScintillaAccelerator* getScintillaAccelerator() { return _pScintAccelerator; }
    const std::wstring& getNppPath() const { return _nppPath; }
    const std::wstring& getContextMenuPath() const { return _contextMenuPath; }
    const wchar_t* getAppDataNppDir() const { return _appdataNppDir.c_str(); }
    const wchar_t* getPluginRootDir() const { return _pluginRootDir.c_str(); }
    const wchar_t* getPluginConfDir() const { return _pluginConfDir.c_str(); }
    const wchar_t* getUserPluginConfDir() const { return _userPluginConfDir.c_str(); }
    const wchar_t* getWorkingDir() const { return _currentDirectory.c_str(); }
    const wchar_t* getWorkSpaceFilePath(int i) const {
        if (i < 0 || i > 2) return nullptr;
        return _workSpaceFilePaths[i].c_str();
    }
    const std::vector<std::wstring>& getFileBrowserRoots() const { return _fileBrowserRoot; }
    const std::wstring& getFileBrowserSelectedItemPath() const { return _fileBrowserSelectedItemPath; }
    void setWorkSpaceFilePath(int i, const wchar_t *wsFile);
    void setWorkingDir(const wchar_t * newPath);
    void setStartWithLocFileName(const std::wstring& locPath) { _startWithLocFileName = locPath; }
    void setFunctionListExportBoolean(bool doIt) { _doFunctionListExport = doIt; }
    bool doFunctionListExport() const { return _doFunctionListExport; }
    void setPrintAndExitBoolean(bool doIt) { _doPrintAndExit = doIt; }
    bool doPrintAndExit() const { return _doPrintAndExit; }
    bool loadSession(Session& session, const wchar_t* sessionFileName, const bool bSuppressErrorMsg = false);
    void setLoadedSessionFilePath(const std::wstring & loadedSessionFilePath) { _loadedSessionFullFilePath = loadedSessionFilePath; }
    const std::wstring& getLoadedSessionFilePath() const { return _loadedSessionFullFilePath; }
    int langTypeToCommandID(LangType lt) const;

    struct FindDlgTabTitles final {
        std::wstring _find;
        std::wstring _replace;
        std::wstring _findInFiles;
        std::wstring _mark;
    };
    FindDlgTabTitles& getFindDlgTabTitles() { return _findDlgTabTitles; }
    bool asNotepadStyle() const { return _asNotepadStyle; }
    bool reloadPluginCmds() { return getPluginCmdsFromXmlTree(); }

    bool getContextMenuFromXmlTree(void* mainMenuHandle, void* pluginsMenu, bool isEditCM = true);
    bool reloadContextMenuFromXmlTree(void* mainMenuHandle, void* pluginsMenu);
    winVer getWinVersion() const { return _winVersion; }
    std::wstring getWinVersionStr() const;
    std::wstring getWinVerBitStr() const;
    int currentSystemCodepage() const { return _currentSystemCodepage; }
    bool isCurrentSystemCodepageUTF8() const { return _currentSystemCodepage == 65001; }
    int defaultCodepage() const;
    FindHistory& getFindHistory() { return _findHistory; }
    bool _isFindReplacing = false;

    LocalizationSwitcher & getLocalizationSwitcher() { return _localizationSwitcher; }
    ThemeSwitcher & getThemeSwitcher() { return _themeSwitcher; }

    std::pair<unsigned char, unsigned char> importUDLFromFile(const std::wstring& sourceFile);
    bool exportUDLToFile(size_t langIndex2export, const std::wstring& fileName2save);
    NativeLangSpeaker* getNativeLangSpeaker() { return _pNativeLangSpeaker; }
    void setNativeLangSpeaker(NativeLangSpeaker *nls) { _pNativeLangSpeaker = nls; }
    bool isLocal() const { return _isLocal; }
    bool isCloud() const { return _isCloud; }
    void saveConfig_xml() const;
    const std::wstring& getUserPath() const { return _userPath; }
    const std::wstring& getUserDefineLangFolderPath() const { return _userDefineLangsFolderPath; }
    bool writeSettingsFilesOnCloudForThe1stTime(const std::wstring& cloudSettingsPath) const;
    void setCloudChoice(const wchar_t* pathChoice) const;
    void removeCloudChoice() const;
    bool isCloudPathChanged() const;
    static constexpr int archType() { return npp_sci::ARCH_TYPE; }
    QRgb getCurrentDefaultBgColor() const { return _currentDefaultBgColor; }
    QRgb getCurrentDefaultFgColor() const { return _currentDefaultFgColor; }
    void setCurrentDefaultBgColor(QRgb c) { _currentDefaultBgColor = c; }
    void setCurrentDefaultFgColor(QRgb c) { _currentDefaultFgColor = c; }
    void setCmdSettingsDir(const std::wstring& settingsDir) { _cmdSettingsDir = settingsDir; }
    void setTitleBarAdd(const std::wstring& titleAdd) { _titleBarAdditional = titleAdd; }
    const std::wstring& getTitleBarAdd() const { return _titleBarAdditional; }
    static std::wstring getSpecialFolderLocation(int folderKind);
    void setUdlXmlDirtyFromIndex(size_t i);
    void setUdlXmlDirtyFromXmlDoc(const NppXml::Document& xmlDoc);
    void removeIndexFromXmlUdls(size_t i);
    bool isStylerDocLoaded() const { return _pXmlUserStylerDoc._doc != nullptr; }
    ColumnEditorParam _columnEditParam;
    unsigned long getScintillaModEventMask() const { return _sintillaModEventMask; }
    void addScintillaModEventMask(unsigned long mask2Add) { _sintillaModEventMask |= mask2Add; }
    bool isAsNotepadStyle() const { return _asNotepadStyle; }
    static LanguageNameInfo getLangNameInfoFromNameID(const std::wstring& langNameID);
    void setNbTabCompactLabelLen(int nb) { _nppGUI._tabCompactLabelLen = nb; }
    int getNbTabCompactLabelLen() const { return _nppGUI._tabCompactLabelLen; }
    static void buildGupParams(std::wstring& params);

    void setShortcutDirty() { _isAnyShortcutModified = true; }
    void setAdminMode(bool isAdmin) { _isAdminMode = isAdmin; }
    bool isAdmin() const { return _isAdminMode; }
    bool regexBackward4PowerUser() const { return _findHistory._regexBackward4PowerUser; }
    bool isRegForOSAppRestartDisabled() const { return _isRegForOSAppRestartDisabled; }

    void initTabCustomColors();
    void setIndividualTabColor(QRgb colour2Set, int colourIndex, bool isDarkMode);
    QRgb getIndividualTabColor(int colourIndex, bool isDarkMode, bool saturated);

    void initFindDlgStatusMsgCustomColors();
    void setFindDlgStatusMsgIndexColor(QRgb colour2Set, int colourIndex);
    QRgb getFindDlgStatusMsgColor(int colourIndex);

    const std::wstring& getWingupFullPath() const { return _wingupFullPath; }
    const std::wstring& getWingupParams() const { return _wingupParams; }
    const std::wstring& getWingupDir() const { return _wingupDir; }
    bool shouldDoUAC() const { return _isElevationRequired; }
    void setWingupFullPath(const std::wstring& val2set) { _wingupFullPath = val2set; }
    void setWingupParams(const std::wstring& val2set) { _wingupParams = val2set; }
    void setWingupDir(const std::wstring& val2set) { _wingupDir = val2set; }
    void setElevationRequired(bool val2set) { _isElevationRequired = val2set; }
    bool doNppLogNetworkDriveIssue() const { return _doNppLogNetworkDriveIssue; }
    bool isNppAutoUpdateDisabled() const { return _isNppAutoUpdateDisabled; }
    void endSessionStart() { _isEndSessionStarted = true; }
    bool isEndSessionStarted() const { return _isEndSessionStarted; }
    void makeEndSessionCritical() { _isEndSessionCritical = true; }
    bool isEndSessionCritical() const { return _isEndSessionCritical; }
    void setPlaceHolderEnable(bool isEnabled) { _isPlaceHolderEnabled = isEnabled; }
    bool isPlaceHolderEnabled() const { return _isPlaceHolderEnabled; }
    void setTheWarningHasBeenGiven(bool isEnabled) { _theWarningHasBeenGiven = isEnabled; }
    bool theWarningHasBeenGiven() const { return _theWarningHasBeenGiven; }

private:
    NppParameters();
    ~NppParameters();
    NppParameters(const NppParameters&) = delete;
    NppParameters& operator=(const NppParameters&) = delete;
    NppParameters(NppParameters&&) = delete;
    NppParameters& operator=(NppParameters&&) = delete;

    XmlDocPath _pXmlDoc{};
    XmlDocPath _xmlUserDoc{};
    XmlDocPath _pXmlUserStylerDoc{};
    XmlDocPath _pXmlUserLangDoc{};
    std::vector<UdlXmlFileState> _pXmlUserLangsDoc;
    NppXml::Document _pXmlToolButtonsConfDoc = nullptr;
    NppXml::Document _pXmlShortcutDoc = nullptr;
    NppXml::Document _pXmlNativeLangDoc = nullptr;
    NppXml::Document _pXmlContextMenuDoc = nullptr;
    NppXml::Document _pXmlTabContextMenuDoc = nullptr;
    std::vector<XmlDocPath> _pXmlExternalLexerDoc;

    NppGUI _nppGUI;
    ScintillaViewParams _svp;
    std::unique_ptr<Lang> _langList[NB_LANG] = { nullptr };
    int _nbLang = 0;

    std::unique_ptr<std::wstring> _LRFileList[NB_MAX_LRF_FILE] = { nullptr };
    int _nbRecentFile = 0;
    int _nbMaxRecentFile = 10;
    bool _putRecentFileInSubMenu = false;
    int _recentFileCustomLength = RECENTFILES_SHOWFULLPATH;

    FindHistory _findHistory;
    std::unique_ptr<UserLangContainer> _userLangArray[NB_MAX_USER_LANG] = { nullptr };
    unsigned char _nbUserLang = 0;
    std::wstring _userDefineLangsFolderPath;
    std::wstring _userDefineLangPath;
    std::unique_ptr<ExternalLangContainer> _externalLangArray[NB_MAX_EXTERNAL_LANG] = { nullptr };
    int _nbExternalLang = 0;

    CmdLineParamsDTO _cmdLineParams;
    std::wstring _cmdLineString;
    int _fileSaveDlgFilterIndex = -1;

    LexerStylerArray _lexerStylerVect;
    StyleArray _widgetStyleArray;
    std::vector<std::wstring> _fontlist;

    bool _isLocal = false;
    bool _isx64 = false;
    bool _isCloud = false;

    std::wstring _cmdSettingsDir;
    std::wstring _titleBarAdditional;
    std::wstring _loadedSessionFullFilePath;

    std::array<HLSColour, 5> individualTabHuesFor_Dark{ {
        HLSColour{37, 60, 60}, HLSColour{70, 60, 60}, HLSColour{144, 70, 60},
        HLSColour{255, 60, 60}, HLSColour{195, 60, 60} } };
    std::array<HLSColour, 5> individualTabHues{ {
        HLSColour{37, 210, 150}, HLSColour{70, 210, 150}, HLSColour{144, 210, 150},
        HLSColour{255, 210, 150}, HLSColour{195, 210, 150}} };

    std::array<QRgb, 4> findDlgStatusMessageColor{ qRgb(255,0,0), qRgb(0,0,255), qRgb(0,128,0), qRgb(255,165,0) };

    bool _isAnyShortcutModified = false;
    std::vector<CommandShortcut> _shortcuts;
    std::vector<size_t> _customizedShortcuts;
    std::vector<QPointer<MacroShortcut>> _macros;
    std::vector<UserCommand> _userCommands;
    std::vector<PluginCmdShortcut> _pluginCommands;
    std::vector<size_t> _pluginCustomizedCmds;
    std::vector<ScintillaKeyMap> _scintillaKeyCommands;
    std::vector<int> _scintillaModifiedKeyIndices;

    LocalizationSwitcher _localizationSwitcher;
    std::wstring _startWithLocFileName;
    bool _doFunctionListExport = false;
    bool _doPrintAndExit = false;

    ThemeSwitcher _themeSwitcher;
    std::vector<MenuItemUnit> _contextMenuItems;
    std::vector<MenuItemUnit> _tabContextMenuItems;
    DynamicMenu _macroMenuItems;
    DynamicMenu _runMenuItems;
    Session _session;

    std::wstring _shortcutsPath;
    std::wstring _contextMenuPath;
    std::wstring _tabContextMenuPath;
    std::wstring _sessionPath;
    std::wstring _nppPath;
    std::wstring _userPath;
    std::wstring _stylerPath;
    std::wstring _appdataNppDir;
    std::wstring _pluginRootDir;
    std::wstring _pluginConfDir;
    std::wstring _userPluginConfDir;
    std::wstring _currentDirectory;
    std::wstring _workSpaceFilePaths[3];
    std::vector<std::wstring> _fileBrowserRoot;
    std::wstring _fileBrowserSelectedItemPath;

    Accelerator* _pAccelerator = nullptr;
    ScintillaAccelerator* _pScintAccelerator = nullptr;

    FindDlgTabTitles _findDlgTabTitles;
    bool _asNotepadStyle = false;

    winVer _winVersion = WV_UNKNOWN;
    int _platForm = PF_UNKNOWN;
    NativeLangSpeaker *_pNativeLangSpeaker = nullptr;

    QRgb _currentDefaultBgColor = qRgb(0xFF, 0xFF, 0xFF);
    QRgb _currentDefaultFgColor = qRgb(0x00, 0x00, 0x00);

    std::wstring _initialCloudChoice;
    std::wstring _wingupFullPath;
    std::wstring _wingupParams;
    std::wstring _wingupDir;
    bool _isElevationRequired = false;
    bool _isAdminMode = false;
    bool _isRegForOSAppRestartDisabled = false;
    bool _doNppLogNetworkDriveIssue = false;
    bool _isNppAutoUpdateDisabled = false;
    bool _isEndSessionStarted = false;
    bool _isEndSessionCritical = false;
    bool _isPlaceHolderEnabled = false;
    bool _theWarningHasBeenGiven = false;
    int _currentSystemCodepage = -1;

    unsigned long _sintillaModEventMask = npp_sci::SC_MOD_DELETETEXT | npp_sci::SC_MOD_INSERTTEXT | npp_sci::SC_PERFORMED_UNDO | npp_sci::SC_PERFORMED_REDO | npp_sci::SC_MOD_CHANGEINDICATOR;
    enum class ConfXml { lang, styles };

    std::pair<unsigned char, unsigned char> addUserDefineLangsFromXmlTree(NppXml::Document xmldoc);
    void getLangKeywordsFromXmlTree();
    bool getUserParametersFromXmlTree();
    bool getUserStylersFromXmlTree();
    bool updateFromModelXml(NppXml::Element& rootUser, ConfXml whichConf);
    static void updateLangXml(NppXml::Element& mainElemUser, const NppXml::Element& mainElemModel);
    static void updateStylesXml(const NppXml::Element& rootUser, const std::wstring& userDocPath,
                                const NppXml::Element& rootModel, NppXml::Element& mainElemUser,
                                const NppXml::Element& mainElemModel);
    void addDefaultStyles(const NppXml::Element& element);
    int addStyleDefaultColors(NppXml::Element& globalStyleRoot, const std::wstring& name,
                              const std::string& fgColor = "", const std::string& bgColor = "",
                              const std::wstring& fromStyle = L"", const std::string& styleID = "0");
    bool getShortcutsFromXmlTree();
    bool getMacrosFromXmlTree();
    bool getUserCmdsFromXmlTree();
    bool getPluginCmdsFromXmlTree();
    bool getScintKeysFromXmlTree();
    static bool getSessionFromXmlTree(const NppXml::Document& pSessionDoc, Session& session);
    void feedGUIParameters(const NppXml::Element& element);
    void feedKeyWordsParameters(const NppXml::Element& element);
    void feedFileListParameters(const NppXml::Element& element);
    void feedScintillaParam(const NppXml::Element& element);
    void feedDockingManager(const NppXml::Element& element);
    void feedFindHistoryParameters(const NppXml::Element& element);
    void feedProjectPanelsParameters(const NppXml::Element& element);
    void feedFileBrowserParameters(const NppXml::Element& element);
    void feedColumnEditorParameters(const NppXml::Element& element);
    bool feedStylerArray(const NppXml::Element& element);
    std::pair<unsigned char, unsigned char> feedUserLang(const NppXml::Element& element);
    void feedUserStyles(const NppXml::Element& element);
    void feedUserKeywordList(const NppXml::Element& element);
    void feedUserSettings(const NppXml::Element& settingsRoot);
    void feedShortcut(const NppXml::Element& element);
    void feedMacros(const NppXml::Element& element);
    void feedUserCmds(const NppXml::Element& element);
    void feedPluginCustomizedCmds(const NppXml::Element& element);
    void feedScintKeys(const NppXml::Element& element);
    static void getActions(const NppXml::Element& element, Macro& macro);
    static bool getShortcuts(const NppXml::Element& element, Shortcut& sc, std::string* folderName = nullptr);
    static bool getInternalCommandShortcuts(const NppXml::Element& element, CommandShortcut& cs, std::string* folderName = nullptr);
    static void writeStyle2Element(const Style& style2Write, Style& style2Sync, NppXml::Element& element);
    static void insertUserLang2Tree(NppXml::Element& node, const UserLangContainer* userLang);
    static void insertCmd(NppXml::Element& cmdRoot, const CommandShortcut& cmd);
    static void insertMacro(NppXml::Element& macrosRoot, const MacroShortcut& macro, const std::string& folderName);
    static void insertUserCmd(NppXml::Element& userCmdRoot, const UserCommand& userCmd, const std::string& folderName);
    static void insertScintKey(NppXml::Element& scintKeyRoot, const ScintillaKeyMap& scintKeyMap);
    static void insertPluginCmd(NppXml::Element& pluginCmdRoot, const PluginCmdShortcut& pluginCmd);
    static NppXml::Element insertGUIConfigBoolNode(NppXml::Element& r2w, const char* name, bool bVal);
    void insertDockingParamNode(NppXml::Element& GUIRoot) const;
    void writeExcludedLangList(NppXml::Element& element) const;
    void writePrintSetting(NppXml::Element& element) const;
    void initMenuKeys();
    void initScintillaKeys();
    static int getCmdIdFromMenuEntryItemName(void* mainMenuHandle, const std::wstring& menuEntryName, const std::wstring& menuItemName);
    static int getPluginCmdIdFromMenuEntryItemName(void* pluginsMenu, const std::wstring& pluginName, const std::wstring& pluginCmdName);
    winVer getWindowsVersion();
    static void generateXmlFromScratch(const wchar_t* filePathToWrite, const char* content2write);
};

#endif // PARAMETERS_H
