// ScintillaEditView.cpp - Qt port
#include "ScintillaEditView.h"
#include "ScintillaCtrls.h"
#include "UserDefineDialog.h"
#include "Parameters.h"
#include "NppConstants.h"
#include "Colors.h"
#include "Utf8_16.h"
#include <QApplication>
#include <QBoxLayout>
#include <climits>

bool ScintillaEditView::_SciInit = false;
int ScintillaEditView::_refCount = 0;
UserDefineDialog* ScintillaEditView::_userDefineDlg = nullptr;
std::string ScintillaEditView::_defaultCharList;

const int ScintillaEditView::_markersArray[][NB_FOLDER_STATE] = {
    { SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND, SC_MARKNUM_FOLDERMIDTAIL, SC_MARKNUM_FOLDERMIDEND, SC_MARKNUM_VERTLINE },
    { SC_MARKNUM_BOXPLUS, SC_MARKNUM_BOXMINUS, SC_MARKNUM_BOXPLUSCONNECTED, SC_MARKNUM_BOXMINUSCONNECTED, SC_MARKNUM_LINES, SC_MARKNUM_MINUS, SC_MARKNUM_EMPTY },
};

const int ScintillaEditView::_SC_MARGE_LINENUMBER = 0;
const int ScintillaEditView::_SC_MARGE_SYMBOL = 1;
const int ScintillaEditView::_SC_MARGE_FOLDER = 2;
const int ScintillaEditView::_SC_MARGE_CHANGEHISTORY = 3;

ScintillaEditView::ScintillaEditView() : QWidget() {
    ++_refCount;
    init(nullptr, nullptr);
}

ScintillaEditView::ScintillaEditView(bool isMainEditZone) : QWidget(), _isMainEditZone(isMainEditZone) {
    ++_refCount;
    init(nullptr, nullptr);
}

ScintillaEditView::~ScintillaEditView() {
    --_refCount;
    if (!_refCount && _SciInit)
        _SciInit = false;
    delete _sci;
}

void ScintillaEditView::init(HINSTANCE, HWND) {
    _sci = new ScintillaEdit(this);
    _sci->setUtf8(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_sci);

    if (!_SciInit)
        _SciInit = true;

    setAcceptDrops(true);
    _sci->setAcceptDrops(false);

    execute(SCI_SETMARGINMASKN, _SC_MARGE_FOLDER, SC_MASK_FOLDERS);
}

void ScintillaEditView::destroy() {
    if (_sci) {
        _sci->deleteLater();
        _sci = nullptr;
    }
}

sptr_t ScintillaEditView::execute(sptr_t Msg, uptr_t wParam, sptr_t lParam) const {
    return _sci ? _sci->send(Msg, wParam, lParam) : -1;
}

void ScintillaEditView::activateBuffer(BufferID buffer, bool) {
    _currentBufferID = buffer;
}

void ScintillaEditView::getText(char* dest, size_t start, size_t end) const {
    getGenericText(dest, end - start + 1, start, end);
}

void ScintillaEditView::getGenericText(char* dest, size_t destlen, size_t start, size_t end) const {
    if (!_sci || destlen == 0) return;

    QByteArray data = _sci->textRange(static_cast<int>(start), static_cast<int>(end));
    qstrncpy(dest, data.constData(), qMin(static_cast<size_t>(data.size()), destlen - 1));
    dest[destlen - 1] = '\0';
}

void ScintillaEditView::getGenericText(wchar_t* dest, size_t destlen, size_t start, size_t end) const {
    if (!_sci || destlen == 0) return;

    QByteArray data = _sci->textRange(static_cast<int>(start), static_cast<int>(end));
    QString str = QString::fromLatin1(data);
    QString subset = str.mid(static_cast<int>(start), static_cast<int>(end - start));
    qstrncpy(reinterpret_cast<QChar*>(dest), subset.utf16(), qMin(static_cast<size_t>(subset.size()), destlen - 1));
    dest[destlen - 1] = L'\0';
}

void ScintillaEditView::getGenericText(wchar_t*, size_t, size_t, size_t, intptr_t*, intptr_t*, intptr_t*) const {}

std::wstring ScintillaEditView::getGenericTextAsString(size_t start, size_t end) const {
    if (!_sci) return std::wstring();

    QByteArray data = _sci->textRange(static_cast<int>(start), static_cast<int>(end));
    QString str = QString::fromLatin1(data);
    return str.mid(static_cast<int>(start), static_cast<int>(end - start)).toStdWString();
}

void ScintillaEditView::insertGenericTextFrom(size_t position, const char* text2insert) const {
    if (_sci) _sci->insertText(position, text2insert);
}

void ScintillaEditView::insertGenericTextFrom(size_t, const wchar_t*) const {}

void ScintillaEditView::replaceSelWith(const char* replaceText) {
    if (_sci) _sci->replaceSel(replaceText);
}

void ScintillaEditView::getVisibleStartAndEndPosition(intptr_t* startPos, intptr_t* endPos) {
    if (!_sci || !startPos || !endPos) return;
    *startPos = execute(SCI_GETFIRSTVISIBLELINE);
    *endPos = execute(SCI_GETLASTVISIBLELINE);
}

char* ScintillaEditView::getWordFromRange(char* txt, size_t size, size_t pos1, size_t pos2) {
    if (!_sci || !txt || size == 0) return nullptr;

    QByteArray data = _sci->textRange(0, INT_MAX);
    QString str = QString::fromLatin1(data);
    QString word = str.mid(static_cast<int>(pos1), static_cast<int>(pos2 - pos1));
    qstrncpy(txt, word.toLatin1().constData(), size - 1);
    txt[size - 1] = '\0';
    return txt;
}

char* ScintillaEditView::getSelectedTextToMultiChar(char* txt, size_t, bool) {
    if (!_sci || !txt) return nullptr;

    QByteArray selected = _sci->getSelText();
    if (selected.isEmpty()) {
        txt[0] = '\0';
        return txt;
    }
    qstrncpy(txt, selected.constData(), 8192);
    return txt;
}

std::wstring ScintillaEditView::getSelectedTextToWChar(bool, Sci_Position*) {
    std::wstring result;
    if (!_sci) return result;

    QByteArray selected = _sci->getSelText();
    if (!selected.isEmpty()) {
        result = QString::fromLatin1(selected).toStdWString();
    }
    return result;
}

char* ScintillaEditView::getWordOnCaretPos(char* txt, size_t size) {
    if (!_sci || !txt || size == 0) return nullptr;

    sptr_t pos = _sci->currentPos();
    sptr_t wordStart = _sci->wordStartPosition(pos, true);
    sptr_t wordEnd = _sci->wordEndPosition(pos, true);

    QByteArray data = _sci->textRange(static_cast<int>(wordStart), static_cast<int>(wordEnd));
    qstrncpy(txt, data.constData(), size - 1);
    txt[size - 1] = '\0';
    return txt;
}

intptr_t ScintillaEditView::searchInTarget(const std::string_view& text2Find, size_t fromPos, size_t toPos) const {
    if (!_sci) return -1;

    QByteArray searchText(text2Find.data(), static_cast<int>(text2Find.size()));
    _sci->setTargetStart(static_cast<sptr_t>(fromPos));
    _sci->setTargetEnd(static_cast<sptr_t>(toPos));
    return _sci->searchInTarget(searchText.size(), searchText.constData());
}

intptr_t ScintillaEditView::searchInTarget(const wchar_t*, size_t, size_t, size_t) const {
    return -1;
}

void ScintillaEditView::appendGenericText(const wchar_t* text2Append) const {
    if (!_sci || !text2Append) return;
    sptr_t endPos = _sci->length();
    QString qstr = QString::fromUtf16(reinterpret_cast<const ushort*>(text2Append));
    _sci->insertText(endPos, qstr.toLatin1().constData());
}

void ScintillaEditView::addGenericText(const wchar_t* text2Append) const {
    appendGenericText(text2Append);
}

void ScintillaEditView::addGenericText(const wchar_t*, intptr_t*, intptr_t*) const {}

intptr_t ScintillaEditView::replaceTarget(const std::string& str2replace, intptr_t, intptr_t) const {
    if (!_sci) return -1;
    return _sci->replaceTarget(static_cast<sptr_t>(str2replace.size()), str2replace.data());
}

intptr_t ScintillaEditView::replaceTarget(const wchar_t* str2replace, intptr_t, intptr_t) const {
    if (!_sci || !str2replace) return -1;
    QString qstr = QString::fromUtf16(reinterpret_cast<const ushort*>(str2replace));
    QByteArray data = qstr.toLatin1();
    return _sci->replaceTarget(data.size(), data.constData());
}

intptr_t ScintillaEditView::replaceTargetRegExMode(const wchar_t* re, intptr_t fp, intptr_t tp) const {
    execute(SCI_SETSEARCHFLAGS, SCFIND_REGEXP | SCFIND_POSIX);
    return replaceTarget(re, fp, tp);
}

void ScintillaEditView::showAutoCompletion(size_t lenEntered, const std::string& list) const {
    if (!_sci) return;
    QByteArray qlist = QByteArray::fromStdString(list);
    _sci->autoCShow(static_cast<sptr_t>(lenEntered), qlist.constData());
}

void ScintillaEditView::showCallTip(size_t startPos, const std::string& def) const {
    if (!_sci) return;
    QByteArray qdef = QByteArray::fromStdString(def);
    _sci->callTipShow(static_cast<sptr_t>(startPos), qdef.constData());
}

std::wstring ScintillaEditView::getLine(size_t lineNumber) const {
    std::wstring result;
    if (!_sci) return result;

    QByteArray lineData = _sci->getLine(static_cast<sptr_t>(lineNumber));
    result = QString::fromLatin1(lineData).toStdWString();
    return result;
}

void ScintillaEditView::getLine(size_t lineNumber, char* line, size_t lineBufferLen) const {
    if (!_sci || !line || lineBufferLen == 0) return;

    QByteArray lineData = _sci->getLine(static_cast<sptr_t>(lineNumber));
    if (!lineData.isEmpty()) {
        qstrncpy(line, lineData.constData(), lineBufferLen - 1);
        line[lineBufferLen - 1] = '\0';
    } else {
        line[0] = '\0';
    }
}

void ScintillaEditView::getLine(size_t lineNumber, wchar_t* line, size_t lineBufferLen) const {
    if (!_sci || !line || lineBufferLen == 0) return;

    QByteArray lineData = _sci->getLine(static_cast<sptr_t>(lineNumber));
    if (!lineData.isEmpty()) {
        QString str = QString::fromLatin1(lineData);
        qstrncpy(reinterpret_cast<QChar*>(line), str.utf16(), lineBufferLen - 1);
        line[lineBufferLen - 1] = L'\0';
    } else {
        line[0] = L'\0';
    }
}

void ScintillaEditView::addText(size_t length, const char* buf) {
    if (!_sci || !buf) return;
    _sci->appendText(static_cast<sptr_t>(length), buf);
}

void ScintillaEditView::insertNewLineAboveCurrentLine() {
    if (!_sci) return;
    sptr_t pos = _sci->currentPos();
    sptr_t line = _sci->lineFromPosition(pos);
    sptr_t lineStart = _sci->positionFromLine(line);
    _sci->insertText(lineStart, "\n");
}

void ScintillaEditView::insertNewLineBelowCurrentLine() {
    if (!_sci) return;
    sptr_t pos = _sci->currentPos();
    sptr_t line = _sci->lineFromPosition(pos);
    sptr_t lineEnd = _sci->lineEndPosition(line);
    _sci->insertText(lineEnd, "\n");
}

void ScintillaEditView::saveCurrentPos() {}
void ScintillaEditView::restoreCurrentPosPreStep() {}
void ScintillaEditView::restoreCurrentPosPostStep() {}

void ScintillaEditView::beginOrEndSelect(bool isColumnMode) {
    if (!_sci) return;
    if (_beginSelectPosition == -1) {
        _beginSelectPosition = _sci->currentPos();
        if (isColumnMode)
            _sci->setSelectionMode(SC_SEL_RECTANGLE);
    } else {
        _beginSelectPosition = -1;
    }
}

void ScintillaEditView::doUserDefineDlg(bool willBeShown, bool) {
    if (_userDefineDlg)
        _userDefineDlg->doDialog(willBeShown);
}

void ScintillaEditView::beSwitched() {
    if (_userDefineDlg)
        _userDefineDlg->setScintilla(this);
}

void ScintillaEditView::showMargin(int whichMarge, bool willBeShown) {
    if (!_sci) return;
    int width = willBeShown ? 20 : 0;
    execute(SCI_SETMARGINWIDTHN, whichMarge, width);
}

void ScintillaEditView::showChangeHistoryMargin(bool willBeShown) {
    showMargin(_SC_MARGE_CHANGEHISTORY, willBeShown);
}

bool ScintillaEditView::hasMarginShown(int witchMarge) {
    return execute(SCI_GETMARGINWIDTHN, witchMarge, 0) != 0;
}

void ScintillaEditView::setMakerStyle(folderStyle style) {
    bool display = style != FOLDER_STYLE_NONE;
    if (style == FOLDER_STYLE_NONE)
        style = FOLDER_STYLE_BOX;

    COLORREF foldfgColor = 0x00FFFFFF;
    COLORREF foldbgColor = 0x00808080;
    COLORREF activeFoldFgColor = 0x00FF0000;
    getFoldColor(foldfgColor, foldbgColor, activeFoldFgColor);

    for (int i = 0; i < NB_FOLDER_STATE; ++i)
        defineMarker(_markersArray[FOLDER_TYPE][i], SC_MARK_PLUS, foldfgColor, foldbgColor, activeFoldFgColor);

    showMargin(_SC_MARGE_FOLDER, display);
}

void ScintillaEditView::setWrapMode(lineWrapMethod meth) {
    if (!_sci) return;
    int mode = (meth == LINEWRAP_ALIGNED) ? SC_WRAPINDENT_SAME :
               (meth == LINEWRAP_INDENT) ? SC_WRAPINDENT_INDENT : SC_WRAPINDENT_FIXED;
    execute(SCI_SETWRAPINDENTMODE, mode);
}

void ScintillaEditView::showWSAndTab(bool willBeShowed) {
    if (!_sci) return;
    execute(SCI_SETVIEWWS, willBeShowed ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    execute(SCI_SETWHITESPACESIZE, 2, 0);
}

bool ScintillaEditView::isShownSpaceAndTab() {
    return _sci && execute(SCI_GETVIEWWS) != 0;
}

void ScintillaEditView::showEOL(bool willBeShowed) {
    if (_sci) execute(SCI_SETVIEWEOL, willBeShowed);
}

bool ScintillaEditView::isShownEol() {
    return _sci && execute(SCI_GETVIEWEOL) != 0;
}

void ScintillaEditView::showNpc(bool willBeShown, bool) {
    if (_sci) execute(SCI_SETVIEWWS, willBeShown ? SC_SHOW_ALL : SC_SHOW_NO_SPECIAL);
}

bool ScintillaEditView::isShownNpc() {
    return false;
}

void ScintillaEditView::maintainStateForNpc() {}

void ScintillaEditView::showCcUniEol(bool willBeShowed, bool) {
    if (_sci) execute(SCI_SETVIEWWS, willBeShowed ? SC_SHOW_ALL : SC_SHOW_NO_SPECIAL);
}

bool ScintillaEditView::isShownCcUniEol() {
    return false;
}

void ScintillaEditView::showInvisibleChars(bool willBeShown) {
    showNpc(willBeShown);
    showCcUniEol(willBeShown);
    showWSAndTab(willBeShown);
    showEOL(willBeShown);
}

void ScintillaEditView::showIndentGuideLine(bool willBeShown) {
    if (_sci) execute(SCI_SETINDENTATIONGUIDES, willBeShown ? SC_IV_LOOKBOTH : SC_IV_NONE);
}

bool ScintillaEditView::isShownIndentGuide() const {
    return _sci && execute(SCI_GETINDENTATIONGUIDES) != 0;
}

void ScintillaEditView::wrap(bool willBeWrapped) {
    if (_sci) execute(SCI_SETWRAPMODE, willBeWrapped ? SC_WRAP_WORD : SC_WRAP_NONE);
}

bool ScintillaEditView::isWrap() const {
    return _sci && execute(SCI_GETWRAPMODE) == SC_WRAP_WORD;
}

bool ScintillaEditView::isWrapSymbolVisible() const {
    return _sci && execute(SCI_GETWRAPVISUALFLAGS) != SC_WRAPVISUALFLAG_NONE;
}

void ScintillaEditView::showWrapSymbol(bool willBeShown) {
    if (_sci) {
        execute(SCI_SETWRAPVISUALFLAGSLOCATION, SC_WRAPVISUALFLAGLOC_DEFAULT);
        execute(SCI_SETWRAPVISUALFLAGS, willBeShown ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    }
}

intptr_t ScintillaEditView::getCurrentLineNumber() const {
    return _sci ? _sci->lineFromPosition(_sci->currentPos()) : 0;
}

intptr_t ScintillaEditView::lastZeroBasedLineNumber() const {
    return _sci ? _sci->lineCount() - 1 : 0;
}

intptr_t ScintillaEditView::getCurrentXOffset() const {
    return _sci ? _sci->xOffset() : 0;
}

void ScintillaEditView::setCurrentXOffset(long xOffset) {
    if (_sci) _sci->setXOffset(xOffset);
}

void ScintillaEditView::scroll(intptr_t column, intptr_t line) {
    if (_sci) _sci->lineScroll(column, line);
}

intptr_t ScintillaEditView::getCurrentPointX() const {
    return _sci ? _sci->pointXFromPosition(_sci->currentPos()) : 0;
}

intptr_t ScintillaEditView::getCurrentPointY() const {
    return _sci ? _sci->pointYFromPosition(_sci->currentPos()) : 0;
}

intptr_t ScintillaEditView::getTextHeight() const {
    return _sci ? _sci->textHeight(0) : 0;
}

int ScintillaEditView::getTextZoneWidth() const {
    return _sci ? _sci->width() : 0;
}

void ScintillaEditView::gotoLine(intptr_t line) {
    if (_sci && line < _sci->lineCount()) _sci->gotoLine(line);
}

intptr_t ScintillaEditView::getCurrentColumnNumber() const {
    return _sci ? _sci->column(_sci->currentPos()) : 0;
}

std::pair<size_t, size_t> ScintillaEditView::getSelectedCharsAndLinesCount(long long) const {
    return _sci ? std::make_pair(getSelectedTextCount(), 0ULL) : std::make_pair(0ULL, 0ULL);
}

size_t ScintillaEditView::getUnicodeSelectedLength() const {
    return static_cast<size_t>(getSelectedTextCount());
}

intptr_t ScintillaEditView::getLineLength(size_t line) const {
    return _sci ? (_sci->lineEndPosition(static_cast<sptr_t>(line)) - _sci->positionFromLine(static_cast<sptr_t>(line))) : 0;
}

intptr_t ScintillaEditView::getLineIndent(size_t line) const {
    return _sci ? _sci->lineIndentation(static_cast<sptr_t>(line)) : 0;
}

void ScintillaEditView::setLineIndent(size_t line, size_t indent) const {
    if (_sci) _sci->setLineIndentation(static_cast<sptr_t>(line), static_cast<sptr_t>(indent));
}

void ScintillaEditView::updateLineNumbersMargin(bool forcedToHide) {
    if (forcedToHide)
        execute(SCI_SETMARGINWIDTHN, _SC_MARGE_LINENUMBER, 0);
    else
        updateLineNumberWidth();
}

void ScintillaEditView::updateLineNumberWidth() {
    if (!_sci) return;
    int lineCount = _sci->lineCount();
    int width = 4;
    if (lineCount > 0)
        width = QString::number(lineCount).size() * 8 + 10;
    execute(SCI_SETMARGINWIDTHN, _SC_MARGE_LINENUMBER, width);
}

void ScintillaEditView::performGlobalStyles() {}

std::pair<size_t, size_t> ScintillaEditView::getSelectionLinesRange(intptr_t) const {
    if (!_sci) return {0, 0};
    auto range = getSelection();
    return {static_cast<size_t>(_sci->lineFromPosition(range.cpMin)), static_cast<size_t>(_sci->lineFromPosition(range.cpMax))};
}

void ScintillaEditView::currentLinesUp() const {}
void ScintillaEditView::currentLinesDown() const {}

intptr_t ScintillaEditView::caseConvertRange(intptr_t, intptr_t, TextCase) {
    return 0;
}

void ScintillaEditView::changeCase(wchar_t* const strWToConvert, const int& nbChars, const TextCase& caseToConvert) {
    if (!strWToConvert || nbChars <= 0) return;
    QString str = QString::fromUtf16(reinterpret_cast<ushort*>(strWToConvert), nbChars);
    str = (caseToConvert == UPPERCASE) ? str.toUpper() : (caseToConvert == LOWERCASE) ? str.toLower() : str;
    qstrncpy(reinterpret_cast<QChar*>(strWToConvert), str.utf16(), nbChars);
}

void ScintillaEditView::convertSelectedTextTo(const TextCase& caseToConvert) {
    if (!_sci) return;

    QByteArray selected = _sci->getSelText();
    if (selected.isEmpty()) return;

    QString str = QString::fromLatin1(selected);
    str = (caseToConvert == UPPERCASE) ? str.toUpper() : (caseToConvert == LOWERCASE) ? str.toLower() : str;
    _sci->replaceSel(str.toLatin1().constData());
}

void ScintillaEditView::setMultiSelections(const ColumnModeInfos&) {
    if (!_sci) return;
    execute(SCI_SETMULTIPLESELECTION, 1);
    execute(SCI_SETADDITIONALSELECTIONTYPING, 1);
}

void ScintillaEditView::convertSelectedTextToLowerCase() {
    convertSelectedTextTo(LOWERCASE);
}

void ScintillaEditView::convertSelectedTextToUpperCase() {
    convertSelectedTextTo(UPPERCASE);
}

void ScintillaEditView::convertSelectedTextToNewerCase(const TextCase& caseToConvert) {
    convertSelectedTextTo(caseToConvert);
}

void ScintillaEditView::getCurrentFoldStates(std::vector<size_t>&) {}
void ScintillaEditView::syncFoldStateWith(const std::vector<size_t>&) {}

bool ScintillaEditView::isFoldIndentationBased() const {
    return false;
}

void ScintillaEditView::foldIndentationBasedLevel(int, bool) {}
void ScintillaEditView::foldLevel(int, bool) {}

void ScintillaEditView::foldAll(bool mode) {
    if (_sci) _sci->foldAll(mode ? SC_FOLDACTION_CONTRACT : SC_FOLDACTION_EXPAND);
}

void ScintillaEditView::fold(size_t line, bool mode, bool) {
    if (_sci) _sci->foldLine(static_cast<sptr_t>(line), mode ? SC_FOLDACTION_CONTRACT : SC_FOLDACTION_EXPAND);
}

bool ScintillaEditView::isFolded(size_t line) const {
    return _sci && _sci->foldExpanded(static_cast<sptr_t>(line));
}

void ScintillaEditView::expand(size_t&, bool, bool, intptr_t, intptr_t) {}

bool ScintillaEditView::isCurrentLineFolded() const {
    return false;
}

void ScintillaEditView::foldCurrentPos(bool mode) {
    if (_sci) fold(_sci->lineFromPosition(_sci->currentPos()), mode);
}

ColumnModeInfos ScintillaEditView::getColumnModeSelectInfo() {
    ColumnModeInfos result;
    if (!_sci) return result;

    int selections = _sci->selections();
    for (int i = 0; i < selections; ++i) {
        sptr_t anchor = _sci->selectionNAnchor(i);
        sptr_t caret = _sci->selectionNCaret(i);
        result.emplace_back(anchor, caret, i);
    }
    return result;
}

void ScintillaEditView::columnReplace(ColumnModeInfos&, const char*) const {}
void ScintillaEditView::columnReplace(ColumnModeInfos&, size_t, size_t, size_t, NumBase, ColumnEditorParam::LeadingChoice) const {}

void ScintillaEditView::clearIndicator(int indicatorNumber) {
    if (!_sci) return;
    _sci->setIndicatorCurrent(static_cast<sptr_t>(indicatorNumber));
    _sci->indicatorClearRange(0, static_cast<sptr_t>(getCurrentDocLen()));
}

bool ScintillaEditView::getIndicatorRange(size_t, size_t*, size_t*, size_t*) {
    return false;
}

void ScintillaEditView::bufferUpdated(Buffer*, int) {}
void ScintillaEditView::styleChange() {}

void ScintillaEditView::hideLines() {}
bool ScintillaEditView::hidelineMarkerClicked(intptr_t) {
    return false;
}

void ScintillaEditView::notifyHidelineMarkers(Buffer*, bool, size_t, bool) {}

void ScintillaEditView::hideMarkedLines(size_t, bool) const {}
void ScintillaEditView::showHiddenLines(size_t, bool, bool) const {}
void ScintillaEditView::restoreHiddenLines() {}

bool ScintillaEditView::isPythonStyleIndentation(LangType typeDoc) {
    return (typeDoc == L_PYTHON || typeDoc == L_COFFEESCRIPT || typeDoc == L_HASKELL || typeDoc == L_C || typeDoc == L_CPP ||
            typeDoc == L_OBJC || typeDoc == L_CS || typeDoc == L_JAVA || typeDoc == L_PHP || typeDoc == L_JS_EMBEDDED ||
            typeDoc == L_JAVASCRIPT || typeDoc == L_MAKEFILE || typeDoc == L_ASN1 || typeDoc == L_GDSCRIPT);
}

void ScintillaEditView::defineDocType(LangType) {}
void ScintillaEditView::addCustomWordChars() const {}
void ScintillaEditView::restoreDefaultWordChars() const {}
void ScintillaEditView::setWordChars() const {}
void ScintillaEditView::setCRLF(long) const {}
void ScintillaEditView::setNpcAndCcUniEOL(long) const {}
void ScintillaEditView::setTabSettings(Lang*) {}
void ScintillaEditView::scrollPosToCenter(size_t) {}

std::wstring ScintillaEditView::getEOLString() const {
    return _sci ? L"\n" : L"";
}

void ScintillaEditView::setBorderEdge(bool) {}
void ScintillaEditView::sortLines(size_t, size_t, ISorter*) {}
void ScintillaEditView::changeTextDirection(bool) {}
bool ScintillaEditView::isTextDirectionRTL() const {
    return false;
}
void ScintillaEditView::markedTextToClipboard(int, bool) {}
void ScintillaEditView::removeAnyDuplicateLines() {}
bool ScintillaEditView::expandWordSelection() {
    return false;
}
bool ScintillaEditView::pasteToMultiSelection() const {
    return false;
}

void ScintillaEditView::defineMarker(int marker, int markerType, COLORREF fore, COLORREF back, COLORREF foreActive) {
    if (!_sci) return;
    _sci->markerDefine(marker, markerType);
    _sci->markerSetFore(marker, fore);
    _sci->markerSetBack(marker, back);
    _sci->markerSetBackSelected(marker, foreActive);
}

int ScintillaEditView::codepage2CharSet() const {
    switch (_codepage) {
        case CP_CHINESE_TRADITIONAL: return SC_CHARSET_CHINESEBIG5;
        case CP_CHINESE_SIMPLIFIED: return SC_CHARSET_GB2312;
        case CP_KOREAN: return SC_CHARSET_HANGUL;
        case CP_JAPANESE: return SC_CHARSET_SHIFTJIS;
        case CP_GREEK: return SC_CHARSET_GREEK;
        default: return 0;
    }
}

std::pair<size_t, size_t> ScintillaEditView::getWordRange() {
    return {0, 0};
}

void ScintillaEditView::getFoldColor(COLORREF& fgColor, COLORREF& bgColor, COLORREF& activeFgColor) {
    fgColor = 0x00FFFFFF;
    bgColor = 0x00808080;
    activeFgColor = 0x00FF0000;
}

BufferID ScintillaEditView::attachDefaultDoc() {
    return nullptr;
}

void ScintillaEditView::restyleBuffer() {}

const char* ScintillaEditView::concatToBuildKeywordList(std::string&, LangType, int) {
    return nullptr;
}

void ScintillaEditView::setKeywords(LangType, const char*, int) const {}

void ScintillaEditView::populateSubStyleKeywords(LangType, int, int, int, const char**) const {}

void ScintillaEditView::setLexer(LangType, int, int, int) const {}

bool ScintillaEditView::setLexerFromLangID(int) const {
    return false;
}

void ScintillaEditView::makeStyle(LangType, const char**) const {}

void ScintillaEditView::setStyle(Style) const {}

void ScintillaEditView::setSpecialStyle(const Style&) const {}

void ScintillaEditView::setSpecialIndicator(const Style&) const {}

void ScintillaEditView::setXmlLexer(LangType) const {}
void ScintillaEditView::setCppLexer(LangType) const {}
void ScintillaEditView::setHTMLLexer() const {}
void ScintillaEditView::setJsLexer() const {}
void ScintillaEditView::setTclLexer() const {}
void ScintillaEditView::setObjCLexer(LangType) const {}
void ScintillaEditView::setUserLexer(const wchar_t*) const {}
void ScintillaEditView::setExternalLexer(LangType) const {}
void ScintillaEditView::setEmbeddedJSLexer() const {}
void ScintillaEditView::setEmbeddedPhpLexer() const {}
void ScintillaEditView::setEmbeddedAspLexer() const {}
void ScintillaEditView::setJsonLexer(bool) const {}
void ScintillaEditView::setTypeScriptLexer() const {}

void ScintillaEditView::setCssLexer() {
    setLexer(L_CSS, LIST_0 | LIST_1 | LIST_4 | LIST_6);
}

void ScintillaEditView::setLuaLexer() {
    setLexer(L_LUA, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5 | LIST_6 | LIST_7, SCE_LUA_IDENTIFIER, 4);
}

void ScintillaEditView::setMakefileLexer() {
    setLexer(L_MAKEFILE, LIST_NONE);
}

void ScintillaEditView::setPropsLexer(bool isPropsButNotIni) {
    setLexer(isPropsButNotIni ? L_PROPS : L_INI, LIST_NONE);
}

void ScintillaEditView::setSqlLexer() {
    setLexer(L_SQL, LIST_0 | LIST_1 | LIST_4);
}

void ScintillaEditView::setMSSqlLexer() {
    setLexer(L_MSSQL, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5);
}

void ScintillaEditView::setBashLexer() const {
    setLexerFromLangID(L_BASH);
}

void ScintillaEditView::setVBLexer() {
    setLexer(L_VB, LIST_0);
}

void ScintillaEditView::setPascalLexer() {
    setLexer(L_PASCAL, LIST_0);
}

void ScintillaEditView::setPerlLexer() {
    setLexer(L_PERL, LIST_0);
}

void ScintillaEditView::setPythonLexer() {
    setLexer(L_PYTHON, LIST_0 | LIST_1, SCE_P_IDENTIFIER);
}

void ScintillaEditView::setGDScriptLexer() {
    setLexer(L_GDSCRIPT, LIST_0 | LIST_1, SCE_GD_IDENTIFIER);
}

void ScintillaEditView::setBatchLexer() {
    setLexer(L_BATCH, LIST_0);
}

void ScintillaEditView::setTeXLexer() {
    setLexer(L_TEX, LIST_NONE);
}

void ScintillaEditView::setNsisLexer() {
    setLexer(L_NSIS, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setFortranLexer() {
    setLexer(L_FORTRAN, LIST_0 | LIST_1 | LIST_2);
}

void ScintillaEditView::setFortran77Lexer() {
    setLexer(L_FORTRAN_77, LIST_0 | LIST_1 | LIST_2);
}

void ScintillaEditView::setLispLexer() {
    setLexer(L_LISP, LIST_0 | LIST_1);
}

void ScintillaEditView::setSchemeLexer() {
    setLexer(L_SCHEME, LIST_0 | LIST_1);
}

void ScintillaEditView::setAsmLexer() {
    setLexer(L_ASM, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5 | LIST_6 | LIST_7);
}

void ScintillaEditView::setDiffLexer() {
    setLexer(L_DIFF, LIST_NONE);
}

void ScintillaEditView::setPostscriptLexer() {
    setLexer(L_PS, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setRubyLexer() {
    setLexer(L_RUBY, LIST_0);
}

void ScintillaEditView::setSmalltalkLexer() {
    setLexer(L_SMALLTALK, LIST_0);
}

void ScintillaEditView::setVhdlLexer() {
    setLexer(L_VHDL, LIST_0);
}

void ScintillaEditView::setKixLexer() {
    setLexer(L_KIX, LIST_0);
}

void ScintillaEditView::setAutoItLexer() {
    setLexer(L_AU3, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setCamlLexer() {
    setLexer(L_CAML, LIST_NONE);
}

void ScintillaEditView::setAdaLexer() {
    setLexer(L_ADA, LIST_0);
}

void ScintillaEditView::setVerilogLexer() {
    setLexer(L_VERILOG, LIST_0 | LIST_1 | LIST_2);
}

void ScintillaEditView::setMatlabLexer() {
    setLexer(L_MATLAB, LIST_0 | LIST_1);
}

void ScintillaEditView::setHaskellLexer() {
    setLexer(L_HASKELL, LIST_0);
}

void ScintillaEditView::setInnoLexer() {
    setLexer(L_INNO, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5);
}

void ScintillaEditView::setCmakeLexer() {
    setLexer(L_CMAKE, LIST_NONE);
}

void ScintillaEditView::setYamlLexer() {
    setLexer(L_YAML, LIST_NONE);
}

void ScintillaEditView::setCobolLexer() {
    setLexer(L_COBOL, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setGui4CliLexer() {
    setLexer(L_GUI4CLI, LIST_0 | LIST_1);
}

void ScintillaEditView::setDLexer() {
    setLexer(L_D, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setPowerShellLexer() {
    setLexer(L_POWER_SHELL, LIST_0 | LIST_1 | LIST_2);
}

void ScintillaEditView::setRLexer() {
    setLexer(L_R, LIST_0 | LIST_1);
}

void ScintillaEditView::setCoffeeScriptLexer() {
    setLexer(L_COFFEESCRIPT, LIST_0 | LIST_1);
}

void ScintillaEditView::setBaanCLexer() {
    setLexer(L_BAAN, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setSrecLexer() {
    setLexer(L_SREC, LIST_NONE);
}

void ScintillaEditView::setIHexLexer() {
    setLexer(L_IHEX, LIST_NONE);
}

void ScintillaEditView::setTEHexLexer() {
    setLexer(L_TEHEX, LIST_NONE);
}

void ScintillaEditView::setAsn1Lexer() {
    setLexer(L_ASN1, LIST_0);
}

void ScintillaEditView::setAVSLexer() {
    setLexer(L_AVS, LIST_0);
}

void ScintillaEditView::setBlitzBasicLexer() {
    setLexer(L_BLITZBASIC, LIST_0 | LIST_1);
}

void ScintillaEditView::setPureBasicLexer() {
    setLexer(L_PUREBASIC, LIST_0 | LIST_1);
}

void ScintillaEditView::setFreeBasicLexer() {
    setLexer(L_FREEBASIC, LIST_0 | LIST_1);
}

void ScintillaEditView::setCsoundLexer() {
    setLexer(L_CSOUND, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5);
}

void ScintillaEditView::setErlangLexer() {
    setLexer(L_ERLANG, LIST_0 | LIST_1);
}

void ScintillaEditView::setESCRIPTLexer() {
    setLexer(L_ESCRIPT, LIST_0);
}

void ScintillaEditView::setForthLexer() {
    setLexer(L_FORTH, LIST_0);
}

void ScintillaEditView::setLatexLexer() {
    setLexer(L_LATEX, LIST_NONE);
}

void ScintillaEditView::setMMIXALLexer() {
    setLexer(L_MMIXAL, LIST_0);
}

void ScintillaEditView::setNimrodLexer() {
    setLexer(L_NIM, LIST_0);
}

void ScintillaEditView::setNncrontabLexer() {
    setLexer(L_NNCRONTAB, LIST_0 | LIST_1 | LIST_2 | LIST_3);
}

void ScintillaEditView::setOScriptLexer() {
    setLexer(L_OSCRIPT, LIST_0);
}

void ScintillaEditView::setREBOLLexer() {
    setLexer(L_REBOL, LIST_0 | LIST_1);
}

void ScintillaEditView::setRegistryLexer() {
    setLexer(L_REGISTRY, LIST_NONE);
}

void ScintillaEditView::setRustLexer() {
    setLexer(L_RUST, LIST_0 | LIST_1);
}

void ScintillaEditView::setSpiceLexer() {
    setLexer(L_SPICE, LIST_0);
}

void ScintillaEditView::setTxt2tagsLexer() {
    setLexer(L_TXT2TAGS, LIST_0);
}

void ScintillaEditView::setVisualPrologLexer() {
    setLexer(L_VISUALPROLOG, LIST_0 | LIST_1 | LIST_2);
}

void ScintillaEditView::setHollywoodLexer() {
    setLexer(L_HOLLYWOOD, LIST_0 | LIST_1);
}

void ScintillaEditView::setRakuLexer() {
    setLexer(L_RAKU, LIST_0 | LIST_1);
}

void ScintillaEditView::setTomlLexer() {
    setLexer(L_TOML, LIST_NONE);
}

void ScintillaEditView::setSasLexer() {
    setLexer(L_SAS, LIST_0 | LIST_1 | LIST_2 | LIST_3 | LIST_4 | LIST_5);
}

void ScintillaEditView::setErrorListLexer() {
    setLexer(L_ERRORLIST, LIST_NONE);
}

void ScintillaEditView::setSearchResultLexer() {
    setLexer(L_SEARCHRESULT, LIST_NONE);
}
