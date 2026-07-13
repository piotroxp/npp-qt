// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Printer: Win32 PrintDlg/StartDoc → Qt QPrinter
// Port of Printer.h/.cpp — maps Win32 HDC printing to Qt QPrinter
#pragma once
#include <QObject>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QWidget>
#include "ScintillaComponent.h"

class NppPrinter : public QObject
{
    Q_OBJECT
public:
    NppPrinter() = default;
    ~NppPrinter() override = default;

    // init: stores print parameters (mirrors Win32 Printer::init)
    // parent = QWidget window handle, pSEView = Scintilla view to print
    // showDialog = true → skip QPrintDialog; startPos/endPos = selection range
    void init(QWidget* parent, ScintillaComponent* pSEView,
              bool showDialog, size_t startPos, size_t endPos, bool isRTL = false);

    // doPrint: shows dialog if justDoIt=false; returns number of pages printed
    size_t doPrint(bool justDoIt = true);

    // printPreview: shows Qt print preview dialog with header/footer
    void printPreview();

    // printDirect: print without dialog
    size_t printDirect() { return doPrint(true); }

private:
    ScintillaComponent* _pSEView = nullptr;
    QWidget* _parent = nullptr;
    size_t   _startPos = 0;
    size_t   _endPos   = 0;
    bool     _isRTL    = false;
    bool     _showDialog = true;
};
