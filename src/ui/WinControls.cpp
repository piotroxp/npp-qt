// WinControls.cpp — Qt6 implementations of Win32 common controls
// Copyright (C) 2026 Agent Army | GPL v3

#include "WinControls.h"
#include "DpiManager.h"
#include <QListWidgetItem>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QPainter>
#include <QDebug>
#include <QStyle>
#include <QApplication>

// ============================================================================
// ClipboardHistoryPanel
// ============================================================================

ClipboardHistoryPanel::ClipboardHistoryPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(2, 2, 2, 2);
    
    m_list = new QListWidget(this);
    m_list->setAlternatingRowColors(true);
    
    connect(m_list, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*) { emit pasteRequested(m_list->currentRow()); });
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &ClipboardHistoryPanel::onClipboardChanged, Qt::UniqueConnection);
            
    lay->addWidget(m_list);
    applyDpiScaling();
}

void ClipboardHistoryPanel::applyDpiScaling() {
    DpiManager& dpi = DpiManager::instance();
    setMinimumHeight(dpi.scale(150));
}

void ClipboardHistoryPanel::addEntry(const QString& text) {
    if (m_list->count() >= MaxEntries) delete m_list->takeItem(0);
    auto* item = new QListWidgetItem(text.length() > 100 ? text.left(100) + "..." : text, m_list);
    item->setData(Qt::UserRole, text);
    item->setToolTip(text);
}

void ClipboardHistoryPanel::clear() { m_list->clear(); }

void ClipboardHistoryPanel::onClipboardChanged() {
    QString text = QApplication::clipboard()->text();
    if (!text.isEmpty()) addEntry(text);
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
        
        // Scale tooltip position if DPI manager is available
        if (m_dpiManager) {
            QPoint pos = QCursor::pos();
            int scale = m_dpiManager->scale(0);
            if (scale > 1) {
                pos = QPoint(pos.x() * scale, pos.y() * scale);
            }
            QToolTip::showText(pos, tip, this);
        } else {
            QToolTip::showText(QCursor::pos(), tip, this);
        }
        return true;
    }
    return QToolButton::event(e);
}

// ============================================================================
// NativeListBox
// ============================================================================

NativeListBox::NativeListBox(QWidget* parent) : QListWidget(parent) {
    setAlternatingRowColors(true);
}

int NativeListBox::addString(const QString& text, int data) {
    auto* item = new QListWidgetItem(text, this);
    item->setData(Qt::UserRole, data);
    return count() - 1;
}

QString NativeListBox::getText(int i) const {
    if (i < 0 || i >= count()) return {};
    return item(i)->text();
}

// ============================================================================
// EditableLabel
// ============================================================================

EditableLabel::EditableLabel(QWidget* parent) : QWidget(parent) {
    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    
    m_label = new QLabel(this);
    m_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_label->setCursor(Qt::PointingHandCursor);
    lay->addWidget(m_label);
    
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setFrame(false);
    m_lineEdit->hide();
    lay->addWidget(m_lineEdit);
    
    connect(m_label, &QLabel::linkActivated, this, &EditableLabel::startEditing);
    connect(m_label, &QLabel::linkActivated, this, &EditableLabel::clicked);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &EditableLabel::finishEditing);
}

void EditableLabel::setText(const QString& text) {
    m_label->setText(text);
}

void EditableLabel::startEditing() {
    m_label->hide();
    m_lineEdit->setText(m_label->text());
    m_lineEdit->show();
    m_lineEdit->setFocus();
    m_lineEdit->selectAll();
}

void EditableLabel::finishEditing() {
    QString text = m_lineEdit->text();
    m_lineEdit->hide();
    m_label->setText(text);
    m_label->show();
    emit editingFinished(text);
}

// ============================================================================
// ProgressIndicator
// ============================================================================

ProgressIndicator::ProgressIndicator(QWidget* parent) : QWidget(parent) {
    setFixedHeight(16);
    setMinimumWidth(100);
}

void ProgressIndicator::setProgress(int value, int maximum) {
    m_value = qBound(0, value, maximum);
    m_maximum = qMax(1, maximum);
    update();
}

void ProgressIndicator::startAnimation() {
    m_animating = true;
    update();
}

void ProgressIndicator::stopAnimation() {
    m_animating = false;
    update();
}

void ProgressIndicator::setText(const QString& text) {
    m_text = text;
    update();
}

void ProgressIndicator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    
    QStyleOptionProgressBar opt;
    opt.initFrom(this);
    opt.minimum = 0;
    opt.maximum = m_maximum;
    opt.progress = m_value;
    opt.text = m_text;
    opt.textVisible = !m_text.isEmpty();
    
    style()->drawControl(QStyle::CE_ProgressBar, &opt, &p, this);
}
