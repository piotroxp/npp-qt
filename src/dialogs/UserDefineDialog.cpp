// UserDefineDialog.cpp - User-Defined Language editor dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#include "UserDefineDialog.h"
#include "UdlManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QColorDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QApplication>

// ============================================================================
// Construction
// ============================================================================

UserDefineDialog::UserDefineDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("User-Defined Language Editor"));
    setMinimumSize(700, 500);
    setupUi();
}

UserDefineDialog::~UserDefineDialog() = default;

// ============================================================================
// setupUi
// ============================================================================

void UserDefineDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // --- Language name & extensions row ---
    QGroupBox* infoGroup = new QGroupBox(tr("Language Info"), this);
    QFormLayout* infoLayout = new QFormLayout(infoGroup);
    _nameEdit = new QLineEdit(infoGroup);
    _nameEdit->setPlaceholderText(tr("Language name, e.g. MyScript"));
    _extEdit = new QLineEdit(infoGroup);
    _extEdit->setPlaceholderText(tr("Extensions separated by space, e.g. mss sql"));
    infoLayout->addRow(tr("Name:"), _nameEdit);
    infoLayout->addRow(tr("Extensions:"), _extEdit);
    mainLayout->addWidget(infoGroup);

    // --- Tab widget ---
    QTabWidget* tabs = new QTabWidget(this);
    createKeywordsTab(tabs);
    createCommentsTab(tabs);
    createNumbersTab(tabs);
    createOperatorsTab(tabs);
    createDelimitersTab(tabs);
    createFoldingTab(tabs);
    mainLayout->addWidget(tabs);

    // --- Buttons ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton* okBtn = new QPushButton(tr("OK"), this);
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
}

// ============================================================================
// Keywords Tab
// ============================================================================

void UserDefineDialog::createKeywordsTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* hint = new QLabel(
        tr("Enter keywords separated by spaces. Use a table row for each keyword group."),
        page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    // 9 keyword groups: words0..words8
    for (int i = 0; i < 9; ++i) {
        QGroupBox* group = new QGroupBox(
            tr("Keywords %1").arg(i == 0 ? "(Primary)" : QString("(%1)").arg(i)), page);
        QHBoxLayout* hbox = new QHBoxLayout(group);

        QTableWidget* table = new QTableWidget(group);
        table->setColumnCount(1);
        table->setHorizontalHeaderLabels({tr("Keywords (space-separated)")});
        table->horizontalHeader()->setStretchLastSection(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setMinimumHeight(60);

        // Add/Remove buttons
        QPushButton* addBtn = new QPushButton(tr("+ Add"), group);
        QPushButton* remBtn = new QPushButton(tr("Remove"), group);
        connect(addBtn, &QPushButton::clicked, [table]() {
            int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString()));
            table->setCurrentCell(row, 0);
        });
        connect(remBtn, &QPushButton::clicked, [table]() {
            int row = table->currentRow();
            if (row >= 0)
                table->removeRow(row);
        });

        hbox->addWidget(table, 1);
        QVBoxLayout* btnBox = new QVBoxLayout();
        btnBox->addWidget(addBtn);
        btnBox->addWidget(remBtn);
        btnBox->addStretch();
        hbox->addLayout(btnBox);

        layout->addWidget(group);
        _keywordTables[i] = table;
    }

    layout->addStretch();
    tabs->addTab(page, tr("Keywords"));
}

void UserDefineDialog::setupKeywordTable(QTableWidget* table, const QString& keywords) {
    table->setRowCount(0);
    if (keywords.isEmpty())
        return;
    // Each row holds one group of space-separated keywords
    QStringList items = keywords.split('\n', Qt::SkipEmptyParts);
    for (const QString& item : items) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(item.trimmed()));
    }
    if (table->rowCount() == 0) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString()));
    }
}

QString UserDefineDialog::keywordsFromTable(QTableWidget* table) const {
    QStringList lines;
    for (int r = 0; r < table->rowCount(); ++r) {
        QTableWidgetItem* item = table->item(r, 0);
        if (item && !item->text().trimmed().isEmpty())
            lines.append(item->text().trimmed());
    }
    return lines.join("\n");
}

// ============================================================================
// Comments Tab
// ============================================================================

void UserDefineDialog::createCommentsTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QFormLayout* layout = new QFormLayout(page);

    _lineCommentEdit = new QLineEdit(page);
    _lineCommentEdit->setPlaceholderText(tr("//"));
    layout->addRow(tr("Line comment:"), _lineCommentEdit);

    _blockCommentStartEdit = new QLineEdit(page);
    _blockCommentStartEdit->setPlaceholderText(tr("/*"));
    layout->addRow(tr("Block comment start:"), _blockCommentStartEdit);

    _blockCommentEndEdit = new QLineEdit(page);
    _blockCommentEndEdit->setPlaceholderText(tr("*/"));
    layout->addRow(tr("Block comment end:"), _blockCommentEndEdit);

    _enableCommentFold = new QCheckBox(tr("Enable folder (folding) in comments"), page);
    layout->addRow(QString(), _enableCommentFold);

    layout->addStretch(1);
    tabs->addTab(page, tr("Comments"));
}

// ============================================================================
// Numbers Tab
// ============================================================================

void UserDefineDialog::createNumbersTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Default colors (dark-theme-friendly)
    _numberFgColor = Qt::red;
    _numberBgColor = QColor(0, 0, 0, 0);  // transparent / no background

    QGroupBox* styleGroup = new QGroupBox(tr("Number Style"), page);
    QFormLayout* styleLayout = new QFormLayout(styleGroup);

    // Foreground color
    QHBoxLayout* fgRow = new QHBoxLayout();
    _numberFgPreview = new QLabel(styleGroup);
    _numberFgPreview->setFixedSize(60, 20);
    _numberFgPreview->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888; color: white;")
            .arg(_numberFgColor.name()));
    _numberFgPreview->setText(tr("Fg"));
    _numberFgPreview->setAlignment(Qt::AlignCenter);
    _numberFgBtn = new QPushButton(tr("Choose…"), styleGroup);
    connect(_numberFgBtn, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(_numberFgColor, this, tr("Number Foreground"));
        if (c.isValid()) {
            _numberFgColor = c;
            updateNumberPreviews();
        }
    });
    fgRow->addWidget(_numberFgPreview);
    fgRow->addWidget(_numberFgBtn);
    fgRow->addStretch();
    styleLayout->addRow(tr("Foreground:"), fgRow);

    // Background color
    QHBoxLayout* bgRow = new QHBoxLayout();
    _numberBgPreview = new QLabel(styleGroup);
    _numberBgPreview->setFixedSize(60, 20);
    _numberBgPreview->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888; color: black;")
            .arg(_numberBgColor.name()));
    _numberBgPreview->setText(tr("Bg"));
    _numberBgPreview->setAlignment(Qt::AlignCenter);
    _numberBgBtn = new QPushButton(tr("Choose…"), styleGroup);
    connect(_numberBgBtn, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(_numberBgColor, this, tr("Number Background"));
        if (c.isValid()) {
            _numberBgColor = c;
            updateNumberPreviews();
        }
    });
    bgRow->addWidget(_numberBgPreview);
    bgRow->addWidget(_numberBgBtn);
    bgRow->addStretch();
    styleLayout->addRow(tr("Background:"), bgRow);

    // Bold / Italic
    QHBoxLayout* styleRow = new QHBoxLayout();
    _numberBold = new QCheckBox(tr("Bold"), styleGroup);
    _numberItalic = new QCheckBox(tr("Italic"), styleGroup);
    styleRow->addWidget(_numberBold);
    styleRow->addWidget(_numberItalic);
    styleRow->addStretch();
    styleLayout->addRow(tr("Style:"), styleRow);

    layout->addWidget(styleGroup);
    layout->addStretch();
    tabs->addTab(page, tr("Numbers"));
}

void UserDefineDialog::updateNumberPreviews() {
    _numberFgPreview->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888; color: white;")
            .arg(_numberFgColor.name()));
    _numberBgPreview->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888; color: black;")
            .arg(_numberBgColor.name()));
}

// ============================================================================
// Operators Tab
// ============================================================================

void UserDefineDialog::createOperatorsTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* hint = new QLabel(
        tr("Operators: single-character tokens to highlight as operators "
           "(e.g. + - * / = < > & | ! ~ ^ %%)"),
        page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    _operatorTable = new QTableWidget(page);
    _operatorTable->setColumnCount(1);
    _operatorTable->setHorizontalHeaderLabels({tr("Operator Character")});
    _operatorTable->horizontalHeader()->setStretchLastSection(true);
    _operatorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _operatorTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _operatorTable->setMinimumHeight(150);

    // Pre-populate with default C-family operators
    const QString defaultOps = "+-*/%=<>!&|^~?:,;().[]{}@#$`\\";
    for (QChar ch : defaultOps) {
        int row = _operatorTable->rowCount();
        _operatorTable->insertRow(row);
        _operatorTable->setItem(row, 0, new QTableWidgetItem(QString(ch)));
    }

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton(tr("+ Add"), page);
    QPushButton* remBtn = new QPushButton(tr("Remove"), page);
    QPushButton* clearBtn = new QPushButton(tr("Clear"), page);
    btnRow->addWidget(addBtn);
    btnRow->addWidget(remBtn);
    btnRow->addWidget(clearBtn);
    btnRow->addStretch();

    connect(addBtn, &QPushButton::clicked, [this]() {
        int row = _operatorTable->rowCount();
        _operatorTable->insertRow(row);
        _operatorTable->setItem(row, 0, new QTableWidgetItem(QString()));
        _operatorTable->setCurrentCell(row, 0);
        _operatorTable->editItem(_operatorTable->item(row, 0));
    });
    connect(remBtn, &QPushButton::clicked, [this]() {
        int r = _operatorTable->currentRow();
        if (r >= 0) _operatorTable->removeRow(r);
    });
    connect(clearBtn, &QPushButton::clicked, [this]() {
        _operatorTable->setRowCount(0);
    });

    layout->addWidget(_operatorTable);
    layout->addLayout(btnRow);
    layout->addStretch();
    tabs->addTab(page, tr("Operators"));
}

QString UserDefineDialog::operatorsFromTable() const {
    QString result;
    for (int r = 0; r < _operatorTable->rowCount(); ++r) {
        QTableWidgetItem* item = _operatorTable->item(r, 0);
        if (item) {
            QString t = item->text().trimmed();
            if (!t.isEmpty())
                result += t;
        }
    }
    return result;
}

// ============================================================================
// Delimiters Tab
// ============================================================================

void UserDefineDialog::createDelimitersTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* hint = new QLabel(
        tr("Define character pairs for delimiter highlighting (brace, bracket, "
           "paren, etc.). Each row: Open | Close | Style"),
        page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    _delimiterTable = new QTableWidget(page);
    _delimiterTable->setColumnCount(3);
    _delimiterTable->setHorizontalHeaderLabels(
        {tr("Open"), tr("Close"), tr("Style (Normal/Math)")});
    _delimiterTable->horizontalHeader()->setStretchLastSection(true);
    _delimiterTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _delimiterTable->setMinimumHeight(150);

    // Pre-populate common pairs
    struct Pair { const char* o; const char* c; const char* s; };
    static const Pair defaults[] = {
        {"(", ")", "Normal"},
        {"[", "]", "Normal"},
        {"{", "}", "Normal"},
        {"<", ">", "Math"},
        {"'", "'", "Normal"},
        {"\"", "\"", "Normal"},
    };
    for (const auto& p : defaults) {
        int row = _delimiterTable->rowCount();
        _delimiterTable->insertRow(row);
        _delimiterTable->setItem(row, 0, new QTableWidgetItem(QString(p.o)));
        _delimiterTable->setItem(row, 1, new QTableWidgetItem(QString(p.c)));
        _delimiterTable->setItem(row, 2, new QTableWidgetItem(QString(p.s)));
    }

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton(tr("+ Add"), page);
    QPushButton* remBtn = new QPushButton(tr("Remove"), page);
    btnRow->addWidget(addBtn);
    btnRow->addWidget(remBtn);
    btnRow->addStretch();

    connect(addBtn, &QPushButton::clicked, [this]() {
        int row = _delimiterTable->rowCount();
        _delimiterTable->insertRow(row);
        _delimiterTable->setItem(row, 0, new QTableWidgetItem(QString()));
        _delimiterTable->setItem(row, 1, new QTableWidgetItem(QString()));
        _delimiterTable->setItem(row, 2, new QTableWidgetItem(tr("Normal")));
    });
    connect(remBtn, &QPushButton::clicked, [this]() {
        int r = _delimiterTable->currentRow();
        if (r >= 0) _delimiterTable->removeRow(r);
    });

    QGroupBox* optionsGroup = new QGroupBox(tr("Options"), page);
    QVBoxLayout* optsLayout = new QVBoxLayout(optionsGroup);
    _delimiterForceNested = new QCheckBox(
        tr("Force nested highlighting for delimiter pairs"), optionsGroup);
    _delimiterFarHighlight = new QCheckBox(
        tr("Highlight matching delimiter when cursor is far"), optionsGroup);
    optsLayout->addWidget(_delimiterForceNested);
    optsLayout->addWidget(_delimiterFarHighlight);

    layout->addWidget(_delimiterTable);
    layout->addLayout(btnRow);
    layout->addWidget(optionsGroup);
    layout->addStretch();
    tabs->addTab(page, tr("Delimiters"));
}

QString UserDefineDialog::delimitersFromTable() const {
    QString result;
    for (int r = 0; r < _delimiterTable->rowCount(); ++r) {
        QTableWidgetItem* o = _delimiterTable->item(r, 0);
        QTableWidgetItem* c = _delimiterTable->item(r, 1);
        if (o && c && !o->text().isEmpty() && !c->text().isEmpty()) {
            if (!result.isEmpty()) result += " ";
            result += o->text() + c->text();
        }
    }
    return result;
}

// ============================================================================
// Folding Tab
// ============================================================================

void UserDefineDialog::createFoldingTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    // Fold on keyword
    QGroupBox* kwGroup = new QGroupBox(tr("Fold on Keyword"), page);
    QFormLayout* kwLayout = new QFormLayout(kwGroup);
    _foldOnKeyword = new QCheckBox(tr("Enable keyword-based folding"), kwGroup);
    _foldKeywordEdit = new QLineEdit(kwGroup);
    _foldKeywordEdit->setPlaceholderText(tr("e.g. begin end if endif"));
    kwLayout->addRow(_foldOnKeyword);
    kwLayout->addRow(tr("Keywords:"), _foldKeywordEdit);

    // Fold on comment
    QGroupBox* cmtGroup = new QGroupBox(tr("Fold on Comment"), page);
    QVBoxLayout* cmtLayout = new QVBoxLayout(cmtGroup);
    _foldOnComment = new QCheckBox(
        tr("Enable folding via block comment markers (/* … */)"), cmtGroup);
    cmtLayout->addWidget(_foldOnComment);

    // Fold on preprocessor
    QGroupBox* preGroup = new QGroupBox(tr("Fold on Preprocessor"), page);
    QFormLayout* preLayout = new QFormLayout(preGroup);
    _foldOnPreproc = new QCheckBox(tr("Enable preprocessor folding"), preGroup);
    _preprocEdit = new QLineEdit(preGroup);
    _preprocEdit->setPlaceholderText(tr("e.g. #if #ifdef #else #endif"));
    preLayout->addRow(_foldOnPreproc);
    preLayout->addRow(tr("Preprocessor markers:"), _preprocEdit);

    // Folder open/close markers
    QGroupBox* mrkGroup = new QGroupBox(tr("Folder Markers"), page);
    QFormLayout* mrkLayout = new QFormLayout(mrkGroup);
    _folderOpenEdit = new QLineEdit(mrkGroup);
    _folderOpenEdit->setPlaceholderText(tr("e.g. { { {"));
    _folderCloseEdit = new QLineEdit(mrkGroup);
    _folderCloseEdit->setPlaceholderText(tr("e.g. } } }"));
    mrkLayout->addRow(tr("Open markers:"), _folderOpenEdit);
    mrkLayout->addRow(tr("Close markers:"), _folderCloseEdit);

    layout->addWidget(kwGroup);
    layout->addWidget(cmtGroup);
    layout->addWidget(preGroup);
    layout->addWidget(mrkGroup);
    layout->addStretch();
    tabs->addTab(page, tr("Folding"));
}

// ============================================================================
// Load / Save
// ============================================================================

void UserDefineDialog::loadFromManager(UdlManager* manager, const QString& udlName) {
    UdlDefinition* def = manager->getUdl(udlName);
    if (!def) return;

    _nameEdit->setText(def->name);
    _extEdit->setText(def->extensions.join(" "));

    // Keywords
    _keywordTables[0]->setRowCount(0);
    setupKeywordTable(_keywordTables[0], def->keywords.words0);
    setupKeywordTable(_keywordTables[1], def->keywords.words1);
    setupKeywordTable(_keywordTables[2], def->keywords.words2);
    setupKeywordTable(_keywordTables[3], def->keywords.words3);
    setupKeywordTable(_keywordTables[4], def->keywords.words4);
    setupKeywordTable(_keywordTables[5], def->keywords.words5);
    setupKeywordTable(_keywordTables[6], def->keywords.words6);
    setupKeywordTable(_keywordTables[7], def->keywords.words7);
    setupKeywordTable(_keywordTables[8], def->keywords.words8);

    // Comments
    _lineCommentEdit->setText(def->comment.lineComment);
    _blockCommentStartEdit->setText(def->comment.streamCommentStart);
    _blockCommentEndEdit->setText(def->comment.streamCommentEnd);

    // Operators
    _operatorTable->setRowCount(0);
    if (!def->operators.isEmpty()) {
        for (QChar ch : def->operators) {
            int row = _operatorTable->rowCount();
            _operatorTable->insertRow(row);
            _operatorTable->setItem(row, 0, new QTableWidgetItem(QString(ch)));
        }
    }

    // Folding
    if (!def->folder.folderOpen.isEmpty())
        _folderOpenEdit->setText(def->folder.folderOpen);
    if (!def->folder.folderClose.isEmpty())
        _folderCloseEdit->setText(def->folder.folderClose);
}

void UserDefineDialog::saveToManager(UdlManager* manager, const QString& udlName) {
    UdlDefinition* def = manager->getUdl(udlName);
    if (!def) {
        // Create a new one
        UdlDefinition newDef;
        newDef.name = _nameEdit->text();
        // We'll need a way to add to manager; for now just update if exists
        def = manager->getUdl(udlName);
        if (!def) return;
    }

    def->name = _nameEdit->text().trimmed();
    def->extensions = _extEdit->text().split(' ', Qt::SkipEmptyParts);

    // Keywords
    def->keywords.words0 = keywordsFromTable(_keywordTables[0]);
    def->keywords.words1 = keywordsFromTable(_keywordTables[1]);
    def->keywords.words2 = keywordsFromTable(_keywordTables[2]);
    def->keywords.words3 = keywordsFromTable(_keywordTables[3]);
    def->keywords.words4 = keywordsFromTable(_keywordTables[4]);
    def->keywords.words5 = keywordsFromTable(_keywordTables[5]);
    def->keywords.words6 = keywordsFromTable(_keywordTables[6]);
    def->keywords.words7 = keywordsFromTable(_keywordTables[7]);
    def->keywords.words8 = keywordsFromTable(_keywordTables[8]);

    // Comments
    def->comment.lineComment = _lineCommentEdit->text();
    def->comment.streamCommentStart = _blockCommentStartEdit->text();
    def->comment.streamCommentEnd = _blockCommentEndEdit->text();

    // Operators
    def->operators = operatorsFromTable();

    // Folding
    def->folder.folderOpen = _folderOpenEdit->text();
    def->folder.folderClose = _folderCloseEdit->text();
}

void UserDefineDialog::createNew(const QString& name) {
    _nameEdit->setText(name);
    _extEdit->clear();

    // Clear all keyword tables
    for (int i = 0; i < 9; ++i) {
        _keywordTables[i]->setRowCount(0);
        int row = _keywordTables[i]->rowCount();
        _keywordTables[i]->insertRow(row);
        _keywordTables[i]->setItem(row, 0, new QTableWidgetItem(QString()));
    }

    _lineCommentEdit->clear();
    _blockCommentStartEdit->clear();
    _blockCommentEndEdit->clear();

    // Reset operators to defaults
    _operatorTable->setRowCount(0);
    const QString defaultOps = "+-*/%=<>!&|^~?:,;().[]{}@#$`\\";
    for (QChar ch : defaultOps) {
        int row = _operatorTable->rowCount();
        _operatorTable->insertRow(row);
        _operatorTable->setItem(row, 0, new QTableWidgetItem(QString(ch)));
    }
}

// ============================================================================
// Style color helpers
// ============================================================================

QString UserDefineDialog::getColorString(const QColor& c) const {
    return QString("#%1%2%3")
        .arg(c.red(), 2, 16, QChar('0'))
        .arg(c.green(), 2, 16, QChar('0'))
        .arg(c.blue(), 2, 16, QChar('0'));
}
