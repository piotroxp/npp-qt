// FindInFilesDialog.cpp - Find in Files dialog implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindInFilesDialog.h"
#include "FindInFilesWorker.h"
#include "../common/Types.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QApplication>

// ============================================================================
// Construction / UI Setup
// ============================================================================
FindInFilesDialog::FindInFilesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Find in Files");
    setModal(false);
    setMinimumWidth(700);
    setMinimumHeight(450);
    setupUi();
}

FindInFilesDialog::~FindInFilesDialog() {
    onCancelSearch();
}

void FindInFilesDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // --- Search row ---
    QHBoxLayout* searchRow = new QHBoxLayout();
    _searchEdit = new QLineEdit(this);
    _searchEdit->setPlaceholderText("Find...");
    _searchEdit->setMinimumWidth(250);
    QLabel* searchLbl = new QLabel("Find:", this);
    searchLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    searchRow->addWidget(searchLbl);
    searchRow->addWidget(_searchEdit, 1);

    // --- Directory row ---
    QHBoxLayout* dirRow = new QHBoxLayout();
    _dirEdit = new QLineEdit(this);
    _dirEdit->setPlaceholderText("Directory...");
    _dirEdit->setMinimumWidth(250);
    QLabel* dirLbl = new QLabel("In Directory:", this);
    dirLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QPushButton* browseBtn = new QPushButton("...", this);
    browseBtn->setMaximumWidth(30);
    connect(browseBtn, &QPushButton::clicked, this, &FindInFilesDialog::onBrowseDir);
    dirRow->addWidget(dirLbl);
    dirRow->addWidget(_dirEdit, 1);
    dirRow->addWidget(browseBtn);

    // --- Filter row ---
    QHBoxLayout* filterRow = new QHBoxLayout();
    _filterEdit = new QLineEdit(this);
    _filterEdit->setPlaceholderText("e.g. *.cpp *.h *.txt");
    _filterEdit->setText("*");
    QLabel* filterLbl = new QLabel("Filters:", this);
    filterLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    filterRow->addWidget(filterLbl);
    filterRow->addWidget(_filterEdit, 1);

    // --- Options row ---
    QHBoxLayout* optionsRow = new QHBoxLayout();
    _matchCaseCheck = new QCheckBox("Match Case", this);
    _wholeWordCheck = new QCheckBox("Whole Word", this);
    _regexCheck     = new QCheckBox("Regex", this);
    _wrapCheck      = new QCheckBox("Wrap", this);
    _wrapCheck->setChecked(true);
    optionsRow->addWidget(_matchCaseCheck);
    optionsRow->addWidget(_wholeWordCheck);
    optionsRow->addWidget(_regexCheck);
    optionsRow->addWidget(_wrapCheck);
    optionsRow->addStretch();

    // --- Replace row ---
    QHBoxLayout* replaceRow = new QHBoxLayout();
    _replaceEdit = new QLineEdit(this);
    _replaceEdit->setPlaceholderText("Replace with...");
    QLabel* replaceLbl = new QLabel("Replace:", this);
    replaceLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    replaceRow->addWidget(replaceLbl);
    replaceRow->addWidget(_replaceEdit, 1);

    // --- Button row ---
    QHBoxLayout* btnRow = new QHBoxLayout();
    _findBtn       = new QPushButton("Find All", this);
    _replaceAllBtn = new QPushButton("Replace in Files", this);
    _cancelBtn     = new QPushButton("Cancel", this);
    _cancelBtn->setVisible(false);
    _closeBtn      = new QPushButton("Close", this);
    btnRow->addWidget(_findBtn);
    btnRow->addWidget(_replaceAllBtn);
    btnRow->addWidget(_cancelBtn);
    btnRow->addStretch();
    btnRow->addWidget(_closeBtn);

    // --- Progress bar ---
    _progressBar = new QProgressBar(this);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(0);  // indeterminate
    _progressBar->setVisible(false);

    // --- Status label ---
    _statusLabel = new QLabel(this);
    _statusLabel->setStyleSheet("QLabel { color: palette(highlight-text); }");

    // --- Results tree ---
    _resultsTree = new QTreeWidget(this);
    _resultsTree->setHeaderLabels({"File", "Line", "Content"});
    _resultsTree->setSelectionBehavior(QTreeWidget::SelectRows);
    _resultsTree->setEditTriggers(QTreeWidget::NoEditTriggers);
    _resultsTree->setAlternatingRowColors(true);
    _resultsTree->header()->setStretchLastSection(true);
    _resultsTree->header()->resizeSection(0, 250);
    _resultsTree->header()->resizeSection(1, 50);

    // Context menu: Copy line, Copy path
    _resultsTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_resultsTree, &QTreeWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) {
        QTreeWidgetItem* item = _resultsTree->itemAt(pos);
        if (!item) return;
        QMenu menu(this);
        QAction* copyLine = menu.addAction("Copy Line");
        QAction* copyPath = menu.addAction("Copy File Path");
        QAction* action = menu.exec(_resultsTree->mapToGlobal(pos));
        if (action == copyLine) {
            QApplication::clipboard()->setText(item->text(2));
        } else if (action == copyPath) {
            QApplication::clipboard()->setText(item->text(0));
        }
    });

    // --- Layout assembly ---
    mainLayout->addLayout(searchRow);
    mainLayout->addLayout(dirRow);
    mainLayout->addLayout(filterRow);
    mainLayout->addLayout(optionsRow);
    mainLayout->addLayout(replaceRow);
    mainLayout->addLayout(btnRow);
    mainLayout->addWidget(_progressBar);
    mainLayout->addWidget(_statusLabel);
    mainLayout->addWidget(_resultsTree, 1);

    // --- Connections ---
    connect(_findBtn, &QPushButton::clicked, this, &FindInFilesDialog::onFind);
    connect(_replaceAllBtn, &QPushButton::clicked, this, &FindInFilesDialog::onReplaceAll);
    connect(_cancelBtn, &QPushButton::clicked, this, &FindInFilesDialog::onCancelSearch);
    connect(_closeBtn, &QPushButton::clicked, this, &QDialog::hide);
    connect(_resultsTree, &QTreeWidget::itemDoubleClicked,
            this, &FindInFilesDialog::onResultDoubleClicked);
    connect(_searchEdit, &QLineEdit::returnPressed, this, &FindInFilesDialog::onFind);

    // Load last directory from settings
    QSettings s;
    _dirEdit->setText(s.value("FindInFiles/directory", QDir::homePath()).toString());
    _filterEdit->setText(s.value("FindInFiles/filters", "*").toString());
}

// ============================================================================
// Directory / filter helpers
// ============================================================================
QString FindInFilesDialog::currentDirectory() const {
    return _dirEdit->text().trimmed();
}

QString FindInFilesDialog::currentFilter() const {
    return _filterEdit->text().trimmed();
}

// ============================================================================
// Public setters
// ============================================================================
void FindInFilesDialog::setDirectory(const QString& dir) {
    _dirEdit->setText(dir);
}

void FindInFilesDialog::setSearchText(const QString& text) {
    _searchEdit->setText(text);
}

// ============================================================================
// Browse
// ============================================================================
void FindInFilesDialog::onBrowseDir() {
    QString dir = QFileDialog::getExistingDirectory(
        this, "Select Directory", _dirEdit->text());
    if (!dir.isEmpty()) {
        _dirEdit->setText(dir);
        QSettings s;
        s.setValue("FindInFiles/directory", dir);
    }
}

// ============================================================================
// Search
// ============================================================================
void FindInFilesDialog::onFind() {
    QString searchText = _searchEdit->text().trimmed();
    QString dir = currentDirectory();
    if (searchText.isEmpty()) {
        _statusLabel->setText("  Please enter a search term.");
        return;
    }
    if (dir.isEmpty() || !QDir(dir).exists()) {
        _statusLabel->setText("  Please select a valid directory.");
        return;
    }

    // Save settings
    QSettings s;
    s.setValue("FindInFiles/directory", dir);
    s.setValue("FindInFiles/filters", currentFilter());

    // Clear previous results
    _resultsTree->clear();
    _allResults.clear();
    _searchCancelled = false;

    startSearch();
}

void FindInFilesDialog::startSearch() {
    QString searchText = _searchEdit->text().trimmed();
    QString dir = currentDirectory();
    QString filters = currentFilter();

    bool matchCase  = _matchCaseCheck->isChecked();
    bool wholeWord  = _wholeWordCheck->isChecked();
    bool regex      = _regexCheck->isChecked();

    // Cancel any existing search
    onCancelSearch();

    // Create worker thread
    _workerThread = new QThread(this);
    _worker = new FindInFilesWorker(dir, searchText, filters, matchCase, wholeWord, regex);
    _worker->moveToThread(_workerThread);

    connect(_workerThread, &QThread::started, _worker, &FindInFilesWorker::run);
    connect(_worker, &FindInFilesWorker::resultsReady, this, &FindInFilesDialog::addResults,
            Qt::QueuedConnection);
    connect(_worker, &FindInFilesWorker::progress, this, &FindInFilesDialog::onWorkerProgress,
            Qt::QueuedConnection);
    connect(_worker, &FindInFilesWorker::finished, this, &FindInFilesDialog::onWorkerFinished,
            Qt::QueuedConnection);

    _workerThread->start();
    setSearching(true);
    _statusLabel->setText("  Searching...");
}

void FindInFilesDialog::setSearching(bool searching) {
    _findBtn->setVisible(!searching);
    _cancelBtn->setVisible(searching);
    _searchEdit->setEnabled(!searching);
    _dirEdit->setEnabled(!searching);
    _filterEdit->setEnabled(!searching);
    _matchCaseCheck->setEnabled(!searching);
    _wholeWordCheck->setEnabled(!searching);
    _regexCheck->setEnabled(!searching);
    _progressBar->setVisible(searching);
}

void FindInFilesDialog::onCancelSearch() {
    if (_workerThread) {
        _workerThread->quit();
        if (_workerThread->wait(1000)) {
            // ok
        } else {
            _workerThread->terminate();
            _workerThread->wait();
        }
        if (_worker) {
            _worker->deleteLater();
            _worker = nullptr;
        }
        _workerThread->deleteLater();
        _workerThread = nullptr;
    }
    setSearching(false);
}

void FindInFilesDialog::onWorkerFinished() {
    setSearching(false);
    int count = _allResults.size();
    QString msg = count == 0
        ? "  No matches found."
        : QString("  Found %1 match%2 in %3 file%4.")
              .arg(count)
              .arg(count == 1 ? "" : "es")
              .arg(groupResultsByFile().size())
              .arg(groupResultsByFile().size() == 1 ? "" : "s");
    _statusLabel->setText(msg);
}

void FindInFilesDialog::onWorkerProgress(const QString& file, int filesSearched) {
    Q_UNUSED(filesSearched);
    _statusLabel->setText(QString("  Searching: %1").arg(file));
    QApplication::processEvents();
}

// ============================================================================
// Results
// ============================================================================
void FindInFilesDialog::addResults(const QList<FindResult>& results) {
    if (_searchCancelled) return;

    for (const FindResult& r : results) {
        // Group by file: check if there's already a top-level item for this file
        QTreeWidgetItem* fileItem = nullptr;
        for (int i = 0; i < _resultsTree->topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = _resultsTree->topLevelItem(i);
            if (item && item->text(0) == r.filePath) {
                fileItem = item;
                break;
            }
        }

        if (!fileItem) {
            fileItem = new QTreeWidgetItem(_resultsTree);
            fileItem->setText(0, r.filePath);
            fileItem->setFirstColumnSpanned(true);
            // Make file items bold
            QFont f = fileItem->font(0);
            f.setBold(true);
            fileItem->setFont(0, f);
            _resultsTree->addTopLevelItem(fileItem);
        }

        // Add line item as child
        QTreeWidgetItem* lineItem = new QTreeWidgetItem(fileItem);
        lineItem->setText(0, QString::number(r.lineNumber));
        lineItem->setText(1, r.lineContent);
        // Store full path in data
        lineItem->setData(0, Qt::UserRole, r.filePath);
        lineItem->setData(0, Qt::UserRole + 1, r.lineNumber);
        fileItem->addChild(lineItem);
    }

    // Expand all top-level items
    for (int i = 0; i < _resultsTree->topLevelItemCount(); ++i) {
        _resultsTree->topLevelItem(i)->setExpanded(true);
    }

    _allResults.append(results);
    int count = _allResults.size();
    _statusLabel->setText(QString("  %1 match%2 found...").arg(count).arg(count == 1 ? "" : "es"));
}

QMap<QString, QList<FindResult>> FindInFilesDialog::groupResultsByFile() const {
    QMap<QString, QList<FindResult>> groups;
    for (const FindResult& r : _allResults) {
        groups[r.filePath].append(r);
    }
    return groups;
}

void FindInFilesDialog::onResultDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;

    // Get path and line from item (stored in UserRole/UserRole+1)
    QString path = item->data(0, Qt::UserRole).toString();
    int lineNumber = item->data(0, Qt::UserRole + 1).toInt();

    // For top-level (file) items, get the first child's data
    if (!item->parent()) {
        if (item->childCount() > 0) {
            QTreeWidgetItem* firstChild = item->child(0);
            path = firstChild->data(0, Qt::UserRole).toString();
            lineNumber = firstChild->data(0, Qt::UserRole + 1).toInt();
        } else {
            return;
        }
    }

    if (!path.isEmpty()) {
        emit openFile(path, lineNumber);
    }
}

// ============================================================================
// Replace All
// ============================================================================
void FindInFilesDialog::onReplaceAll() {
    QString replacement = _replaceEdit->text();
    if (_allResults.isEmpty()) {
        QMessageBox::information(this, "Replace in Files",
            "No search results. Run a Find first.");
        return;
    }

    int fileCount = groupResultsByFile().size();
    int reply = QMessageBox::question(this, "Replace in Files",
        QString("Replace %1 occurrences across %2 file%3?\n\nThis will modify files on disk.")
            .arg(_allResults.size())
            .arg(fileCount)
            .arg(fileCount == 1 ? "" : "s"),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    // Group results by file
    QMap<QString, QList<FindResult>> groups = groupResultsByFile();

    int replaced = 0;
    int errors = 0;
    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
        const QString& path = it.key();
        const QList<FindResult>& fileResults = it.value();

        QFile file(path);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            qWarning() << "[FindInFiles] Cannot open file for replace:" << path;
            ++errors;
            continue;
        }

        // Read all lines
        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }

        // Apply replacements per line. Since multiple matches may exist on
        // the same line, we track a running offset to adjust column positions.
        int lineOffset = 0;
        int prevLine = -1;
        for (const FindResult& r : fileResults) {
            int lineIdx = r.lineNumber - 1;
            if (lineIdx < 0 || lineIdx >= (int)lines.size()) continue;

            // If we're on a new line, reset the offset
            if (r.lineNumber != prevLine) {
                lineOffset = 0;
                prevLine = r.lineNumber;
            }

            QString& line = lines[lineIdx];
            int pos = r.column - lineOffset;
            if (pos >= 0 && pos <= line.length()) {
                int matchLen = r.lineContent.length();  // approximate matched text length
                line.replace(pos, matchLen, replacement);
                lineOffset += (matchLen - replacement.length());
                ++replaced;
            }
        }

        // Truncate and rewrite
        file.resize(0);
        QTextStream out(&file);
        for (const QString& line : lines) {
            out << line << "\n";
        }
        file.close();
    }

    QString msg;
    if (errors > 0) {
        msg = QString("Replaced %1 occurrence%2 in %3 file%4 (%5 errors)")
                  .arg(replaced).arg(replaced == 1 ? "" : "s")
                  .arg(fileCount).arg(fileCount == 1 ? "" : "s")
                  .arg(errors);
    } else {
        msg = QString("Replaced %1 occurrence%2 in %3 file%4.")
                  .arg(replaced).arg(replaced == 1 ? "" : "s")
                  .arg(fileCount).arg(fileCount == 1 ? "" : "s");
    }
    _statusLabel->setText(msg);

    // Clear results after replace
    _resultsTree->clear();
    _allResults.clear();
}
