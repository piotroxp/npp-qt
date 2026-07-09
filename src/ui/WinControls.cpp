// WinControls.cpp — Qt6 implementations of Win32 common controls
// Copyright (C) 2026 Agent Army | GPL v3

#include "WinControls.h"
#include <QListWidgetItem>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QPainter>
#include <QDebug>

// ============================================================================
// ClipboardHistoryPanel
// ============================================================================

ClipboardHistoryPanel::ClipboardHistoryPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    m_list = new QListWidget(this);
    connect(m_list, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*) { emit pasteRequested(m_list->currentRow()); });
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &ClipboardHistoryPanel::onClipboardChanged, Qt::UniqueConnection);
    lay->addWidget(m_list);
}

void ClipboardHistoryPanel::addEntry(const QString& text) {
    if (m_list->count() >= MaxEntries) delete m_list->takeItem(0);
    auto* item = new QListWidgetItem(text.length() > 100 ? text.left(100) + "..." : text, m_list);
    item->setData(Qt::UserRole, text);
}

void ClipboardHistoryPanel::clear() { m_list->clear(); }

void ClipboardHistoryPanel::onClipboardChanged() {
    QString text = QApplication::clipboard()->text();
    if (!text.isEmpty()) addEntry(text);
}

// ============================================================================
// ColourPicker
// ============================================================================

ColourPicker::ColourPicker(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QGridLayout(this);
    lay->setSpacing(2);
    m_preview = new QLabel(this);
    m_preview->setFixedHeight(20);
    m_preview->setStyleSheet("background: white; border: 1px solid gray;");
    lay->addWidget(m_preview, 0, 0, 1, 8);

    setupPalette();
    for (int i = 0; i < m_palette.size(); ++i) {
        int row = 1 + i / 8;
        int col = i % 8;
        if (row > 3) break;
        auto* btn = new QPushButton(this);
        btn->setFixedSize(24, 24);
        btn->setStyleSheet("border: 1px solid gray; background: " + m_palette[i].name() + ";");
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            m_current = m_palette[i];
            m_preview->setStyleSheet("background: " + m_current.name() + "; border: 1px solid gray;");
            emit colorSelected(m_current);
        });
        lay->addWidget(btn, row, col);
    }

    auto* customBtn = new QPushButton("Custom...", this);
    connect(customBtn, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(m_current, this);
        if (c.isValid()) { m_current = c; emit colorSelected(c); }
    });
    lay->addWidget(customBtn, 4, 0, 1, 8);
}

void ColourPicker::setupPalette() {
    m_palette = {
        Qt::black, Qt::darkGray, Qt::gray, Qt::lightGray,
        Qt::white, Qt::red, Qt::darkRed, Qt::green,
        Qt::darkGreen, Qt::blue, Qt::darkBlue, Qt::cyan,
        Qt::darkCyan, Qt::magenta, Qt::darkMagenta, Qt::yellow,
        Qt::darkYellow, QColor("#800000"), QColor("#008000"), QColor("#000080"),
        QColor("#808000"), QColor("#800080"), QColor("#008080"), QColor("#c0c0c0"),
        QColor("#808080"), QColor("#9999ff"), QColor("#993366"),
        QColor("#ffffcc"), QColor("#ccffff"), QColor("#ffffff"), QColor("#000000"),
    };
}

void ColourPicker::setCurrentColor(const QColor& c) {
    m_current = c;
    m_preview->setStyleSheet("background: " + c.name() + "; border: 1px solid gray;");
}

// ============================================================================
// WordStyleDlg
// ============================================================================

WordStyleDlg::WordStyleDlg(QWidget* parent) : QWidget(parent) { setupUi(); }

void WordStyleDlg::setupUi() {
    auto* lay = new QGridLayout(this);
    int r = 0;
    lay->addWidget(new QLabel("Language:", this), r, 0);
    m_langCombo = new QComboBox(this);
    lay->addWidget(m_langCombo, r++, 1, 1, 3);
    lay->addWidget(new QLabel("Style:", this), r, 0);
    m_styleCombo = new QComboBox(this);
    lay->addWidget(m_styleCombo, r++, 1, 1, 3);
    lay->addWidget(new QLabel("Font:", this), r, 0);
    m_fontCombo = new QFontComboBox(this);
    lay->addWidget(m_fontCombo, r++, 1, 1, 3);
    lay->addWidget(new QLabel("Size:", this), r, 0);
    m_sizeSpin = new QSpinBox(this);
    m_sizeSpin->setRange(6, 72);
    m_sizeSpin->setValue(10);
    lay->addWidget(m_sizeSpin, r++, 1);
    m_bold = new QCheckBox("Bold", this);
    m_italic = new QCheckBox("Italic", this);
    m_underline = new QCheckBox("Underline", this);
    lay->addWidget(m_bold, r, 0);
    lay->addWidget(m_italic, r, 1);
    lay->addWidget(m_underline, r++, 2);
    lay->addWidget(new QLabel("FG:", this), r, 0);
    m_fgPicker = new ColourPicker(this);
    lay->addWidget(m_fgPicker, r++, 1, 1, 3);
    lay->addWidget(new QLabel("BG:", this), r, 0);
    m_bgPicker = new ColourPicker(this);
    lay->addWidget(m_bgPicker, r++, 1, 1, 3);
}

// ============================================================================
// ToolTipButton
// ============================================================================

ToolTipButton::ToolTipButton(QWidget* parent) : QToolButton(parent) {}

bool ToolTipButton::event(QEvent* e) {
    if (e->type() == QEvent::ToolTip && !m_tip2.isEmpty()) {
        QString tip = toolTip();
        if (!tip.isEmpty()) tip += "\n";
        tip += m_tip2;
        QToolTip::showText(QCursor::pos(), tip, this);
        return true;
    }
    return QToolButton::event(e);
}

// ============================================================================
// NativeListBox
// ============================================================================

NativeListBox::NativeListBox(QWidget* parent) : QListWidget(parent) {}

int NativeListBox::addString(const QString& text, int data) {
    auto* item = new QListWidgetItem(text, this);
    item->setData(Qt::UserRole, data);
    return count() - 1;
}

QString NativeListBox::getText(int i) const {
    if (i < 0 || i >= count()) return {};
    return item(i)->text();
}
