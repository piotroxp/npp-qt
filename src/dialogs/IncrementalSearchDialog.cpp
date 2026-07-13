#include "IncrementalSearchDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QKeyEvent>

IncrementalSearchDialog::IncrementalSearchDialog(QWidget* parent)
    : QDialog(parent, Qt::Tool | Qt::FramelessWindowHint)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    
    _searchEdit = new QLineEdit(this);
    _searchEdit->setPlaceholderText("Search... (Enter: next, Shift+Enter: prev)");
    _countLabel = new QLabel(this);
    
    QPushButton* prevBtn = new QPushButton("↑", this);
    QPushButton* nextBtn = new QPushButton("↓", this);
    QPushButton* closeBtn = new QPushButton("×", this);
    
    layout->addWidget(_searchEdit);
    layout->addWidget(_countLabel);
    layout->addWidget(prevBtn);
    layout->addWidget(nextBtn);
    layout->addWidget(closeBtn);
    
    connect(_searchEdit, &QLineEdit::textChanged, this, &IncrementalSearchDialog::onTextChanged);
    connect(nextBtn, &QPushButton::clicked, this, &IncrementalSearchDialog::onNext);
    connect(prevBtn, &QPushButton::clicked, this, &IncrementalSearchDialog::onPrev);
    connect(closeBtn, &QPushButton::clicked, this, &IncrementalSearchDialog::closeRequested);
    connect(_searchEdit, &QLineEdit::returnPressed, this, [this]() {
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
            onPrev();
        else
            onNext();
    });

    // Handle Escape key to close dialog
    _searchEdit->installEventFilter(this);
}

bool IncrementalSearchDialog::eventFilter(QObject* obj, QEvent* event) {
    if (obj == _searchEdit && event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            hide();
            emit closeRequested();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void IncrementalSearchDialog::onTextChanged(const QString& text) {
    if (text.isEmpty()) {
        _countLabel->setText("");
        return;
    }
    emit searchNext(text);
}

void IncrementalSearchDialog::onNext() {
    emit searchNext(_searchEdit->text());
}

void IncrementalSearchDialog::onPrev() {
    emit searchPrev(_searchEdit->text());
}

void IncrementalSearchDialog::setSearchText(const QString& text) {
    _searchEdit->setText(text);
}

void IncrementalSearchDialog::showAtTop() {
    // Position at top of editor area
    if (auto* win = qobject_cast<QWidget*>(parent())) {
        move(win->x() + 10, win->y() + 10);
    }
    show();
    _searchEdit->setFocus();
    _searchEdit->selectAll();
}
