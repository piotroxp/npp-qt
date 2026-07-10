// FindReplaceDialog.h - Find and Replace dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QHideEvent>
#include "../common/Types.h"

class ScintillaEditor;
class QLabel;

class FindReplaceDialog : public QDialog {
    Q_OBJECT
public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);
    ~FindReplaceDialog() override;

    void setEditor(ScintillaEditor* editor);
    void findNext();
    void findPrevious();
    void replace();
    void replaceAll();
    void setSearchText(const QString& text);
    QString lastSearchText() const { return _lastSearchText; }
    FindOption lastSearchOptions() const { return _lastSearchOptions; }

protected:
    void hideEvent(QHideEvent* event) override;

signals:
    void findNextRequested(const QString& text, FindOption options);
    void replaceRequested(const QString& find, const QString& replace, FindOption options);
    void countRequested(const QString& text, FindOption options);

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onCount();
    void onSearchTextChanged(const QString& text);
    void onDirectionChanged(int directionId);

private:
    void setupUi();
    void readSettings();
    void writeSettings();
    FindOption currentOptions() const;
    void updateHighlight();
    void clearHighlight();

    ScintillaEditor* _editor = nullptr;
    QComboBox* _findCombo = nullptr;
    QComboBox* _replaceCombo = nullptr;
    QCheckBox* _matchCaseCheck = nullptr;
    QCheckBox* _wholeWordCheck = nullptr;
    QCheckBox* _wrapCheck = nullptr;
    QCheckBox* _regexCheck = nullptr;
    QPushButton* _findNextBtn = nullptr;
    QPushButton* _replaceBtn = nullptr;
    QPushButton* _replaceAllBtn = nullptr;
    QPushButton* _countBtn = nullptr;
    QPushButton* _closeBtn = nullptr;
    QButtonGroup* _directionGroup = nullptr;
    QLabel* _statusLabel = nullptr;
    QString _lastSearchText;
    FindOption _lastSearchOptions = FindOption::None;
    bool _searchForward = true;
    QString _prevHighlightTerm;
};
