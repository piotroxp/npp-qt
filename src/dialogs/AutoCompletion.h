// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include <QObject>
#include <QString>
#include <QRgb>
#include <QStringList>
#include <QVector>
#include <QTimer>
#include <vector>
#include <string>
#include <unordered_map>
#include "common/ScintillaComponent.h"

struct MatchedCharInserted {
    MatchedCharInserted() = delete;
    char _c;
    size_t _pos;
    MatchedCharInserted(char c, size_t pos) : _c(c), _pos(pos) {}
};

class InsertedMatchedChars {
public:
    explicit InsertedMatchedChars(ScintillaComponent* pEditView = nullptr)
        : _pEditView(pEditView) {}
    void init(ScintillaComponent* pEditView) { _pEditView = pEditView; }
    void add(MatchedCharInserted mci);
    bool isEmpty() const { return _insertedMatchedChars.empty(); }
    intptr_t search(char startChar, char endChar, size_t posToDetect);
private:
    std::vector<MatchedCharInserted> _insertedMatchedChars;
    ScintillaComponent* _pEditView = nullptr;
};

/// Completion item with type and optional icon.
struct CompletionItem {
    QString text;
    QString displayText;   // may include a brief sig, e.g. "foo(int a, int b)"
    QString docComment;     // one-line doc tooltip
    QString icon;          // "function", "class", "keyword", "word", "path", "snippet", "envvar"
    int sortPriority = 0;  // lower = higher in list (0 = highest)
    bool isInternal = false; // true = word from document, not from API

    bool operator<(const CompletionItem& other) const {
        if (sortPriority != other.sortPriority)
            return sortPriority < other.sortPriority;
        return text.compare(other.text, Qt::CaseInsensitive) < 0;
    }
};

class AutoCompletion : public QObject
{
    Q_OBJECT
public:
    enum class AutocompleteColorIndex {
        autocompleteText,
        autocompleteBg,
        selectedText,
        selectedBg,
        calltipBg,
        calltipText,
        calltipHighlight
    };

    enum class CompletionType {
        None        = 0,
        Api         = 1 << 0,  // language keywords / API
        Word        = 1 << 1,  // words from document
        Function    = 1 << 2,  // function signatures
        Path        = 1 << 3,  // file paths (for #include, import, etc.)
        Snippet     = 1 << 4,  // code snippets
        EnvVar      = 1 << 5,  // $VAR, ${VAR}
    };
    Q_DECLARE_FLAGS(CompletionTypes, CompletionType)

    // Bitwise operators for scoped enum (Q_DECLARE_OPERATORS_FOR_FLAGS doesn't work with enum class)
    friend constexpr CompletionTypes operator|(CompletionType a, CompletionType b) noexcept
        { return CompletionTypes(a) | CompletionTypes(b); }
    friend constexpr CompletionTypes operator~(CompletionType a) noexcept
        { return ~CompletionTypes(a); }

    enum class SortMode {
        Alphabetical,   // A→Z
        ByFrequency,    // most-used first
        ByLength,       // shortest first
    };

    // ── Colour configuration ─────────────────────────────────────────────────
    static void setColour(QRgb colour2Set, AutocompleteColorIndex i);

    // ── Construction ────────────────────────────────────────────────────────
    explicit AutoCompletion(ScintillaComponent* pEditView);
    ~AutoCompletion() override;

    // ── Language setup ──────────────────────────────────────────────────────
    bool setLanguage(int language);
    void setCaseSensitive(bool on);
    bool isCaseSensitive() const { return _caseSensitive; }

    // ── Trigger configuration ────────────────────────────────────────────────
    /// Minimum characters before auto-trigger (default 2).
    void setTriggerThreshold(int n) { _triggerThreshold = qMax(1, n); }
    int triggerThreshold() const { return _triggerThreshold; }

    /// Whether to auto-show completions as user types.
    void setAutoShow(bool on) { _autoShow = on; }
    void setConstructionComplete(bool v) { _constructionComplete = v; }
    bool autoShow() const { return _autoShow; }

    /// Automatically insert the common prefix of all matches.
    void setAutoInsertPrefix(bool on) { _autoInsertPrefix = on; }
    bool autoInsertPrefix() const { return _autoInsertPrefix; }

    /// Show function parameter info (calltip) after completing a function name.
    void setShowParamInfo(bool on) { _showParamInfo = on; }
    bool showParamInfo() const { return _showParamInfo; }

    /// Enable environment-variable completion ($VAR, ${VAR}).
    void setEnvVarCompletion(bool on);
    bool envVarCompletion() const { return _envVarCompletion; }

    // ── Sorting ─────────────────────────────────────────────────────────────
    void setSortMode(SortMode mode);
    SortMode sortMode() const { return _sortMode; }

    // ── Manual show functions ────────────────────────────────────────────────
    /// Show API/keyword completion.
    bool showApiComplete();

    /// Show word completion from document.
    bool showWordComplete(bool autoInsert);

    /// Show API + word + function + path combined.
    bool showApiAndWordComplete();

    /// Show function-signature completion (with calltip).
    bool showFunctionComplete();

    /// Show path completion for #include / import / require.
    void showPathCompletion();

    /// Show snippet completion.
    void showSnippetCompletion();

    /// Show environment-variable completion.
    void showEnvVarCompletion();

    /// Show all applicable completion types at once.
    bool showAllCompletions();

    // ── Completion list (for Scintilla) ─────────────────────────────────────
    /// Get the current completion list as a space-separated string
    /// suitable for SCI_AUTOCSHOW.
    QString currentCompletionList() const;

    // ── Matched chars ────────────────────────────────────────────────────────
    void insertMatchedChars(int character, int matchedPairConf);
    void getCloseTag(char* closeTag, size_t closeTagLen,
                     size_t caretPos, bool isHTML) const;

    // ── Per-keystroke update ────────────────────────────────────────────────
    /// Called on each character typed. Returns true if completion was triggered.
    bool update(int character);

    // ── Calltip integration ─────────────────────────────────────────────────
    void callTipClick(size_t direction);

    // ── Document words ──────────────────────────────────────────────────────
    /// Rescan the current document for words and rebuild word cache.
    void refreshDocumentWords();

    /// Add a word to the usage-frequency map.
    void recordWordUsed(const QString& word);

    // ── Status ──────────────────────────────────────────────────────────────
    bool isActive() const;
    void close();

signals:
    /// Emitted when the completion popup is shown.
    void completionShown();
    /// Emitted when the completion popup is dismissed.
    void completionClosed();
    /// Emitted when a completion item is selected.
    void completionSelected(const QString& text);

private:
    bool showAutoComplete(int autocType, bool autoInsert);
    QStringList buildWordList() const;
    QStringList buildEnvVarList() const;
    QStringList buildPathList(const QString& prefix) const;
    void updateDocumentWords();
    QString commonPrefix(const QStringList& items) const;
    void applySortMode(QVector<CompletionItem>& items) const;

    ScintillaComponent* _pEditView = nullptr;
    int _curLang = 0;
    bool _caseSensitive = false;

    // Keywords / API
    std::string _keyWords;
    QStringList _keyWordList;

    // Document words
    QStringList _documentWords;
    QString _lastWordPrefix;

    // Completion types
    CompletionTypes _enabledTypes = CompletionType::Api |
                                    CompletionType::Word |
                                    CompletionType::Function |
                                    CompletionType::Snippet |
                                    CompletionType::EnvVar;
    int _triggerThreshold = 2;
    bool _autoShow = true;
    bool _autoInsertPrefix = true;
    bool _showParamInfo = true;
    bool _envVarCompletion = true;
    bool _funcCompletionActive = false;
    SortMode _sortMode = SortMode::Alphabetical;

    // Usage frequency (for ByFrequency sort)
    std::unordered_map<QString, int> _wordFrequency;

    // Current completion state
    QVector<CompletionItem> _currentItems;
    QString _currentWord;
    bool _completionActive = false;
    bool _constructionComplete = false;  // set true after widget is shown (valid QScreen exists)
    int _selectedIndex = -1;

    InsertedMatchedChars _insertedMatchedChars;

    // Environment variables (lazy-populated)
    mutable QStringList _cachedEnvVars;
    mutable bool _envVarCacheValid = false;
};
