#include "IncrementalSearchDialog.h"
#include "../editor/ScintillaEditor.h"
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
    prevBtn->setToolTip("Previous match (Shift+Enter)");
    QPushButton* nextBtn = new QPushButton("↓", this);
    nextBtn->setToolTip("Next match (Enter)");
    QPushButton* closeBtn = new QPushButton("×", this);
    closeBtn->setToolTip("Close (Escape)");

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

    // Style
    setStyleSheet(R"(
        QDialog {
            background: #2d2d2d;
            border: 1px solid #555;
            border-radius: 3px;
        }
        QLineEdit {
            background: #1e1e1e;
            color: #d4d4d4;
            border: 1px solid #3c3c3c;
            border-radius: 2px;
            padding: 2px 6px;
            min-width: 200px;
        }
        QLineEdit:focus {
            border: 1px solid #007acc;
        }
        QLabel {
            color: #808080;
            padding: 0 4px;
            min-width: 40px;
        }
        QPushButton {
            background: #3c3c3c;
            color: #d4d4d4;
            border: none;
            border-radius: 2px;
            padding: 2px 6px;
            min-width: 20px;
        }
        QPushButton:hover {
            background: #505050;
        }
        QPushButton:pressed {
            background: #007acc;
        }
    )");
}

bool IncrementalSearchDialog::eventFilter(QObject* obj, QEvent* event) {
    if (obj == _searchEdit && event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            hide();
            clearHighlight();
            emit closeRequested();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void IncrementalSearchDialog::setEditor(ScintillaEditor* editor) {
    _editor = editor;
}

void IncrementalSearchDialog::clearHighlight() {
    if (!_editor) return;
    // Clear any previous incremental search highlights (indicator 0)
    _editor->send(QsciScintillaBase::SCI_SETSEARCHFLAGS, 0);
    _editor->send(QsciScintillaBase::SCI_ANNOTATION_CLEARALL);
    // Clear indicator 0 highlights across the whole document
    int lines = _editor->lineCount();
    _editor->clearIndicatorRange(0, 0, lines, 0, 0);
    _editor->send(QsciScintillaBase::SCI_ANCHOR);
}

void IncrementalSearchDialog::onTextChanged(const QString& text) {
    if (text.isEmpty()) {
        _countLabel->setText("");
        clearHighlight();
        _lastText.clear();
        _totalMatches = 0;
        _currentMatch = 0;
        return;
    }

    // Only search when text actually changes (not on every keystroke during typing)
    if (text != _lastText) {
        _lastText = text;
        _currentMatch = 0;
        updateMatchCount(text);
        performSearch(text, true);
    }
}

void IncrementalSearchDialog::onNext() {
    QString text = _searchEdit->text();
    if (text.isEmpty()) return;
    performSearch(text, true);
}

void IncrementalSearchDialog::onPrev() {
    QString text = _searchEdit->text();
    if (text.isEmpty()) return;
    performSearch(text, false);
}

void IncrementalSearchDialog::updateMatchCount(const QString& text) {
    if (!_editor) {
        _countLabel->setText("");
        return;
    }

    FindOption opts = FindOption::WrapAround;
    _totalMatches = _editor->countMatches(text, opts);
    if (_totalMatches > 0) {
        _countLabel->setText(QString("1/%1").arg(_totalMatches));
    } else {
        _countLabel->setText("No matches");
    }
}

void IncrementalSearchDialog::performSearch(const QString& text, bool forward) {
    if (!_editor) return;

    FindOption opts = FindOption::WrapAround;

    if (forward) {
        _editor->findNext(text, opts, true);
    } else {
        _editor->findPrevious(text, opts);
    }

    // Update the match counter
    if (_totalMatches > 0) {
        if (forward) {
            _currentMatch = (_currentMatch % _totalMatches) + 1;
        } else {
            _currentMatch = (_currentMatch - 1);
            if (_currentMatch < 1) _currentMatch = _totalMatches;
        }
        _countLabel->setText(QString("%1/%2").arg(_currentMatch).arg(_totalMatches));
    }
}

void IncrementalSearchDialog::setSearchText(const QString& text) {
    _searchEdit->setText(text);
    _searchEdit->selectAll();
    if (!text.isEmpty()) {
        _lastText = text;
        _currentMatch = 0;
        updateMatchCount(text);
        performSearch(text, true);
    }
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
