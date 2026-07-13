// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
// Printer: Win32 PrintDlg/StartDoc → Qt QPrinter
// Port of Printer.h/.cpp — maps Win32 HDC printing to Qt QPrinter
#include <QObject>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include "ScintillaComponent.h"

class NppPrinter : public QObject
{
    Q_OBJECT
public:
    NppPrinter() = default;
    ~NppPrinter() override = default;

    // init: showPrintDialog=true → QPrintDialog; false → use default printer
    void init(QWidget* parent, ScintillaComponent* pSEView,
              bool showDialog, size_t startPos, size_t endPos, bool isRTL = false);

    // doPrint: returns number of pages printed
    size_t doPrint(bool justDoIt = true);

    // Show print preview (stub)
    void printPreview() {}

    // Direct print without dialog (stub)
    size_t printDirect() { return doPrint(true); }

private:
    ScintillaComponent* _pSEView = nullptr;
    size_t _startPos = 0;
    size_t _endPos = 0;
    bool _isRTL = false;
    QPrinter _printer;
};
