// FindReplaceDialog.h - Find and Replace dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QFrame>
#include <QHideEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QColor>
#include <QTimer>
#include "../common/Types.h"

class ScintillaEditor;
class FindInFilesDialog;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;

// ============================================================================
// FindReplaceDialog — full-featured Find/Replace with history, marking,
// regex backreferences, and multi-document support
// ============================================================================
class FindReplaceDialog : public QDialog {
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);
    ~FindReplaceDialog() override;

    // --- Editor binding ---
    void setEditor(ScintillaEditor* editor);
    ScintillaEditor* editor() const { return _editor; }

    // --- Search operations ---
    void findNext();
    void findPrevious();
    void replace();
    void replaceAll();
    void countAll();
    void markAll();
    void unmarkAll();
    void purgeMarkedLines();
    void bookmarkAllMatches();

    // --- Selection helpers ---
    void selectAndFind();       // select current word under cursor, then search
    void findNextInSelection(); // search within current selection only
    void replaceInAllOpenDocuments();

    // --- Programmatic access ---
    void setSearchText(const QString& text);
    void setReplaceText(const QString& text);
    void setOptions(FindOption opts);
    QString lastSearchText() const { return _lastSearchText; }
    FindOption lastSearchOptions() const { return _lastSearchOptions; }
    void setMatchCount(int count);
    void setCurrentMatch(int current, int total);
    void setStatusMessage(const QString& msg);

    // --- Find in Files integration ---
    void openFindInFiles();
    void openReplaceInFiles();

    // --- Visibility ---
    void showMinimizedOrFocus(); // show if hidden, or bring to front and focus

Q_SIGNALS:
    void findNextRequested(const QString& text, FindOption options);
    void findPrevRequested(const QString& text, FindOption options);
    void replaceRequested(const QString& find, const QString& replace, FindOption options);
    void replaceAllRequested(const QString& find, const QString& replace, FindOption options);
    void replaceAllInAllRequested(const QString& find, const QString& replace, FindOption options);
    void countRequested(const QString& text, FindOption options);
    void markRequested(const QString& text, FindOption options, const QColor& color, int markStyle);
    void unmarkRequested(const QString& text, FindOption options);
    void purgeRequested(const QString& text, FindOption options);
    void bookmarkRequested(const QString& text, FindOption options);
    void openFindInFilesRequested(const QString& text);
    void openReplaceInFilesRequested(const QString& text, const QString& replace);
    void statusBarMessage(const QString& msg);
    void matchCountChanged(int current, int total);

protected:
    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // --- Button handlers ---
    void onFindNext();
    void onFindPrevious();
    void onReplace();
    void onReplaceAll();
    void onReplaceAllInAll();
    void onCount();
    void onMarkAll();
    void onUnmarkAll();
    void onPurge();
    void onBookmark();
    void onSelectAndFind();
    void onFindInFiles();
    void onReplaceInFiles();

    // --- Option changes ---
    void onSearchTextChanged(const QString& text);
    void onReplaceTextChanged(const QString& text);
    void onDirectionChanged(int directionId);
    void onModeChanged(); // toggled whole-word / regex / case / selection

    // --- Prompt on replace ---
    void onReplacePrompt(const QString& find, const QString& replace, int count);

    // --- Internal helpers ---
    void updateHighlight();
    void clearHighlight();
    void applyHighlightToggle();
    void saveSearchToHistory(const QString& text);
    void saveReplaceToHistory(const QString& text);
    void loadSearchHistory();
    void loadReplaceHistory();
    void updateMarkColor(const QColor& color);
    void scheduleHideForSearch();
    void restoreAfterSearch();

private:
    // --- UI building ---
    void setupUi();
    void setupShortcuts();
    void populateSearchHistory();
    void populateReplaceHistory();

    // --- Settings ---
    void readSettings();
    void writeSettings();
    FindOption currentOptions() const;
    void syncOptionsFromUI();

    // --- Core search ---
    int performFindNext(const QString& text, FindOption opts);
    int performFindPrevious(const QString& text, FindOption opts);
    int performCount(const QString& text, FindOption opts);

    // --- Regex helpers ---
    bool isRegex() const;
    bool isWrapAround() const;
    bool isPromptsOnReplace() const;

    // =========================================================================
    // Widget members
    // =========================================================================
    ScintillaEditor* _editor = nullptr;

    // Search controls
    QComboBox* _findCombo = nullptr;
    QComboBox* _replaceCombo = nullptr;

    // Option checkboxes
    QCheckBox* _matchCaseCheck = nullptr;
    QCheckBox* _wholeWordCheck = nullptr;
    QCheckBox* _wrapCheck = nullptr;
    QCheckBox* _regexCheck = nullptr;
    QCheckBox* _selectionCheck = nullptr;
    QCheckBox* _promptsCheck = nullptr;
    QCheckBox* _hideDialogCheck = nullptr;
    QCheckBox* _highlightingCheck = nullptr;
    QCheckBox* _bookmarkCheck = nullptr;

    // Direction
    QButtonGroup* _directionGroup = nullptr;
    QPushButton* _findNextBtn = nullptr;
    QPushButton* _findPrevBtn = nullptr;
    QPushButton* _replaceBtn = nullptr;
    QPushButton* _replaceAllBtn = nullptr;
    QPushButton* _replaceAllInAllBtn = nullptr;
    QPushButton* _countBtn = nullptr;
    QPushButton* _markBtn = nullptr;
    QPushButton* _unmarkBtn = nullptr;
    QPushButton* _purgeBtn = nullptr;
    QPushButton* _closeBtn = nullptr;
    QPushButton* _findInFilesBtn = nullptr;
    QPushButton* _replaceInFilesBtn = nullptr;

    // Mark color button
    QPushButton* _markColorBtn = nullptr;
    QColor _markColor = Qt::yellow;

    // Status / match counter
    QLabel* _statusLabel = nullptr;
    QLabel* _matchLabel = nullptr;  // e.g. "3 of 47"
    QFrame* _matchFrame = nullptr;

    // Group boxes for layout organization
    QGroupBox* _findGroup = nullptr;
    QGroupBox* _replaceGroup = nullptr;
    QGroupBox* _directionGroupBox = nullptr;
    QGroupBox* _optionsGroup = nullptr;
    QGroupBox* _markGroup = nullptr;
    QGroupBox* _actionsGroup = nullptr;

    // Find-in-files reference
    FindInFilesDialog* _findInFilesDialog = nullptr;

    // =========================================================================
    // State
    // =========================================================================
    QString _lastSearchText;
    FindOption _lastSearchOptions = FindOption::None;
    bool _searchForward = true;
    QString _prevHighlightTerm;

    // History
    static constexpr int MAX_HISTORY = 20;
    QStringList _searchHistory;
    QStringList _replaceHistory;

    // Prompt replace state
    int _replacePromptCount = 0;
    int _replacePromptTotal = 0;

    // Hide-while-searching
    QTimer* _hideTimer = nullptr;
    bool _wasHidden = false;
    bool _highlightingEnabled = true;

    // Mark style
    int _markStyle = 0; // 0=yellow, 1=green, 2=blue, 3=pink, etc.

    // Stored position for selection-only search
    int _selectionAnchor = -1;
    int _selectionCaret = -1;

    // Settings key
    static const QString SETTINGS_GROUP;
    static const QString SETTINGS_RECENT_FINDS;
    static const QString SETTINGS_RECENT_REPLACES;
    static const QString SETTINGS_MATCH_CASE;
    static const QString SETTINGS_WHOLE_WORD;
    static const QString SETTINGS_WRAP;
    static const QString SETTINGS_REGEX;
    static const QString SETTINGS_HIGHLIGHT;
    static const QString SETTINGS_HIDE_DIALOG;
    static const QString SETTINGS_PROMPTS;
    static const QString SETTINGS_MARK_COLOR;
    static const QString SETTINGS_DIRECTION;
    static const QString SETTINGS_GEOMETRY;
};
