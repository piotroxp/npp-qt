// This file is part of Notepad++ Qt port
// Copyright (C)2024 Qt Notepad++ Team

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

#include <memory>
#include <QWidget>
#include <QPointer>
#include <QMenu>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QKeyEvent>

#include <ScintillaEdit.h>
#include <SciLexer.h>
#include <Scintilla.h>

#include "Buffer.h"
#include "Common.h"
#include "Parameters.h"

class ScintillaEditView;
class ScintillaEdit;

#define WM_DOCK_USERDEFINE_DLG      (SCINTILLA_USER + 1)
#define WM_UNDOCK_USERDEFINE_DLG    (SCINTILLA_USER + 2)
#define WM_CLOSE_USERDEFINE_DLG     (SCINTILLA_USER + 3)
#define WM_REMOVE_USERLANG          (SCINTILLA_USER + 4)
#define WM_RENAME_USERLANG          (SCINTILLA_USER + 5)
#define WM_REPLACEALL_INOPENEDDOC   (SCINTILLA_USER + 6)
#define WM_FINDALL_INOPENEDDOC      (SCINTILLA_USER + 7)
#define WM_DOOPEN                   (SCINTILLA_USER + 8)
#define WM_FINDINFILES              (SCINTILLA_USER + 9)
#define WM_REPLACEINFILES           (SCINTILLA_USER + 10)
#define WM_FINDALL_INCURRENTDOC     (SCINTILLA_USER + 11)
#define WM_FRSAVE_INT               (SCINTILLA_USER + 12)
#define WM_FRSAVE_STR               (SCINTILLA_USER + 13)
#define WM_FINDALL_INCURRENTFINDER  (SCINTILLA_USER + 14)
#define WM_FINDINPROJECTS           (SCINTILLA_USER + 15)
#define WM_REPLACEINPROJECTS        (SCINTILLA_USER + 16)

// Codepage constants
inline constexpr int CP_CHINESE_TRADITIONAL = 950;
inline constexpr int CP_CHINESE_SIMPLIFIED = 936;
inline constexpr int CP_JAPANESE = 932;
inline constexpr int CP_KOREAN = 949;
inline constexpr int CP_GREEK = 1253;

// Word list definitions
#define LIST_NONE 0
#define LIST_0 1
#define LIST_1 2
#define LIST_2 4
#define LIST_3 8
#define LIST_4 16
#define LIST_5 32
#define LIST_6 64
#define LIST_7 128
#define LIST_8 256

inline constexpr int NB_FOLDER_STATE = 7;
inline constexpr int MARK_BOOKMARK = 20;
inline constexpr int MARK_HIDELINESBEGIN = 19;
inline constexpr int MARK_HIDELINESEND = 18;

size_t getNbDigits(size_t aNum, size_t base);

template<typename T>
T* variedFormatNumber2String(T* str, size_t strLen, size_t number, size_t base, bool useUpper, size_t nbDigits, ColumnEditorParam::LeadingChoice lead);

struct ColumnModeInfo {
    intptr_t _selLpos = 0;
    intptr_t _selRpos = 0;
    intptr_t _order = -1;
    bool _isDirectionL2R = true;
    intptr_t _nbVirtualAnchorSpc = 0;
    intptr_t _nbVirtualCaretSpc = 0;

    ColumnModeInfo(intptr_t lPos, intptr_t rPos, intptr_t order, bool isL2R = true,
                   intptr_t vAnchorNbSpc = 0, intptr_t vCaretNbSpc = 0)
        : _selLpos(lPos), _selRpos(rPos), _order(order), _isDirectionL2R(isL2R),
          _nbVirtualAnchorSpc(vAnchorNbSpc), _nbVirtualCaretSpc(vCaretNbSpc) {}

    bool isValid() const {
        return (_order >= 0 && _selLpos >= 0 && _selRpos >= 0 && _selLpos <= _selRpos);
    }
};

struct SortInSelectOrder {
    bool operator()(const ColumnModeInfo& l, const ColumnModeInfo& r) {
        return (l._order < r._order);
    }
};

struct SortInPositionOrder {
    bool operator()(const ColumnModeInfo& l, const ColumnModeInfo& r) {
        return (l._selLpos < r._selLpos);
    }
};

typedef std::vector<ColumnModeInfo> ColumnModeInfos;

struct LanguageNameInfo {
    const wchar_t* _langName = nullptr;
    const wchar_t* _shortName = nullptr;
    const wchar_t* _longName = nullptr;
    LangType _langID = LangType::L_TEXT;
    const char* _lexerID = nullptr;
};

#define URL_INDIC 8

class ISorter;

/**
 * @class ScintillaEditView
 * @brief Main editing widget wrapping Scintilla for Notepad++
 *
 * This class provides the core text editing functionality for Notepad++.
 * It wraps Scintilla (via bundled ScintillaEdit) and provides Notepad++ specific
 * features like buffer management, language styling, and editing commands.
 */
class ScintillaEditView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isMainEditZone READ isMainEditZone WRITE setMainEditZone)

    friend class Finder;

public:
    /**
     * @brief Default constructor
     */
    ScintillaEditView();

    /**
     * @brief Constructor with main edit zone flag
     * @param isMainEditZone Whether this is the main editing zone
     */
    explicit ScintillaEditView(bool isMainEditZone);

    /**
     * @brief Virtual destructor
     */
    ~ScintillaEditView() override;

    /**
     * @brief Initialize the scintilla view
     * @param hInst Application instance (unused in Qt)
     * @param hParent Parent window
     */
    void init(HINSTANCE hInst, HWND hParent);

    /**
     * @brief Get the underlying ScintillaEdit pointer
     * @return Pointer to ScintillaEdit
     */
    ScintillaEdit* sci() { return _sci; }
    const ScintillaEdit* sci() const { return _sci; }

    /**
     * @brief Get the native widget handle
     * @return QWidget pointer (HWND shim)
     */
    HWND getHSelf() const { return asHWND(_sci); }

    /**
     * @brief Execute a Scintilla message
     * @param Msg Message ID
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return Result of the message
     */
    LRESULT execute(sptr_t Msg, uptr_t wParam = 0, sptr_t lParam = 0) const;

    /**
     * @brief Activate a buffer in this view
     * @param buffer Buffer to activate
     * @param force Force activation even if already active
     */
    void activateBuffer(BufferID buffer, bool force);

    /**
     * @brief Get text from a range
     * @param dest Destination buffer
     * @param start Start position
     * @param end End position
     */
    void getText(char* dest, size_t start, size_t end) const;
    void getGenericText(char* dest, size_t destlen, size_t start, size_t end) const;
    void getGenericText(wchar_t* dest, size_t destlen, size_t start, size_t end) const;
    void getGenericText(wchar_t* dest, size_t destlen, size_t start, size_t end,
                        intptr_t* mstart, intptr_t* mend, intptr_t* outLen = nullptr) const;
    std::wstring getGenericTextAsString(size_t start, size_t end) const;
    void insertGenericTextFrom(size_t position, const char* text2insert) const;
    void insertGenericTextFrom(size_t position, const wchar_t* text2insert) const;
    void replaceSelWith(const char* replaceText);

    intptr_t getSelectedTextCount() {
        auto range = getSelection();
        return (range.cpMax - range.cpMin);
    }

    void getVisibleStartAndEndPosition(intptr_t* startPos, intptr_t* endPos);
    char* getWordFromRange(char* txt, size_t size, size_t pos1, size_t pos2);
    char* getSelectedTextToMultiChar(char* txt, size_t size, bool expand = true);
    std::wstring getSelectedTextToWChar(bool expand = true, Sci_Position* selCharNumber = nullptr);
    char* getWordOnCaretPos(char* txt, size_t size);

    // Search functionality
    intptr_t searchInTarget(const std::string_view& text2Find, size_t fromPos, size_t toPos) const;
    intptr_t searchInTarget(const wchar_t* text2Find, size_t lenOfText2Find, size_t fromPos, size_t toPos) const;

    // Text manipulation
    void appendGenericText(const wchar_t* text2Append) const;
    void addGenericText(const wchar_t* text2Append) const;
    void addGenericText(const wchar_t* text2Append, intptr_t* mstart, intptr_t* mend) const;
    intptr_t replaceTarget(const std::string& str2replace, intptr_t fromTargetPos = -1, intptr_t toTargetPos = -1) const;
    intptr_t replaceTarget(const wchar_t* str2replace, intptr_t fromTargetPos = -1, intptr_t toTargetPos = -1) const;
    intptr_t replaceTargetRegExMode(const wchar_t* re, intptr_t fromTargetPos = -1, intptr_t toTargetPos = -1) const;

    // Auto-completion
    void showAutoCompletion(size_t lenEntered, const std::string& list) const;
    void showCallTip(size_t startPos, const std::string& def) const;

    // Line operations
    std::wstring getLine(size_t lineNumber) const;
    void getLine(size_t lineNumber, char* line, size_t lineBufferLen) const;
    void getLine(size_t lineNumber, wchar_t* line, size_t lineBufferLen) const;
    void addText(size_t length, const char* buf);

    void insertNewLineAboveCurrentLine();
    void insertNewLineBelowCurrentLine();

    // Position management
    void saveCurrentPos();
    void restoreCurrentPosPreStep();
    void restoreCurrentPosPostStep();

    // Selection
    void beginOrEndSelect(bool isColumnMode);
    bool beginEndSelectedIsStarted() const {
        return _beginSelectPosition != -1;
    }

    size_t getCurrentDocLen() const {
        return size_t(execute(SCI_GETLENGTH));
    }

    Sci_CharacterRangeFull getSelection() const {
        Sci_CharacterRangeFull crange{};
        crange.cpMin = execute(SCI_GETSELECTIONSTART);
        crange.cpMax = execute(SCI_GETSELECTIONEND);
        return crange;
    }

    void getWordToCurrentPos(char* str, intptr_t strLen) const;
    void getWordToCurrentPos(wchar_t* str, intptr_t strLen) const;

    // User-defined dialog
    static void doUserDefineDlg(bool willBeShown = true, bool isRTL = false);
    static class UserDefineDialog* getUserDefineDlg() { return _userDefineDlg; }
    void beSwitched();

    // Margin settings
    static const int _SC_MARGE_LINENUMBER;
    static const int _SC_MARGE_SYMBOL;
    static const int _SC_MARGE_FOLDER;
    static const int _SC_MARGE_CHANGEHISTORY;

    void showMargin(int whichMarge, bool willBeShown = true);
    void showChangeHistoryMargin(bool willBeShown = true);
    bool hasMarginShown(int witchMarge);

    // Style settings
    void setMakerStyle(int style);
    void setWrapMode(lineWrapMethod meth);
    void showWSAndTab(bool willBeShowed = true);
    bool isShownSpaceAndTab();
    void showEOL(bool willBeShowed = true);
    bool isShownEol();
    void showNpc(bool willBeShown = true, bool isSearchResult = false);
    static bool isShownNpc();
    void maintainStateForNpc();
    void showCcUniEol(bool willBeShown = true, bool isSearchResult = false);
    static bool isShownCcUniEol();
    void showInvisibleChars(bool willBeShown = true);
    void showIndentGuideLine(bool willBeShown = true);
    bool isShownIndentGuide() const;
    void wrap(bool willBeWrapped = true);
    bool isWrap() const;
    bool isWrapSymbolVisible() const;
    void showWrapSymbol(bool willBeShown = true);

    // Position and line info
    intptr_t getCurrentLineNumber() const;
    intptr_t lastZeroBasedLineNumber() const;
    intptr_t getCurrentXOffset() const;
    void setCurrentXOffset(long xOffset);
    void scroll(intptr_t column, intptr_t line);
    intptr_t getCurrentPointX() const;
    intptr_t getCurrentPointY() const;
    intptr_t getTextHeight() const;
    int getTextZoneWidth() const;
    void gotoLine(intptr_t line);
    intptr_t getCurrentColumnNumber() const;
    std::pair<size_t, size_t> getSelectedCharsAndLinesCount(long long maxSelectionsForLineCount = -1) const;
    size_t getUnicodeSelectedLength() const;
    intptr_t getLineLength(size_t line) const;
    intptr_t getLineIndent(size_t line) const;
    void setLineIndent(size_t line, size_t indent) const;

    // Line number margin
    void updateLineNumbersMargin(bool forcedToHide);
    void updateLineNumberWidth();
    void performGlobalStyles();

    std::pair<size_t, size_t> getSelectionLinesRange(intptr_t selectionNumber = -1) const;
    void currentLinesUp() const;
    void currentLinesDown() const;

    // Text case operations
    intptr_t caseConvertRange(intptr_t start, intptr_t end, TextCase caseToConvert);
    static void changeCase(wchar_t* const strWToConvert, const int& nbChars, const TextCase& caseToConvert);
    void convertSelectedTextTo(const TextCase& caseToConvert);
    void setMultiSelections(const ColumnModeInfos& cmi);

    void convertSelectedTextToLowerCase();
    void convertSelectedTextToUpperCase();
    void convertSelectedTextToNewerCase(const TextCase& caseToConvert);

    // Folding
    void getCurrentFoldStates(std::vector<size_t>& lineStateVector);
    void syncFoldStateWith(const std::vector<size_t>& lineStateVectorNew);
    bool isFoldIndentationBased() const;
    void foldIndentationBasedLevel(int level2Collapse, bool mode);
    void foldLevel(int level2Collapse, bool mode);
    void foldAll(bool mode);
    void fold(size_t line, bool mode, bool shouldBeNotified = true);
    bool isFolded(size_t line) const;
    void expand(size_t& line, bool doExpand, bool force = false, intptr_t visLevels = 0, intptr_t level = -1);
    bool isCurrentLineFolded() const;
    void foldCurrentPos(bool mode);

    int getCodepage() const { return _codepage; }
    bool isCJK() const {
        return ((_codepage == CP_CHINESE_TRADITIONAL) || (_codepage == CP_CHINESE_SIMPLIFIED) ||
                (_codepage == CP_JAPANESE) || (_codepage == CP_KOREAN));
    }

    // Column mode
    ColumnModeInfos getColumnModeSelectInfo();
    void columnReplace(ColumnModeInfos& cmi, const char* str) const;
    void columnReplace(ColumnModeInfos& cmi, size_t initial, size_t incr, size_t repeat,
                        NumBase format, ColumnEditorParam::LeadingChoice lead) const;

    // Indicators
    void clearIndicator(int indicatorNumber);
    bool getIndicatorRange(size_t indicatorNumber, size_t* from = nullptr, size_t* to = nullptr, size_t* cur = nullptr);

    static LanguageNameInfo _langNameInfoArray[101];

    // Buffer management
    void bufferUpdated(Buffer* buffer, int mask);
    BufferID getCurrentBufferID() const { return _currentBufferID; }
    Buffer* getCurrentBuffer() const { return _currentBuffer; }
    void setCurrentBuffer(Buffer* buf2set) { _currentBuffer = buf2set; }
    void styleChange();

    // Line hiding
    void hideLines();
    bool hidelineMarkerClicked(intptr_t lineNumber);
    void notifyHidelineMarkers(Buffer* buf, bool isHide, size_t location, bool del);
    void hideMarkedLines(size_t searchStart, bool toEndOfDoc) const;
    void showHiddenLines(size_t searchStart, bool toEndOfDoc, bool doDelete) const;
    void restoreHiddenLines();

    bool hasSelection() const { return !execute(SCI_GETSELECTIONEMPTY); }

    static bool isPythonStyleIndentation(LangType typeDoc);

    // Document type
    void defineDocType(LangType typeDoc);
    void addCustomWordChars() const;
    void restoreDefaultWordChars() const;
    void setWordChars() const;
    void setCRLF(long color = -1) const;
    void setNpcAndCcUniEOL(long color = -1) const;
    void setTabSettings(Lang* lang);

    bool isWrapRestoreNeeded() const { return _wrapRestoreNeeded; }
    void setWrapRestoreNeeded(bool isWrapRestoreNeeded) { _wrapRestoreNeeded = isWrapRestoreNeeded; }

    // Scrolling
    void scrollPosToCenter(size_t pos);

    // EOL
    std::wstring getEOLString() const;

    // Border edge
    void setBorderEdge(bool doWithBorderEdge);

    // Sorting
    void sortLines(size_t fromLine, size_t toLine, ISorter* pSort);

    // Text direction
    void changeTextDirection(bool isRTL);
    bool isTextDirectionRTL() const;
    void setPositionRestoreNeeded(bool val) { _positionRestoreNeeded = val; }

    // Clipboard operations
    void markedTextToClipboard(int indiStyle, bool doAll = false);

    // Duplicate lines
    void removeAnyDuplicateLines();

    // Word selection
    bool expandWordSelection();
    bool pasteToMultiSelection() const;

    // Element colors
    void setElementColour(int element, COLORREF color) const { execute(SCI_SETELEMENTCOLOUR, element, color | 0xFF000000); }

    // Focus handling
    void grabFocus() const { if (_sci) _sci->setFocus(true); }

public slots:
    void destroy();

signals:
    void fileDropped(const QString& filePath);
    void contextMenuRequested(const QPoint& pos);
    void doubleClicked(int position, int modifiers);

protected:
    // Scintilla notification handler
    void handleNotification(SCNotification* scn);

private:
    QPointer<ScintillaEdit> _sci;

    // Class-level initialization tracking
    static bool _SciInit;
    static int _refCount;

    // Static user define dialog
    static class UserDefineDialog* _userDefineDlg;

    // Marker arrays
    static const int _markersArray[][NB_FOLDER_STATE];

    // State
    bool _isMainEditZone = false;
    int _codepage = 0;
    bool _wrapRestoreNeeded = false;
    bool _positionRestoreNeeded = false;
    uint32_t _restorePositionRetryCount = 0;

    // Selection tracking
    intptr_t _beginSelectPosition = -1;
    static std::string _defaultCharList;
    bool _isMultiPasteActive = false;

    // Buffer tracking
    BufferID _currentBufferID = nullptr;
    Buffer* _currentBuffer = nullptr;

    // Document lifecycle
    BufferID attachDefaultDoc();

    // Lexer and styling methods
    void restyleBuffer();
    static const char* concatToBuildKeywordList(std::string& kwl, LangType langType, int keywordIndex);
    void setKeywords(LangType langType, const char* keywords, int index) const;
    void populateSubStyleKeywords(LangType langType, int baseStyleID, int numSubStyles,
                                   int firstLangIndex, const char** pKwArray) const;
    void setLexer(LangType langType, int whichList, int baseStyleID = STYLE_NOT_USED,
                   int numSubStyles = 8) const;
    bool setLexerFromLangID(int langID) const;
    void makeStyle(LangType langType, const char** keywordArray = nullptr) const;
    void setStyle(Style styleToSet) const;
    void setSpecialStyle(const Style& styleToSet) const;
    void setSpecialIndicator(const Style& styleToSet) const;

    // Complex lexers
    void setXmlLexer(LangType type) const;
    void setCppLexer(LangType langType) const;
    void setHTMLLexer() const;
    void setJsLexer() const;
    void setTclLexer() const;
    void setObjCLexer(LangType langType) const;
    void setUserLexer(const wchar_t* userLangName = nullptr) const;
    void setExternalLexer(LangType typeDoc) const;
    void setEmbeddedJSLexer() const;
    void setEmbeddedPhpLexer() const;
    void setEmbeddedAspLexer() const;
    void setJsonLexer(bool isJson5 = false) const;
    void setTypeScriptLexer() const;

    // Simple lexers
    void setCssLexer();
    void setLuaLexer();
    void setMakefileLexer();
    void setPropsLexer(bool isPropsButNotIni = true);
    void setSqlLexer();
    void setMSSqlLexer();
    void setBashLexer() const;
    void setVBLexer();
    void setPascalLexer();
    void setPerlLexer();
    void setPythonLexer();
    void setGDScriptLexer();
    void setBatchLexer();
    void setTeXLexer();
    void setNsisLexer();
    void setFortranLexer();
    void setFortran77Lexer();
    void setLispLexer();
    void setSchemeLexer();
    void setAsmLexer();
    void setDiffLexer();
    void setPostscriptLexer();
    void setRubyLexer();
    void setSmalltalkLexer();
    void setVhdlLexer();
    void setKixLexer();
    void setAutoItLexer();
    void setCamlLexer();
    void setAdaLexer();
    void setVerilogLexer();
    void setMatlabLexer();
    void setHaskellLexer();
    void setInnoLexer();
    void setCmakeLexer();
    void setYamlLexer();
    void setCobolLexer();
    void setGui4CliLexer();
    void setDLexer();
    void setPowerShellLexer();
    void setRLexer();
    void setCoffeeScriptLexer();
    void setBaanCLexer();
    void setSrecLexer();
    void setIHexLexer();
    void setTEHexLexer();
    void setAsn1Lexer();
    void setAVSLexer();
    void setBlitzBasicLexer();
    void setPureBasicLexer();
    void setFreeBasicLexer();
    void setCsoundLexer();
    void setErlangLexer();
    void setESCRIPTLexer();
    void setForthLexer();
    void setLatexLexer();
    void setMMIXALLexer();
    void setNimrodLexer();
    void setNncrontabLexer();
    void setOScriptLexer();
    void setREBOLLexer();
    void setRegistryLexer();
    void setRustLexer();
    void setSpiceLexer();
    void setTxt2tagsLexer();
    void setVisualPrologLexer();
    void setHollywoodLexer();
    void setRakuLexer();
    void setTomlLexer();
    void setSasLexer();
    void setErrorListLexer();
    void setSearchResultLexer();

    // Marker definition
    void defineMarker(int marker, int markerType, COLORREF fore, COLORREF back, COLORREF foreActive);

    // Charset mapping
    int codepage2CharSet() const;

    std::pair<size_t, size_t> getWordRange();
    static void getFoldColor(COLORREF& fgColor, COLORREF& bgColor, COLORREF& activeFgColor);

    bool isMainEditZone() const { return _isMainEditZone; }
    void setMainEditZone(bool val) { _isMainEditZone = val; }
};

