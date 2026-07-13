// Stub QApplication for unit tests that need to compile against
// source files originally requiring Qt GUI, but where the test
// executable itself does not initialize QApplication.
// Provides minimal forward declarations and inline stubs so that
// files like StaticDialog.cpp, ColourPicker.cpp etc. compile cleanly.
#pragma once

// Only activate stubs when building as part of the unit-test library
#ifdef UNIT_TEST_STUBS

#include <QWidget>
#include <QScreen>
#include <QCursor>
#include <QPoint>
#include <QRect>

// Minimal QApplication stub
class QApplication {
public:
    static QScreen* primaryScreen() { return nullptr; }
    static QScreen* screenAt(const QPoint&) { return nullptr; }
    static Qt::KeyboardModifiers queryKeyboardModifiers() { return Qt::NoModifier; }
    static void processEvents() {}
};

#else
// Normal Qt includes when not in unit-test stub mode
#include <QApplication>
#endif
