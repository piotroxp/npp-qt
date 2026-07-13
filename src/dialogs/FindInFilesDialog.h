// FindInFilesDialog.h - Find in Files dialog (directory search)
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTreeWidget>
#include <QThread>

class FindInFilesWorker;
struct FindResult;

// ============================================================================
// FindInFilesDialog — directory-wide search with replace-all support
// ============================================================================
class FindInFilesDialog : public QDialog {
    Q_OBJECT

public:
    explicit FindInFilesDialog(QWidget* parent = nullptr);
    ~FindInFilesDialog() override;

    void setDirectory(const QString& dir);
    void setSearchText(const QString& text);

signals:
    // Emitted when user double-clicks a result — open file at line
    void openFile(const QString& path, int lineNumber);

private slots:
    void onBrowseDir();
    void onFind();
    void onCancelSearch();
    void onReplaceAll();
    void onResultDoubleClicked(QTreeWidgetItem* item, int column);
    void onWorkerFinished();
    void onWorkerProgress(const QString& file, int filesSearched);

private:
    void startSearch();
    void addResults(const QList<FindResult>& results);
    void replaceInFile(const QString& path, const QList<FindResult>& fileResults,
                       const QString& replacement);
    QString currentDirectory() const;
    QString currentFilter() const;
    void setSearching(bool searching);

    // UI
    QLineEdit*   _searchEdit   = nullptr;
    QLineEdit*   _replaceEdit  = nullptr;
    QLineEdit*   _dirEdit      = nullptr;
    QLineEdit*   _filterEdit   = nullptr;
    QCheckBox*   _matchCaseCheck    = nullptr;
    QCheckBox*   _wholeWordCheck   = nullptr;
    QCheckBox*   _regexCheck       = nullptr;
    QCheckBox*   _wrapCheck        = nullptr;
    QPushButton* _findBtn      = nullptr;
    QPushButton* _replaceAllBtn = nullptr;
    QPushButton* _cancelBtn    = nullptr;
    QPushButton* _closeBtn     = nullptr;
    QProgressBar* _progressBar = nullptr;
    QLabel*      _statusLabel  = nullptr;
    QTreeWidget* _resultsTree  = nullptr;

    // Background search
    QThread*           _workerThread = nullptr;
    FindInFilesWorker* _worker       = nullptr;
    QList<FindResult>  _allResults;
    bool               _searchCancelled = false;
};
