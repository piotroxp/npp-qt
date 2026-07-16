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
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

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
    createStringsTab(tabs);
    createPreviewTab(tabs);
    mainLayout->addWidget(tabs);

    // --- Buttons ---
    QHBoxLayout* btnLayout = new QHBoxLayout();

    QPushButton* importBtn = new QPushButton(tr("Import…"), this);
    QPushButton* exportBtn = new QPushButton(tr("Export…"), this);
    QPushButton* validateBtn = new QPushButton(tr("Validate"), this);
    QPushButton* resetBtn = new QPushButton(tr("Reset"), this);
    btnLayout->addWidget(importBtn);
    btnLayout->addWidget(exportBtn);
    btnLayout->addWidget(validateBtn);
    btnLayout->addWidget(resetBtn);
    btnLayout->addStretch();

    QPushButton* okBtn = new QPushButton(tr("OK"), this);
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(importBtn, &QPushButton::clicked, this, &UserDefineDialog::onImportClicked);
    connect(exportBtn, &QPushButton::clicked, this, &UserDefineDialog::onExportClicked);
    connect(validateBtn, &QPushButton::clicked, this, &UserDefineDialog::onValidateClicked);
    connect(resetBtn, &QPushButton::clicked, this, &UserDefineDialog::onResetClicked);
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

        // Refresh preview when keywords change
        connect(table, &QTableWidget::itemChanged, this, &UserDefineDialog::onKeywordTableChanged);
        connect(table->model(), &QAbstractItemModel::rowsRemoved, this, &UserDefineDialog::onKeywordTableChanged);

        hbox->addWidget(table, 1);
        QVBoxLayout* btnBox = new QVBoxLayout();
        btnBox->addWidget(addBtn);
        btnBox->addWidget(remBtn);
        {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        btnBox->insertItem(btnBox->count(), _spacer);
    }
        hbox->addLayout(btnBox);

        layout->addWidget(group);
        _keywordTables[i] = table;
    }

    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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

    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        fgRow->addItem(_spacer);
    }
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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        bgRow->addItem(_spacer);
    }
    styleLayout->addRow(tr("Background:"), bgRow);

    // Bold / Italic
    QHBoxLayout* styleRow = new QHBoxLayout();
    _numberBold = new QCheckBox(tr("Bold"), styleGroup);
    _numberItalic = new QCheckBox(tr("Italic"), styleGroup);
    styleRow->addWidget(_numberBold);
    styleRow->addWidget(_numberItalic);
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        styleRow->addItem(_spacer);
    }
    styleLayout->addRow(tr("Style:"), styleRow);

    layout->addWidget(styleGroup);
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        btnRow->addItem(_spacer);
    }

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

    // Refresh preview when operator list changes
    connect(_operatorTable, &QTableWidget::itemChanged, this, &UserDefineDialog::onKeywordTableChanged);
    connect(_operatorTable->model(), &QAbstractItemModel::rowsRemoved, this, &UserDefineDialog::onKeywordTableChanged);
    connect(_operatorTable->model(), &QAbstractItemModel::rowsInserted, this, &UserDefineDialog::onKeywordTableChanged);

    layout->addWidget(_operatorTable);
    layout->addLayout(btnRow);
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        btnRow->addItem(_spacer);
    }

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

    // Refresh preview when delimiters change
    connect(_delimiterTable, &QTableWidget::itemChanged, this, &UserDefineDialog::onKeywordTableChanged);
    connect(_delimiterTable->model(), &QAbstractItemModel::rowsRemoved, this, &UserDefineDialog::onKeywordTableChanged);
    connect(_delimiterTable->model(), &QAbstractItemModel::rowsInserted, this, &UserDefineDialog::onKeywordTableChanged);

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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
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

    // Delimiters — stored as space-separated pairs in stringChars
    _delimiterTable->setRowCount(0);
    if (!def->stringChars.isEmpty()) {
        QStringList pairs = def->stringChars.split(' ', Qt::SkipEmptyParts);
        for (const QString& pair : pairs) {
            if (pair.size() >= 2) {
                int row = _delimiterTable->rowCount();
                _delimiterTable->insertRow(row);
                _delimiterTable->setItem(row, 0, new QTableWidgetItem(pair.left(1)));
                _delimiterTable->setItem(row, 1, new QTableWidgetItem(pair.mid(1, 1)));
                _delimiterTable->setItem(row, 2, new QTableWidgetItem(tr("Normal")));
            }
        }
    }

    // Strings tab
    if (_stringOpenEdit) _stringOpenEdit->clear();
    if (_stringCloseEdit) _stringCloseEdit->clear();
    if (_escapeCharCheck) _escapeCharCheck->setChecked(false);
    if (_escapeCharEdit) _escapeCharEdit->clear();
    if (_stringWordCharCheck) _stringWordCharCheck->setChecked(false);
    if (_stringWordCharEdit) _stringWordCharEdit->clear();

    // Folding
    if (!def->folder.folderOpen.isEmpty())
        _folderOpenEdit->setText(def->folder.folderOpen);
    if (!def->folder.folderClose.isEmpty())
        _folderCloseEdit->setText(def->folder.folderClose);
}

void UserDefineDialog::saveToManager(UdlManager* manager, const QString& udlName) {
    // Always write to a local copy so we can add a new UDL entry via setUdl
    UdlDefinition def;

    // Load existing data if the UDL already exists
    UdlDefinition* existing = manager->getUdl(udlName);
    if (existing)
        def = *existing;

    def.name = _nameEdit->text().trimmed();
    def.extensions = _extEdit->text().split(' ', Qt::SkipEmptyParts);

    // Keywords
    def.keywords.words0 = keywordsFromTable(_keywordTables[0]);
    def.keywords.words1 = keywordsFromTable(_keywordTables[1]);
    def.keywords.words2 = keywordsFromTable(_keywordTables[2]);
    def.keywords.words3 = keywordsFromTable(_keywordTables[3]);
    def.keywords.words4 = keywordsFromTable(_keywordTables[4]);
    def.keywords.words5 = keywordsFromTable(_keywordTables[5]);
    def.keywords.words6 = keywordsFromTable(_keywordTables[6]);
    def.keywords.words7 = keywordsFromTable(_keywordTables[7]);
    def.keywords.words8 = keywordsFromTable(_keywordTables[8]);

    // Comments
    def.comment.lineComment = _lineCommentEdit->text();
    def.comment.streamCommentStart = _blockCommentStartEdit->text();
    def.comment.streamCommentEnd = _blockCommentEndEdit->text();

    // Operators
    def.operators = operatorsFromTable();

    // Delimiters
    def.stringChars = delimitersFromTable();

    // Folding
    def.folder.folderOpen = _folderOpenEdit->text();
    def.folder.folderClose = _folderCloseEdit->text();

    // Persist via manager
    manager->setUdl(udlName, def);
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

// ============================================================================
// Validation
// ============================================================================

QStringList UserDefineDialog::validateUdl() const {
    QStringList errors;

    if (_nameEdit->text().trimmed().isEmpty())
        errors << tr("Language name is empty.");

    // Collect all non-empty keywords across groups
    QStringList allKeywords;
    for (int i = 0; i < 9; ++i) {
        if (_keywordTables[i]) {
            QString kws = keywordsFromTable(_keywordTables[i]);
            QStringList lines = kws.split('\n', Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                QStringList words = line.trimmed().split(' ', Qt::SkipEmptyParts);
                for (const QString& kw : words) {
                    if (allKeywords.contains(kw))
                        errors << tr("Duplicate keyword '%1' found in group %2.").arg(kw).arg(i);
                    allKeywords.append(kw);
                }
            }
        }
    }

    // Block comment requires both start and end
    if (!_blockCommentStartEdit->text().isEmpty() && _blockCommentEndEdit->text().isEmpty())
        errors << tr("Block comment start is set but block comment end is empty.");

    if (_blockCommentStartEdit->text().isEmpty() && !_blockCommentEndEdit->text().isEmpty())
        errors << tr("Block comment end is set but block comment start is empty.");

    return errors;
}

// ============================================================================
// Import / Export
// ============================================================================

bool UserDefineDialog::importFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Import Failed"),
            tr("Could not open file:\n%1").arg(path));
        return false;
    }

    QXmlStreamReader xml(&file);

    // Read to the first UserLang element
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QLatin1String("UserLang"))
            break;
    }

    if (xml.atEnd() || xml.name() != QLatin1String("UserLang")) {
        QMessageBox::warning(this, tr("Import Failed"),
            tr("No <UserLang> element found in:\n%1").arg(path));
        return false;
    }

    // Parse attributes: name and ext
    QString name = xml.attributes().value(QLatin1String("name")).toString();
    QString extStr = xml.attributes().value(QLatin1String("ext")).toString();

    if (!name.isEmpty())
        _nameEdit->setText(name);
    if (!extStr.isEmpty())
        _extEdit->setText(extStr);

    // Parse child elements
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isEndElement() && xml.name() == QLatin1String("UserLang"))
            break;
        if (!xml.isStartElement())
            continue;

        if (xml.name() == QLatin1String("Keywords")) {
            QString attrName = xml.attributes().value(QLatin1String("name")).toString();
            QString keywords = xml.readElementText().trimmed();

            if (attrName == QLatin1String("Keywords"))
                setupKeywordTable(_keywordTables[0], keywords);
            else if (attrName == QLatin1String("Keywords2"))
                setupKeywordTable(_keywordTables[1], keywords);
            else if (attrName == QLatin1String("Keywords3"))
                setupKeywordTable(_keywordTables[2], keywords);
            else if (attrName == QLatin1String("Keywords4"))
                setupKeywordTable(_keywordTables[3], keywords);
            else if (attrName == QLatin1String("Keywords5"))
                setupKeywordTable(_keywordTables[4], keywords);
            else if (attrName == QLatin1String("Keywords6"))
                setupKeywordTable(_keywordTables[5], keywords);
            else if (attrName == QLatin1String("Keywords7"))
                setupKeywordTable(_keywordTables[6], keywords);
            else if (attrName == QLatin1String("Keywords8"))
                setupKeywordTable(_keywordTables[7], keywords);
        }
        else if (xml.name() == QLatin1String("Comment")) {
            _lineCommentEdit->setText(
                xml.attributes().value(QLatin1String("lineComment")).toString());
            _blockCommentStartEdit->setText(
                xml.attributes().value(QLatin1String("streamCommentStart")).toString());
            _blockCommentEndEdit->setText(
                xml.attributes().value(QLatin1String("streamCommentEnd")).toString());
        }
        else if (xml.name() == QLatin1String("Operators")) {
            QString ops = xml.readElementText().trimmed();
            _operatorTable->setRowCount(0);
            for (QChar ch : ops) {
                int row = _operatorTable->rowCount();
                _operatorTable->insertRow(row);
                _operatorTable->setItem(row, 0, new QTableWidgetItem(QString(ch)));
            }
        }
        else if (xml.name() == QLatin1String("Folder")) {
            _folderOpenEdit->setText(
                xml.attributes().value(QLatin1String("folderStart")).toString());
            _folderCloseEdit->setText(
                xml.attributes().value(QLatin1String("folderEnd")).toString());
        }
    }

    return true;
}

bool UserDefineDialog::exportToFile(const QString& path) const {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(const_cast<UserDefineDialog*>(this), tr("Export Failed"),
            tr("Could not write to:\n%1").arg(path));
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QLatin1String("NotepadPlus"));
    xml.writeStartElement(QLatin1String("UserLang"));
    xml.writeAttribute(QLatin1String("name"), _nameEdit->text().trimmed());
    xml.writeAttribute(QLatin1String("ext"), _extEdit->text().trimmed());

    // Keywords
    QStringList kwNames = {
        QLatin1String("Keywords"), QLatin1String("Keywords2"),
        QLatin1String("Keywords3"), QLatin1String("Keywords4"),
        QLatin1String("Keywords5"), QLatin1String("Keywords6"),
        QLatin1String("Keywords7"), QLatin1String("Keywords8")
    };
    for (int i = 0; i < 8; ++i) {
        xml.writeStartElement(QLatin1String("Keywords"));
        xml.writeAttribute(QLatin1String("name"), kwNames[i]);
        xml.writeCharacters(keywordsFromTable(_keywordTables[i]));
        xml.writeEndElement(); // Keywords
    }

    // Keywords9 — stored in _keywordTables[8]
    xml.writeStartElement(QLatin1String("Keywords"));
    xml.writeAttribute(QLatin1String("name"), QLatin1String("Keywords9"));
    xml.writeCharacters(keywordsFromTable(_keywordTables[8]));
    xml.writeEndElement(); // Keywords9

    // Comment
    xml.writeStartElement(QLatin1String("Comment"));
    if (!_lineCommentEdit->text().isEmpty())
        xml.writeAttribute(QLatin1String("lineComment"), _lineCommentEdit->text());
    if (!_blockCommentStartEdit->text().isEmpty())
        xml.writeAttribute(QLatin1String("streamCommentStart"), _blockCommentStartEdit->text());
    if (!_blockCommentEndEdit->text().isEmpty())
        xml.writeAttribute(QLatin1String("streamCommentEnd"), _blockCommentEndEdit->text());
    xml.writeEndElement(); // Comment

    // Operators
    xml.writeStartElement(QLatin1String("Operators"));
    xml.writeCharacters(operatorsFromTable());
    xml.writeEndElement(); // Operators

    // Folder
    xml.writeStartElement(QLatin1String("Folder"));
    if (!_folderOpenEdit->text().isEmpty())
        xml.writeAttribute(QLatin1String("folderStart"), _folderOpenEdit->text());
    if (!_folderCloseEdit->text().isEmpty())
        xml.writeAttribute(QLatin1String("folderEnd"), _folderCloseEdit->text());
    xml.writeEndElement(); // Folder

    xml.writeEndElement(); // UserLang
    xml.writeEndElement(); // NotepadPlus
    xml.writeEndDocument();

    return true;
}

// ============================================================================
// Button handlers
// ============================================================================
// Strings Tab
// ============================================================================

void UserDefineDialog::createStringsTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* hint = new QLabel(
        tr("Define string delimiters for the language. Strings are highlighted "
           "between the open and close characters."),
        page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    // Open/close delimiters
    QGroupBox* delimGroup = new QGroupBox(tr("String Delimiters"), page);
    QFormLayout* delimLayout = new QFormLayout(delimGroup);
    _stringOpenEdit = new QLineEdit(delimGroup);
    _stringOpenEdit->setPlaceholderText(tr("e.g. \" (double quote)"));
    _stringCloseEdit = new QLineEdit(delimGroup);
    _stringCloseEdit->setPlaceholderText(tr("e.g. \" (double quote)"));
    delimLayout->addRow(tr("Open char:"), _stringOpenEdit);
    delimLayout->addRow(tr("Close char:"), _stringCloseEdit);

    // Escape character
    QGroupBox* escapeGroup = new QGroupBox(tr("Escape Character"), page);
    QFormLayout* escapeLayout = new QFormLayout(escapeGroup);
    _escapeCharCheck = new QCheckBox(tr("Enable escape character"), escapeGroup);
    _escapeCharEdit = new QLineEdit(escapeGroup);
    _escapeCharEdit->setPlaceholderText(tr("e.g. \\"));
    _escapeCharEdit->setMaxLength(1);
    escapeLayout->addRow(QString(), _escapeCharCheck);
    escapeLayout->addRow(tr("Escape char:"), _escapeCharEdit);

    // Word characters
    QGroupBox* wordGroup = new QGroupBox(tr("Word Characters"), page);
    QFormLayout* wordLayout = new QFormLayout(wordGroup);
    _stringWordCharCheck = new QCheckBox(
        tr("Treat the following characters as part of a string word"), wordGroup);
    _stringWordCharEdit = new QLineEdit(wordGroup);
    _stringWordCharEdit->setPlaceholderText(tr("e.g. \\"));
    wordLayout->addRow(QString(), _stringWordCharCheck);
    wordLayout->addRow(tr("Extra chars:"), _stringWordCharEdit);

    layout->addWidget(delimGroup);
    layout->addWidget(escapeGroup);
    layout->addWidget(wordGroup);
    {
        auto* _spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(_spacer);
    }
    tabs->addTab(page, tr("Strings"));
}

// ============================================================================
// Preview Tab
// ============================================================================

void UserDefineDialog::createPreviewTab(QTabWidget* tabs) {
    QWidget* page = new QWidget(tabs);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QLabel* hint = new QLabel(
        tr("This preview shows sample text based on your UDL settings. "
           "Keywords, comments, numbers and operators are rendered as they "
           "would appear in the editor. Click 'Test' to apply to an open buffer."),
        page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    _previewEditor = new QPlainTextEdit(page);
    _previewEditor->setMinimumHeight(200);
    _previewEditor->setReadOnly(true);
    _previewEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    layout->addWidget(_previewEditor);

    _previewStatusLabel = new QLabel(page);
    _previewStatusLabel->setText(tr("No language defined yet."));
    layout->addWidget(_previewStatusLabel);

    // A small toolbar to refresh / test
    QHBoxLayout* previewBtnLayout = new QHBoxLayout();
    QPushButton* refreshBtn = new QPushButton(tr("Refresh Preview"), page);
    QPushButton* testBtn = new QPushButton(tr("Test in Editor"), page);
    connect(refreshBtn, &QPushButton::clicked, this, &UserDefineDialog::updatePreview);
    connect(testBtn, &QPushButton::clicked, this, &UserDefineDialog::onTestClicked);
    previewBtnLayout->addWidget(refreshBtn);
    previewBtnLayout->addWidget(testBtn);
    previewBtnLayout->addStretch();
    layout->addLayout(previewBtnLayout);

    tabs->addTab(page, tr("Preview"));
}

// ============================================================================
// Keyword table helpers (stubs resolved)
// ============================================================================

void UserDefineDialog::setupOperatorTable() {
    // Default operators are already set in createOperatorsTab().
    // This method can be used to reset to defaults programmatically.
    _operatorTable->setRowCount(0);
    const QString defaultOps = "+-*/%=<>!&|^~?:,;().[]{}@#$`\\";
    for (QChar ch : defaultOps) {
        int row = _operatorTable->rowCount();
        _operatorTable->insertRow(row);
        _operatorTable->setItem(row, 0, new QTableWidgetItem(QString(ch)));
    }
}

void UserDefineDialog::setupDelimiterTable() {
    // Default delimiters are already set in createDelimitersTab().
    _delimiterTable->setRowCount(0);
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
}

void UserDefineDialog::populateKeywordAutocomplete() {
    // Build a combined list of all keywords for future autocompletion support.
    // The actual QLineEdit autocomplete is optional and can be wired here.
    _allKnownKeywords.clear();
    for (int i = 0; i < 9; ++i) {
        if (!_keywordTables[i])
            continue;
        QString kws = keywordsFromTable(_keywordTables[i]);
        QStringList lines = kws.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList words = line.trimmed().split(' ', Qt::SkipEmptyParts);
            for (const QString& w : words) {
                if (!w.isEmpty() && !_allKnownKeywords.contains(w))
                    _allKnownKeywords.append(w);
            }
        }
    }
}

// ============================================================================
// onKeywordTableChanged — called whenever any keyword/operator/delimiter table
// content changes; refreshes the preview.
// ============================================================================

void UserDefineDialog::onKeywordTableChanged() {
    populateKeywordAutocomplete();
    updatePreview();
}

// ============================================================================

void UserDefineDialog::onValidateClicked() {
    QStringList errors = validateUdl();
    if (errors.isEmpty()) {
        QMessageBox::information(this, tr("Validation"),
            tr("UDL is valid."));
    } else {
        QMessageBox::warning(this, tr("Validation Errors"),
            tr("The following issues were found:\n\n%1").arg(errors.join("\n")));
    }
}

void UserDefineDialog::updatePreview() {
    // Build a simple sample snippet from keywords
    QString sample;

    for (int i = 0; i < 9; ++i) {
        if (!_keywordTables[i])
            continue;
        QString kws = keywordsFromTable(_keywordTables[i]);
        QStringList lines = kws.split('\n', Qt::SkipEmptyParts);
        for (int li = 0; li < lines.size() && li < 2; ++li) {
            QStringList words = lines[li].trimmed().split(' ', Qt::SkipEmptyParts);
            for (int wi = 0; wi < words.size() && wi < 4; ++wi) {
                sample += words[wi] + " ";
            }
            sample += "\n";
        }
    }

    // Add some generic sample content if no keywords defined
    if (sample.trimmed().isEmpty()) {
        sample = tr("// Sample code\n"
                   "// Add keywords in the Keywords tab to see them here\n"
                   "int main() {\n"
                   "    return 0;\n"
                   "}\n");
    }

    // Emit the test signal so the main app can apply the UDL to a buffer
    emit testUdlRequested(currentName());

    if (_previewEditor) {
        _previewEditor->setPlainText(sample);
    }
}

void UserDefineDialog::onImportClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Import UDL"),
        QString(),
        tr("UDL files (*.xml *.udl);;All files (*)"));
    if (path.isEmpty())
        return;

    if (importFromFile(path)) {
        QMessageBox::information(this, tr("Import"),
            tr("Import successful."));
        updatePreview();
    }
}

void UserDefineDialog::onExportClicked() {
    QString defaultName = _nameEdit->text().trimmed().isEmpty()
        ? QStringLiteral("mylang")
        : _nameEdit->text().trimmed();

    QString path = QFileDialog::getSaveFileName(
        this,
        tr("Export UDL"),
        defaultName + ".xml",
        tr("UDL files (*.xml);;All files (*)"));
    if (path.isEmpty())
        return;

    if (exportToFile(path)) {
        QMessageBox::information(this, tr("Export"),
            tr("Export successful."));
    }
}

void UserDefineDialog::onTestClicked() {
    emit testUdlRequested(currentName());
    QMessageBox::information(this, tr("Test Mode"),
        tr("Test mode: Apply your language definition and test it on the sample "
           "text in the preview tab."));
}

void UserDefineDialog::onResetClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Reset to Defaults"),
        tr("Reset all settings to defaults?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        createNew(currentName());
        updatePreview();
    }
}

void UserDefineDialog::onAddKeywordClicked(int group) {
    if (group < 0 || group >= 9)
        return;
    QTableWidget* table = _keywordTables[group];
    if (!table)
        return;

    int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0, new QTableWidgetItem(QString()));
    table->setCurrentCell(row, 0);
    table->editItem(table->item(row, 0));

    updatePreview();
}

