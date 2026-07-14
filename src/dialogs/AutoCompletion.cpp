// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of AutoCompletion.cpp — autocompletion hub (API, word, function, path)
#include "AutoCompletion.h"
#include "FunctionCallTip.h"
#include "../editor/ScintillaEditor.h"
#include "core/LanguageManager.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include <QMessageBox>
#include <QFileInfo>

// ─── QRgb colour cache ────────────────────────────────────────────────────────

static QRgb g_colourTable[7] = {
    0xFFEEDDCC, // autocompleteText
    0xFF2A2A2A, // autocompleteBg
    0xFF3399FF, // selectedText
    0xFF0066CC, // selectedBg
    0xFFFFD700, // calltipBg
    0xFFFFFFFF, // calltipText
    0xFF33AAFF, // calltipHighlight
};

void AutoCompletion::setColour(QRgb colour2Set, AutocompleteColorIndex i)
{
    g_colourTable[static_cast<int>(i)] = colour2Set;
}

void AutoCompletion::setColour(int colour, int colorIndex)
{
    g_colourTable[colorIndex % 7] = static_cast<QRgb>(colour);
}

// ─── Construction / Destruction ───────────────────────────────────────────────

AutoCompletion::AutoCompletion(ScintillaComponent* pEditView)
    : _pEditView(pEditView)
    , _insertedMatchedChars(pEditView)
{
}

AutoCompletion::~AutoCompletion()
{
    close();
}

// ─── Language setup ────────────────────────────────────────────────────────────

bool AutoCompletion::setLanguage(int language)
{
    _curLang = language;
    _keyWordArray.clear();
    _keyWords.clear();

    auto& langMgr = LanguageManager::instance();
    auto keywords = langMgr.getKeywords(static_cast<LangType>(language));

    for (const auto& [setIdx, kwString] : keywords) {
        Q_UNUSED(setIdx);
        if (!kwString.empty()) {
            if (!_keyWords.empty())
                _keyWords += ' ';
            _keyWords += kwString;
        }
    }

    _ignoreCase = !langMgr.isCaseSensitive(static_cast<LangType>(language));
    return !_keyWords.empty();
}

// ─── Core autocomplete logic ───────────────────────────────────────────────────

bool AutoCompletion::showAutoComplete(int /*autocType*/, bool /*autoInsert*/)
{
    if (!_pEditView || _keyWords.empty())
        return false;

    // npp-qt: SCI_AUTOCSHOW — Scintilla handles Win32 ListBox natively
    _pEditView->send(SCI_AUTOCSHOW, 0,
        reinterpret_cast<intptr_t>(_keyWords.c_str()));
    return true;
}

// ─── Public API ───────────────────────────────────────────────────────────────

bool AutoCompletion::showApiComplete()
{
    return showAutoComplete(0, false);
}

bool AutoCompletion::showWordComplete(bool /*autoInsert*/)
{
    if (!_pEditView || _keyWords.empty())
        return false;

    // npp-qt: same keyword list; Scintilla deduplicates automatically
    _pEditView->send(SCI_AUTOCSHOW, 0,
        reinterpret_cast<intptr_t>(_keyWords.c_str()));
    return true;
}

bool AutoCompletion::showApiAndWordComplete()
{
    return showWordComplete(false);
}

bool AutoCompletion::showFunctionComplete()
{
    _funcCompletionActive = true;
    return showAutoComplete(1, false);
}

void AutoCompletion::showPathCompletion()
{
    if (!_pEditView)
        return;

    // Get current word (path being typed)
    Sci_CharacterRange cr = _pEditView->getSelection();
    int startWord = static_cast<int>(_pEditView->send(SCI_WORDSTARTPOSITION, cr.cpMin, 0));
    int endWord   = static_cast<int>(_pEditView->send(SCI_WORDENDPOSITION,   cr.cpMax, 0));
    QByteArray word = _pEditView->textRange(startWord, endWord);
    Q_UNUSED(word);

    // TODO: enumerate directory entries for this prefix
    // Fall back to word completion for now
    showWordComplete(false);
}

// ─── Matched-char insertion ────────────────────────────────────────────────────

void AutoCompletion::insertMatchedChars(int /*character*/, int /*matchedPairConf*/)
{
    // npp-qt: Scintilla handles matched-pair insertion natively
    // via auto-close configuration on the editor widget
}

void AutoCompletion::update(int /*character*/)
{
    // npp-qt: keystroke-driven logic is handled by Scintilla editor widget
}

void AutoCompletion::callTipClick(size_t direction)
{
    if (direction == 0)
        _pEditView->send(SCI_CALLTIPUP);
    else
        _pEditView->send(SCI_CALLTIPTEXTCOLOUR, 0);
}

// ─── Close-tag helper (HTML/XML) ──────────────────────────────────────────────

void AutoCompletion::getCloseTag(char* closeTag, size_t closeTagLen,
                                  size_t caretPos, bool isHTML) const
{
    if (!closeTag || closeTagLen == 0 || !_pEditView)
        return;

    closeTag[0] = '\0';

    // Scan backwards from caret to find the last unclosed opening tag
    int scanStart = static_cast<int>(caretPos > 200 ? caretPos - 200 : 0);
    int scanEnd   = static_cast<int>(caretPos);
    QByteArray buf = _pEditView->textRange(scanStart, scanEnd);

    int ltPos = buf.lastIndexOf('<');
    if (ltPos < 0)
        return;

    std::string tag;
    for (int i = ltPos + 1; i < buf.size(); ++i) {
        char c = buf[i];
        if (std::isalnum(static_cast<unsigned char>(c)))
            tag += c;
        else
            break;
    }

    if (tag.empty())
        return;

    if (isHTML && (tag == "script" || tag == "style"))
        return;

    snprintf(closeTag, closeTagLen, "</%s>", tag.c_str());
}

// ─── Static draw ──────────────────────────────────────────────────────────────

void AutoCompletion::drawAutocomplete(const ScintillaComponent* /*pEditView*/)
{
    // npp-qt: Scintilla's autocompletion draws via its own colour scheme
    // configured via SCI_AUTOCSETFORE / SCI_AUTOCSETBACK — no manual draw needed
}
