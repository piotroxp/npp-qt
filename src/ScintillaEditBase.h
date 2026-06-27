#pragma once

// ScintillaEditBase — Qt6 Scintilla wrapper using QScintilla
//
// Uses QScintilla (libqscintilla2-qt6) as the base editor widget.
// QsciScintilla provides the full Scintilla editing engine with Qt bindings.
//
// Note: sptr_t / uptr_t are self-contained here (Qt/Scintilla types).
// Do NOT include <Scintilla.h> — it has macros that conflict with C++ headers.
// SCI_* constants come from QsciScintillaBase (included via qsciscintilla.h).

#include <QString>
#include <QByteArray>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QEvent>
#include <QtGlobal>       // for qintptr / quintptr
#include <cstdint>       // for intptr_t / uintptr_t
#include "NppSciCompat.h"
#include <Qsci/qsciprinter.h>
#include <Qsci/qsciscintilla.h>

// sptr_t / uptr_t — Scintilla integer types (mirrors Scintilla.h)
// Guard against redefinition (Scintilla.h also defines these via SCINTILLA_SPTR_T_DEFINED)
#ifndef SCINTILLA_SPTR_T_DEFINED
using sptr_t = intptr_t;
#endif
#ifndef SCINTILLA_UPTR_T_DEFINED
using uptr_t = uintptr_t;
#endif

// Scintilla forward declarations
// SCNotification is the native Scintilla notification struct.
struct SCNotification;
namespace Scintilla {
    enum class ModificationFlags : int;
    enum class FoldLevel : int;
    enum class Update : int;
    enum class KeyMod : int;
    using Position = int;
}

class ScintillaEditBase : public QsciScintilla
{
    Q_OBJECT

public:
    explicit ScintillaEditBase(QWidget* parent = nullptr);
    ~ScintillaEditBase() override;

    // Low-level send — mirrors Win32 SendMessage(SCI_*, ...)
    // QsciScintilla::SendScintilla() takes (int, intptr_t, intptr_t)
    sptr_t send(unsigned int msg, uptr_t wParam = 0, sptr_t lParam = 0) const;

    // Text
    void setText(const char* text);
    void setText(const QByteArray& text);
    QString text() const;
    QString selectedText() const;
    int length() const;

    // Selection
    void selectAll();
    size_t selectionStart() const;
    size_t selectionEnd() const;

    // Navigation
    void gotoPos(int pos);
    void gotoLine(int line);
    int currentPos() const;
    int currentLine() const;
    int lineCount() const;

    // Scintilla notifications → Qt signals
Q_SIGNALS:
    void marginClicked(Scintilla::Position pos, Scintilla::KeyMod mod, int margin);
    void doubleClick(Scintilla::Position pos, Scintilla::Position line);
    void modified(Scintilla::ModificationFlags type,
                  Scintilla::Position pos,
                  Scintilla::Position len,
                  const QByteArray& text,
                  Scintilla::Position line,
                  Scintilla::FoldLevel foldNow,
                  Scintilla::FoldLevel foldPrev);
    void updateUi(Scintilla::Update updated);
    void charAdded(int ch);
    void savePointChanged(bool dirty);
    void modifyAttemptReadOnly();
    void zoom(int zoomLevel);
    void needShown(Scintilla::Position pos, Scintilla::Position len);
    void painted();
    void userListSelection(const QString& text, int type);
    void linesAdded(int lines);

public slots:
    void autoCompleteCancel();
    void autoCompleteSelect(const QString& text);

protected:
    virtual void notifyParent(const SCNotification* scn);
    bool event(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void init();
    int _id = 0;
    static int _nextId;
};
