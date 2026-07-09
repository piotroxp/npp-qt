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

signals:
    void findNextRequested(const QString& text, FindOption options);
    void replaceRequested(const QString& find, const QString& replace, FindOption options);

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();

private:
    void setupUi();
    void readSettings();
    void writeSettings();
    FindOption currentOptions() const;

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
    QPushButton* _closeBtn = nullptr;
    QString _lastSearchText;
    FindOption _lastSearchOptions = FindOption::None;
    QLabel* _statusLabel = nullptr;
    int _matchCount = 0;
};
