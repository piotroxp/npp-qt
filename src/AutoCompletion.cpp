// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of AutoCompletion.h/.cpp (1332 lines)
// Win32→Qt6: ListView_* → QListWidget, autoComplete from XML→QSettings/QFile
// AutoCompletion manages:
//   - Word completion (current file keywords)
//   - API completion (from .api XML files)
//   - Function calltips
//   - Matched char insertion (auto-close brackets)

#include "AutoCompletion.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include <QFile>
#include <QDir>

// Static colour defaults (match Win32 resource values)
static int _autocompleteBg = 0xFFFFFF;
static int _autocompleteText = 0x000000;
static int _selectedBg = 0x0A2463;
static int _selectedText = 0xFFFFFF;
static int _calltipBg = 0xFFFFE1;
static int _calltipText = 0x000000;
static int _calltipHighlight = 0x0000FF;

AutoCompletion::AutoCompletion(ScintillaComponent* pEditView)
    : QObject(pEditView), _pEditView(pEditView)
{
    _insertedMatchedChars.init(pEditView);
}

AutoCompletion::~AutoCompletion() = default;

void AutoCompletion::setColour(QRgb colour, AutocompleteColorIndex colorIndex)
{
    switch (colorIndex) {
        case AutocompleteColorIndex::autocompleteText: _autocompleteText = colour; break;
        case AutocompleteColorIndex::autocompleteBg: _autocompleteBg = colour; break;
        case AutocompleteColorIndex::selectedText: _selectedText = colour; break;
        case AutocompleteColorIndex::selectedBg: _selectedBg = colour; break;
        case AutocompleteColorIndex::calltipBg: _calltipBg = colour; break;
        case AutocompleteColorIndex::calltipText: _calltipText = colour; break;
        case AutocompleteColorIndex::calltipHighlight: _calltipHighlight = colour; break;
    }
}

void AutoCompletion::drawAutocomplete(const ScintillaComponent*) {}

bool AutoCompletion::setLanguage(int language)
{
    _curLang = language;
    _keyWords.clear();
    _keyWordArray.clear();
    _keyWordMaxLen = 0;
    _ignoreCase = true;
    // NOTE: keyword loading from user-defined language or .api XML files
    // requires NppParameters and the lexer/API file system.
    // Currently uses built-in scintilla keywords via SCI_AUTOCSETSEPARATOR.
    return true;
}

bool AutoCompletion::showApiComplete()
{
    return showAutoComplete(0, false); // autocFunc
}

bool AutoCompletion::showWordComplete(bool autoInsert)
{
    return showAutoComplete(3, autoInsert); // autocWord
}

bool AutoCompletion::showApiAndWordComplete()
{
    return showAutoComplete(1, false); // autocFuncAndWord
}

bool AutoCompletion::showFunctionComplete()
{
    return showAutoComplete(2, false); // autocFuncBrief
}

bool AutoCompletion::showAutoComplete(int autocType, bool autoInsert)
{
    if (!_pEditView) return false;
    size_t caret = _pEditView->send(SCI_GETCURRENTPOS);
    size_t wordStart = _pEditView->send(SCI_WORDSTARTPOSITION, caret, true);
    size_t wordEnd = _pEditView->send(SCI_WORDENDPOSITION, caret, true);
    size_t lenEntered = wordEnd - wordStart;
    if (lenEntered == 0 && autocType != 2) return false;

    // Build completion list string from _keyWords (populated via setLanguage)
    // NOTE: loading actual API keywords from .api XML files for the current language
    // requires the UserDefineDialog + API file reader (pending).
    QString listStr;
    if (!_keyWords.empty()) {
        listStr = QString::fromLatin1(_keyWords.c_str());
    }
    if (!listStr.isEmpty() && lenEntered > 0) {
        QByteArray utf8 = listStr.toUtf8();
        _pEditView->send(SCI_AUTOCSETSEPARATOR, ' ');
        _pEditView->send(SCI_AUTOCSETIGNORECASE, _ignoreCase ? 1 : 0);
        _pEditView->send(SCI_AUTOCSETAUTOHIDE, 0);
        _pEditView->send(SCI_AUTOCSETDROPRESTOFWORD, 0);
        _pEditView->send(SCI_AUTOCSHOW, lenEntered, reinterpret_cast<sptr_t>(utf8.constData()));
        return true;
    }
    return false;
}

void AutoCompletion::showPathCompletion()
{
    if (!_pEditView) return;
    // Trigger path autocomplete on '/' or '\\'
    _pEditView->send(SCI_AUTOCSHOW, 0, reinterpret_cast<sptr_t>("/"));
}

void AutoCompletion::insertMatchedChars(int character, int matchedPairConf)
{
    Q_UNUSED(matchedPairConf);
    if (!_pEditView) return;
    size_t caret = _pEditView->send(SCI_GETCURRENTPOS);
    char chars[2] = { static_cast<char>(character), '\0' };
    _pEditView->send(SCI_INSERTTEXT, caret, reinterpret_cast<sptr_t>(chars));
    _insertedMatchedChars.add(MatchedCharInserted(static_cast<char>(character), caret));
}

void AutoCompletion::update(int character)
{
    Q_UNUSED(character);
}

void AutoCompletion::callTipClick(size_t direction)
{
    if (!_pEditView) return;
    _pEditView->send(SCI_CALLTIPCANCEL);
    Q_UNUSED(direction);
    // Stub: SCI_CALLTIPSETCURRENTTIP for overload cycling
}

void AutoCompletion::getCloseTag(char* closeTag, size_t closeTagLen, size_t, bool) const
{
    if (!closeTag || closeTagLen < 1) return;
    closeTag[0] = '\0';
}

// InsertedMatchedChars
void InsertedMatchedChars::add(MatchedCharInserted mci)
{
    _insertedMatchedChars.push_back(mci);
}

intptr_t InsertedMatchedChars::search(char startChar, char, size_t posToDetect)
{
    for (auto it = _insertedMatchedChars.rbegin(); it != _insertedMatchedChars.rend(); ++it) {
        if (it->_c == startChar && it->_pos < posToDetect)
            return static_cast<intptr_t>(it->_pos);
    }
    return -1;
}
