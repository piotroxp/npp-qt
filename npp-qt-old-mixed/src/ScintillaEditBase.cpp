// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include "ScintillaEditBase.h"
#include "NppSciCompat.h"  // npp_sci namespace (SCI_* constants)
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>

int ScintillaEditBase::_nextId = 0;

ScintillaEditBase::ScintillaEditBase(QWidget* parent)
    : QsciScintilla(parent)
    , _id(++_nextId)
{
    // QsciScintilla sets up the Scintilla editor widget automatically.
    // Basic editor configuration
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
    setEolMode(EolWindows);
    setEolVisibility(false);
}

ScintillaEditBase::~ScintillaEditBase() = default;

void ScintillaEditBase::init()
{
    // Additional per-instance initialization after construction.
    // QsciScintilla is already initialized by its constructor.
}

sptr_t ScintillaEditBase::send(unsigned int msg, uptr_t wParam, sptr_t lParam) const
{
    // QsciScintilla::SendScintilla() is the Qt equivalent of
    // Win32 SendMessage(hwnd, msg, wParam, lParam)
    return SendScintilla(msg, static_cast<unsigned long>(wParam),
                         static_cast<long>(lParam));
}

void ScintillaEditBase::setText(const char* text)
{
    QsciScintilla::setText(QString::fromUtf8(text));
}

void ScintillaEditBase::setText(const QByteArray& text)
{
    QsciScintilla::setText(QString::fromUtf8(text));
}

QString ScintillaEditBase::text() const
{
    return QsciScintilla::text();
}

QString ScintillaEditBase::selectedText() const
{
    // QsciScintilla::selectedText() returns the selected text
    return QsciScintilla::selectedText();
}

int ScintillaEditBase::length() const
{
    return static_cast<int>(send(npp_sci::SCI_GETLENGTH));
}

void ScintillaEditBase::selectAll()
{
    QsciScintilla::selectAll(true);
}

size_t ScintillaEditBase::selectionStart() const
{
    return static_cast<size_t>(send(npp_sci::SCI_GETSELECTIONSTART));
}

size_t ScintillaEditBase::selectionEnd() const
{
    return static_cast<size_t>(send(npp_sci::SCI_GETSELECTIONEND));
}

void ScintillaEditBase::gotoPos(int pos)
{
    send(SCI_GOTOPOS, pos);
}

void ScintillaEditBase::gotoLine(int line)
{
    // SCI_GOTOLINE uses line number (0-based)
    send(SCI_GOTOLINE, line);
}

int ScintillaEditBase::currentPos() const
{
    return static_cast<int>(send(SCI_GETCURRENTPOS));
}

int ScintillaEditBase::currentLine() const
{
    return static_cast<int>(send(npp_sci::SCI_GETCURRENTLINE));
}

int ScintillaEditBase::lineCount() const
{
    return lines();
}

void ScintillaEditBase::notifyParent(const SCNotification*)
{
    // Forward Scintilla notifications as Qt signals
    // QsciScintilla emits SCNotification as Qt signals automatically
}

bool ScintillaEditBase::event(QEvent* e)
{
    return QsciScintilla::event(e);
}

void ScintillaEditBase::wheelEvent(QWheelEvent* e)
{
    QsciScintilla::wheelEvent(e);
}

void ScintillaEditBase::keyPressEvent(QKeyEvent* e)
{
    QsciScintilla::keyPressEvent(e);
}

void ScintillaEditBase::autoCompleteCancel()
{
    send(npp_sci::SCI_AUTOCCANCEL);
}

void ScintillaEditBase::autoCompleteSelect(const QString& text)
{
    // SCI_AUTOCSELECT: selects and inserts the chosen autocompletion item
    QByteArray utf8 = text.toUtf8();
    send(SCI_AUTOCSELECT, 0, reinterpret_cast<sptr_t>(utf8.constData()));
}
