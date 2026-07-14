// AnsiCharPanel.cpp - ASCII/ANSI character insertion panel
// Copyright (C) 2026 Agent Army
// GPL v3

#include "AnsiCharPanel.h"
#include "ScintillaEditor.h"
#include "Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QToolTip>
#include <QFontDatabase>

// ============================================================================
// Code page map — combo index → Windows code page number
// ============================================================================
const QMap<int, int> AnsiCharPanel::codePageMap = {
    {0,  437},   // OEM (DOS US)
    {1,  850},   // DOS Multilingual (Latin I)
    {2,  1252},  // Windows Western European
    {3,  1250},  // Windows Central European
    {4,  1251},  // Windows Cyrillic
    {5,  1253},  // Windows Greek
    {6,  1254},  // Windows Turkish
    {7,  1255},  // Windows Hebrew
    {8,  1256},  // Windows Arabic
    {9,  1257},  // Windows Baltic
    {10, 1258},  // Windows Vietnamese
    {11, 874},   // Thai (TIS-620)
    {12, 932},   // Japanese (Shift-JIS)
    {13, 936},   // Simplified Chinese (GBK)
    {14, 949},   // Korean
    {15, 950},   // Traditional Chinese (Big5)
};

// ============================================================================
// Construction
// ============================================================================

AnsiCharPanel::AnsiCharPanel(QWidget* parent)
    : QDockWidget(tr("Character Panel"), parent)
{
    setObjectName("AnsiCharPanel");
    setupUi();
}

AnsiCharPanel::~AnsiCharPanel() = default;

// ============================================================================
// setupUi
// ============================================================================

void AnsiCharPanel::setupUi() {
    QWidget* w = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(w);

    // Code page selector
    QHBoxLayout* cpRow = new QHBoxLayout();
    _cpLabel = new QLabel(tr("Code Page:"), w);
    _cpCombo = new QComboBox(w);
    _cpCombo->addItem("437 - OEM (DOS US)", 437);
    _cpCombo->addItem("850 - DOS Multilingual", 850);
    _cpCombo->addItem("1252 - Windows Western", 1252);
    _cpCombo->addItem("1250 - Windows Central European", 1250);
    _cpCombo->addItem("1251 - Windows Cyrillic", 1251);
    _cpCombo->addItem("1253 - Windows Greek", 1253);
    _cpCombo->addItem("1254 - Windows Turkish", 1254);
    _cpCombo->addItem("1255 - Windows Hebrew", 1255);
    _cpCombo->addItem("1256 - Windows Arabic", 1256);
    _cpCombo->addItem("1257 - Windows Baltic", 1257);
    _cpCombo->addItem("874 - Thai (TIS-620)", 874);
    _cpCombo->addItem("932 - Japanese (Shift-JIS)", 932);
    _cpCombo->addItem("936 - Chinese (GBK)", 936);
    _cpCombo->addItem("949 - Korean", 949);
    _cpCombo->addItem("950 - Traditional Chinese (Big5)", 950);
    _cpCombo->setCurrentIndex(2);  // default 1252
    connect(_cpCombo, &QComboBox::currentIndexChanged,
            this, &AnsiCharPanel::onCodePageChanged);

    cpRow->addWidget(_cpLabel);
    cpRow->addWidget(_cpCombo, 1);
    layout->addLayout(cpRow);

    // 16x16 grid
    _grid = new QTableWidget(16, 16, w);
    _grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _grid->setSelectionMode(QAbstractItemView::SingleSelection);
    _grid->setSelectionBehavior(QAbstractItemView::SelectItems);
    _grid->setShowGrid(true);

    // Monospace font for the grid
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    _grid->setFont(monoFont);

    // Fix column/row widths
    _grid->verticalHeader()->setVisible(true);
    _grid->horizontalHeader()->setVisible(true);
    for (int c = 0; c < 16; ++c)
        _grid->setColumnWidth(c, 28);
    for (int r = 0; r < 16; ++r)
        _grid->setRowHeight(r, 24);

    connect(_grid, &QTableWidget::cellClicked,
            this, &AnsiCharPanel::onCellClicked);
    connect(_grid, &QTableWidget::cellDoubleClicked,
            this, &AnsiCharPanel::onCharDoubleClicked);

    layout->addWidget(_grid);
    w->setLayout(layout);
    setWidget(w);

    // Build the initial grid
    buildGrid();
}

// ============================================================================
// Grid building
// ============================================================================

void AnsiCharPanel::buildGrid() {
    _grid->blockSignals(true);
    _grid->clear();

    // Set headers
    QStringList hex = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
    _grid->setHorizontalHeaderLabels(hex);
    _grid->setVerticalHeaderLabels(hex);

    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            int code = row * 16 + col;
            QString label = charLabel(code);
            QTableWidgetItem* item = new QTableWidgetItem(label);
            item->setTextAlignment(Qt::AlignCenter);
            item->setToolTip(charTooltip(code));
            item->setBackground(cellColor(code));
            _grid->setItem(row, col, item);
        }
    }

    _grid->blockSignals(false);
}

QString AnsiCharPanel::charLabel(int code) const {
    if (code < 32) {
        // Control chars — show abbreviated name
        static const QString names[32] = {
            "NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL",
            "BS","TAB","LF","VT","FF","CR","SO","SI",
            "DLE","DC1","DC2","DC3","DC4","NAK","SYN","ETB",
            "CAN","EM","SUB","ESC","FS","GS","RS","US"
        };
        return names[code];
    } else if (code == 127) {
        return "DEL";
    }
    // Try to display using current code page
    QChar ch(code);
    if (ch.isPrint())
        return QString(ch);
    // Extended — show hex
    return QString("%1").arg(code, 2, 16, QChar('0')).toUpper();
}

QString AnsiCharPanel::charTooltip(int code) const {
    return QString("U+%1  Dec: %2  Hex: 0x%3")
        .arg(code, 4, 16, QChar('0')).arg(code)
        .arg(code, 2, 16, QChar('0')).toUpper();
}

QColor AnsiCharPanel::cellColor(int code) const {
    if (isControlChar(code))
        return QColor(0xCC, 0xCC, 0xCC);  // gray
    if (isExtendedChar(code))
        return QColor(0xFF, 0xFF, 0x99);  // yellow
    if (isPrintable(code))
        return QColor(0xFF, 0xFF, 0xFF);  // white
    return QColor(0xEE, 0xEE, 0xEE);  // light gray
}

bool AnsiCharPanel::isControlChar(int code) const {
    return code < 32 || code == 127;
}

bool AnsiCharPanel::isPrintable(int code) const {
    return code >= 32 && code < 127;
}

bool AnsiCharPanel::isExtendedChar(int code) const {
    return code >= 128;
}

// ============================================================================
// Events
// ============================================================================

void AnsiCharPanel::onCellClicked(int row, int col) {
    int code = row * 16 + col;
    QTableWidgetItem* item = _grid->item(row, col);
    if (!item) return;

    // Show hex representation in tooltip
    QString tip = QString("U+%1  Click again to insert")
        .arg(code, 4, 16, QChar('0')).toUpper();
    QToolTip::showText(QCursor::pos(), tip, _grid);
}

void AnsiCharPanel::onCharDoubleClicked(int row, int col) {
    int code = row * 16 + col;
    onCharClicked(code);
}

void AnsiCharPanel::onCharClicked(int code) {
    // Insert the character into the current editor
    ScintillaEditor* editor = nullptr;
    Application& app = Application::instance();
    editor = app.getActiveEditor();

    if (editor) {
        // Insert character at cursor position
        QChar ch(code);
        editor->qsciEditor()->insert(ch);
        editor->setFocus();
    } else {
        // Fallback: copy to clipboard
        QChar ch(code);
        QApplication::clipboard()->setText(QString(ch));
    }
}

void AnsiCharPanel::onCodePageChanged(int index) {
    QVariant data = _cpCombo->itemData(index);
    if (data.isValid()) {
        _currentCp = data.toInt();
        buildGrid();
    }
}

void AnsiCharPanel::refreshPanel() {
    buildGrid();
}

void AnsiCharPanel::setCodePage(int cp) {
    int idx = _cpCombo->findData(cp);
    if (idx >= 0) {
        _cpCombo->blockSignals(true);
        _cpCombo->setCurrentIndex(idx);
        _cpCombo->blockSignals(false);
        _currentCp = cp;
        buildGrid();
    }
}
