// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of AutoCompletion.cpp — autocompletion hub (API, word, function, path)
#include "AutoCompletion.h"
#include "FunctionCallTip.h"
#include "../editor/ScintillaEditor.h"
#include "../core/LanguageManager.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include <QFileInfo>
#include <QDir>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QDebug>

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

// ─── Construction / Destruction ───────────────────────────────────────────────

AutoCompletion::AutoCompletion(ScintillaComponent* pEditView)
    : _pEditView(pEditView)
    , _insertedMatchedChars(pEditView)
{
    // Refresh document words after a short delay on startup
    QTimer::singleShot(100, this, &AutoCompletion::refreshDocumentWords);
}

AutoCompletion::~AutoCompletion()
{
    close();
}

// ─── Language setup ────────────────────────────────────────────────────────────

bool AutoCompletion::setLanguage(int language)
{
    _curLang = language;
    _keyWordList.clear();
    _keyWords.clear();

    auto& langMgr = LanguageManager::instance();
    auto keywords = langMgr.getKeywords(static_cast<LangType>(language));

    for (const auto& [setIdx, kwString] : keywords) {
        Q_UNUSED(setIdx);
        if (!kwString.empty()) {
            if (!_keyWords.empty())
                _keyWords += ' ';
            _keyWords += kwString;
            _keyWordList << QString::fromStdString(kwString).split(' ', Qt::SkipEmptyParts);
        }
    }

    _caseSensitive = langMgr.isCaseSensitive(static_cast<LangType>(language));
    _envVarCacheValid = false; // Reset env var cache on language change

    return !_keyWords.empty();
}

// ─── Sorting ───────────────────────────────────────────────────────────────────

void AutoCompletion::setSortMode(SortMode mode)
{
    _sortMode = mode;
    // If completion is active, re-sort the visible list.
    if (_completionActive)
        refreshDocumentWords(); // Rebuilds word list with current sort
}

void AutoCompletion::applySortMode(QVector<CompletionItem>& items) const
{
    switch (_sortMode) {
        case SortMode::Alphabetical:
            std::sort(items.begin(), items.end(),
                      [](const CompletionItem& a, const CompletionItem& b) {
                          if (a.sortPriority != b.sortPriority)
                              return a.sortPriority < b.sortPriority;
                          if (a.isInternal != b.isInternal) {
                              // Keywords/API before document words
                              return !a.isInternal;
                          }
                          return a.text.compare(b.text, Qt::CaseInsensitive) < 0;
                      });
            break;
        case SortMode::ByFrequency:
            std::sort(items.begin(), items.end(),
                      [this](const CompletionItem& a, const CompletionItem& b) {
                          auto itFa = _wordFrequency.find(a.text.toLower());
        int fa = (itFa != _wordFrequency.end()) ? itFa->second : 0;
                          auto itFb = _wordFrequency.find(b.text.toLower());
        int fb = (itFb != _wordFrequency.end()) ? itFb->second : 0;
                          if (fa != fb) return fa > fb;
                          return a.text.compare(b.text, Qt::CaseInsensitive) < 0;
                      });
            break;
        case SortMode::ByLength:
            std::sort(items.begin(), items.end(),
                      [](const CompletionItem& a, const CompletionItem& b) {
                          if (a.text.length() != b.text.length())
                              return a.text.length() < b.text.length();
                          return a.text.compare(b.text, Qt::CaseInsensitive) < 0;
                      });
            break;
    }
}

// ─── Common prefix ─────────────────────────────────────────────────────────────

QString AutoCompletion::commonPrefix(const QStringList& items) const
{
    if (items.isEmpty()) return QString();
    if (items.size() == 1) return items.first();

    const QString& first = items.first();
    for (int len = first.length(); len > 0; --len) {
        QString prefix = first.left(len);
        bool allMatch = true;
        for (const QString& item : items) {
            if (!item.startsWith(prefix, Qt::CaseInsensitive)) {
                allMatch = false;
                break;
            }
        }
        if (allMatch) return prefix;
    }
    return QString();
}

// ─── Environment variables ─────────────────────────────────────────────────────

void AutoCompletion::setEnvVarCompletion(bool on)
{
    _envVarCompletion = on;
    if (on) _envVarCacheValid = false;
}

QStringList AutoCompletion::buildEnvVarList() const
{
    if (_envVarCacheValid) return _cachedEnvVars;

    _cachedEnvVars.clear();
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QStringList vars = env.keys();
    _cachedEnvVars.reserve(vars.size());

    for (const QString& var : vars) {
        _cachedEnvVars << QString("$%1").arg(var);
        _cachedEnvVars << QString("${%1}").arg(var);
    }

    _envVarCacheValid = true;
    return _cachedEnvVars;
}

// ─── Path completion ────────────────────────────────────────────────────────────

QStringList AutoCompletion::buildPathList(const QString& prefix) const
{
    QStringList results;

    // Determine the base directory and the partial filename.
    QString dirPath;
    QString filePrefix;

    int lastSlash = prefix.lastIndexOf('/');
    int lastBackslash = prefix.lastIndexOf('\\');
    int sep = qMax(lastSlash, lastBackslash);

    if (sep >= 0) {
        dirPath = prefix.left(sep + 1);
        filePrefix = prefix.mid(sep + 1);
    } else {
        dirPath = ".";
        filePrefix = prefix;
    }

    QDir dir(dirPath);
    if (!dir.exists()) return results;

    // For include/import statements, also search in standard include paths.
    // (Full implementation would consult LanguageManager for include directories.)

    QFileInfoList entries;
    if (dirPath == ".") {
        entries = QDir::current().entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    } else {
        entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    }

    for (const QFileInfo& info : entries) {
        QString name = info.fileName();
        if (name.startsWith(filePrefix, Qt::CaseInsensitive)) {
            if (info.isDir()) {
                results << prefix + name + "/";
            } else {
                results << prefix + name;
            }
        }
    }

    return results;
}

// ─── Document word scanning ────────────────────────────────────────────────────

QStringList AutoCompletion::buildWordList() const
{
    return _documentWords;
}

void AutoCompletion::updateDocumentWords()
{
    if (!_pEditView) return;

    // Get the entire document text.
    const Sci_CharacterRange cr = _pEditView->getSelection();
    const int docLen = static_cast<int>(_pEditView->send(SCI_GETLENGTH));
    Q_UNUSED(cr);

    if (docLen <= 0) {
        _documentWords.clear();
        return;
    }

    // We only need to scan around the current word — for simplicity,
    // scan the whole document and collect identifiers.
    // For performance with very large files, this should be incremental.
    const int maxScan = qMin(docLen, 2 * 1024 * 1024); // cap at 2 MB
    QByteArray buffer(maxScan + 1, '\0');
    _pEditView->send(SCI_GETTEXT, maxScan, reinterpret_cast<sptr_t>(buffer.data()));
    buffer.data()[maxScan] = '\0';

    QString text = QString::fromUtf8(buffer.constData());
    QRegularExpression wordRe(R"(\b([\w_][\w\d_]*)\b)");
    QRegularExpressionMatchIterator it = wordRe.globalMatch(text);

    // Deduplicate using a QSet.
    QSet<QString> seen;
    _documentWords.clear();

    while (it.hasNext()) {
        QString word = it.next().captured(1);
        // Skip single-char words and very long words.
        if (word.length() < 2 || word.length() > 128) continue;
        // Skip pure numeric strings.
        bool allDigits = true;
        for (const QChar& c : word) {
            if (!c.isDigit()) { allDigits = false; break; }
        }
        if (allDigits) continue;

        if (!seen.contains(word)) {
            seen.insert(word);
            _documentWords << word;
        }
    }

    _envVarCacheValid = false; // Refresh env vars too
}

void AutoCompletion::refreshDocumentWords()
{
    updateDocumentWords();
}

// ─── Per-keystroke update ───────────────────────────────────────────────────────

bool AutoCompletion::update(int character)
{
    if (!_pEditView) return false;
    if (!_autoShow) return false;

    // Determine if we should trigger auto-completion based on the character.
    // Trigger after alphanumeric chars or underscore (start of identifier).
    bool isIdentifierChar = (character >= 'a' && character <= 'z') ||
                            (character >= 'A' && character <= 'Z') ||
                            (character >= '0' && character <= '9') ||
                            character == '_' ||
                            character == '$';

    if (!isIdentifierChar) return false;

    // Get the word prefix at the cursor.
    Sci_CharacterRange cr = _pEditView->getSelection();
    const int curPos = static_cast<int>(cr.cpMin);

    const int wordStart = static_cast<int>(_pEditView->send(SCI_WORDSTARTPOSITION, static_cast<intptr_t>(curPos)));
    const int wordEnd   = static_cast<int>(_pEditView->send(SCI_WORDENDPOSITION,   static_cast<intptr_t>(curPos)));
    const int prefixLen = wordEnd - wordStart;

    if (prefixLen < _triggerThreshold) return false;

    // Refresh words if needed.
    if (_documentWords.isEmpty())
        updateDocumentWords();

    // Build the combined completion list.
    return showAllCompletions();
}

// ─── Core autocomplete logic ───────────────────────────────────────────────────

bool AutoCompletion::showAutoComplete(int /*autocType*/, bool /*autoInsert*/)
{
    if (!_pEditView) return false;

    const QString listStr = currentCompletionList();
    if (listStr.isEmpty()) return false;

    // SCI_AUTOCSETSEPARATOR sets the separator (default ' ').
    // Scintilla will filter the list automatically as the user types.
    _pEditView->send(SCI_AUTOCSETSEPARATOR, ' ');
    // Case sensitivity.
    _pEditView->send(SCI_AUTOCSETIGNORECASE, _caseSensitive ? 0 : 1);
    // AutoInsert: automatically insert the best match when there's only one.
    _pEditView->send(2112 /*SCI_AUTOCSETFILLUPS*/, 0);
    // Cancel on any non-identifier char.
    _pEditView->send(2110 /*SCI_AUTOCSETCANCELATSTART*/, 1);

    _pEditView->send(SCI_AUTOCSHOW, static_cast<uptr_t>(_lastWordPrefix.length()),
                     reinterpret_cast<sptr_t>(listStr.toUtf8().constData()));

    _completionActive = true;
    emit completionShown();
    return true;
}

QString AutoCompletion::currentCompletionList() const
{
    // Gather all applicable completion items.
    QVector<CompletionItem> items;

    // API keywords.
    for (const QString& kw : _keyWordList) {
        CompletionItem it;
        it.text = kw;
        it.displayText = kw;
        it.sortPriority = 0; // highest
        it.isInternal = false;
        items.append(it);
    }

    // Document words (lower priority).
    for (const QString& w : _documentWords) {
        CompletionItem it;
        it.text = w;
        it.displayText = w;
        it.sortPriority = 5;
        it.isInternal = true;
        items.append(it);
    }

    // Environment variables.
    if (_envVarCompletion) {
        const QStringList envVars = buildEnvVarList();
        for (const QString& ev : envVars) {
            CompletionItem it;
            it.text = ev;
            it.displayText = ev;
            it.icon = "envvar";
            it.sortPriority = 8;
            it.isInternal = false;
            items.append(it);
        }
    }

    // Sort.
    applySortMode(items);

    // Build space-separated string for Scintilla.
    QStringList strs;
    strs.reserve(items.size());
    for (const CompletionItem& it : items) {
        strs << it.text;
    }
    return strs.join(' ');
}

// ─── Public API ───────────────────────────────────────────────────────────────

bool AutoCompletion::showApiComplete()
{
    if (!_pEditView || _keyWords.empty()) return false;
    return showAutoComplete(0, false);
}

bool AutoCompletion::showWordComplete(bool /*autoInsert*/)
{
    if (!_pEditView || _documentWords.isEmpty()) return false;
    return showAutoComplete(0, false);
}

bool AutoCompletion::showApiAndWordComplete()
{
    if (!_pEditView) return false;
    updateDocumentWords();
    return showAllCompletions();
}

bool AutoCompletion::showFunctionComplete()
{
    _funcCompletionActive = true;
    // Function completion is similar to API completion but with calltip support.
    return showAutoComplete(1, false);
}

void AutoCompletion::showPathCompletion()
{
    if (!_pEditView) return;

    // Get the path prefix at the cursor.
    Sci_CharacterRange cr = _pEditView->getSelection();
    const int curPos = static_cast<int>(cr.cpMin);

    // Scan back for the start of the path token.
    int start = curPos;
    while (start > 0) {
        --start;
        char ch = static_cast<char>(_pEditView->send(SCI_GETCHARAT, start));
        if (ch == '"' || ch == '<' || ch == '\'' || ch == '\n' || ch == ' ')
            break;
    }
    ++start;

    const int len = curPos - start;
    if (len <= 0) return;

    QByteArray prefixBuf(len + 1, '\0');
    for (int i = 0; i < len; ++i)
        prefixBuf.data()[i] = static_cast<char>(_pEditView->send(SCI_GETCHARAT, start + i));
    prefixBuf.data()[len] = '\0';
    QString prefix = QString::fromUtf8(prefixBuf.constData());

    QStringList paths = buildPathList(prefix);
    if (paths.isEmpty()) return;

    QString listStr = paths.join(' ');
    _pEditView->send(SCI_AUTOCSETSEPARATOR, ' ');
    _pEditView->send(SCI_AUTOCSETIGNORECASE, 1);
    _pEditView->send(SCI_AUTOCSHOW, 0,
                     reinterpret_cast<sptr_t>(listStr.toUtf8().constData()));
    _completionActive = true;
    emit completionShown();
}

void AutoCompletion::showSnippetCompletion()
{
    // Snippet completion: load from SnippetsManager.
    // For now, return — full snippet support is in Wave 6.
    qDebug() << "AutoCompletion::showSnippetCompletion — snippet manager not yet connected";
}

void AutoCompletion::showEnvVarCompletion()
{
    if (!_pEditView) return;
    const QStringList envVars = buildEnvVarList();
    if (envVars.isEmpty()) return;

    QString listStr = envVars.join(' ');
    _pEditView->send(SCI_AUTOCSETSEPARATOR, ' ');
    _pEditView->send(SCI_AUTOCSETIGNORECASE, 1);
    _pEditView->send(SCI_AUTOCSHOW, 1,
                     reinterpret_cast<sptr_t>(listStr.toUtf8().constData()));
    _completionActive = true;
    emit completionShown();
}

bool AutoCompletion::showAllCompletions()
{
    if (!_pEditView) return false;
    updateDocumentWords();
    return showAutoComplete(0, false);
}

// ─── Matched-char insertion ────────────────────────────────────────────────────

void AutoCompletion::insertMatchedChars(int /*character*/, int /*matchedPairConf*/)
{
    // npp-qt: Scintilla handles matched-pair insertion natively
    // via auto-close configuration on the editor widget
}

void AutoCompletion::callTipClick(size_t direction)
{
    if (direction == 0) {
        // SCI_CALLTIPUP not available in QScintilla — no-op
    } else {
        _pEditView->send(SCI_CALLTIPSETFORE, 0);
    }
}

// ─── Close-tag helper (HTML/XML) ──────────────────────────────────────────────

void AutoCompletion::getCloseTag(char* closeTag, size_t closeTagLen,
                                  size_t caretPos, bool isHTML) const
{
    if (!closeTag || closeTagLen == 0 || !_pEditView)
        return;

    closeTag[0] = '\0';

    int scanStart = static_cast<int>(caretPos > 200 ? caretPos - 200 : 0);
    int scanEnd   = static_cast<int>(caretPos);
    QByteArray buf(scanEnd - scanStart + 1, '\0');
    for (int i = scanStart; i < scanEnd; ++i)
        buf.data()[i - scanStart] = static_cast<char>(_pEditView->send(SCI_GETCHARAT, i));

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

// ─── Word usage recording ──────────────────────────────────────────────────────

void AutoCompletion::recordWordUsed(const QString& word)
{
    if (word.isEmpty()) return;
    QString key = word.toLower();
    _wordFrequency[key] = _wordFrequency[key] + 1;
}

// ─── Status ───────────────────────────────────────────────────────────────────

bool AutoCompletion::isActive() const
{
    return _completionActive && _pEditView &&
           (_pEditView->send(2048 /*SCI_AUTOCACTIVE*/) != 0);
}

void AutoCompletion::close()
{
    if (_pEditView && isActive()) {
        _pEditView->send(SCI_AUTOCCANCEL);
    }
    _completionActive = false;
    _funcCompletionActive = false;
    emit completionClosed();
}

// ─── InsertedMatchedChars ──────────────────────────────────────────────────────

void InsertedMatchedChars::add(MatchedCharInserted mci)
{
    _insertedMatchedChars.push_back(mci);
}

intptr_t InsertedMatchedChars::search(char startChar, char endChar, size_t posToDetect)
{
    for (int i = static_cast<int>(_insertedMatchedChars.size()) - 1; i >= 0; --i) {
        const auto& mci = _insertedMatchedChars[i];
        if (mci._c == startChar && mci._pos == posToDetect)
            return static_cast<intptr_t>(mci._pos);
    }
    return -1;
}
