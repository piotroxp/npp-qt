// Printer.h - Qt port
#pragma once
#include <QPrinter>
#include <QPrintPreviewWidget>
#include <QPrintDialog>
#include <QPainter>
#include "ScintillaEditView.h"
// Forward declarations
#ifndef intPtr_t
using intPtr_t = qintptr;
#endif

class Printer : public QWidget {
    Q_OBJECT
public:
    Printer() : QWidget() {}
    void init(void* hInst, void* hwndApp, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); _hParent = hwndApp; _ppEditView = ppEditView; }
    bool filePrint();
    bool filePrintPreview();
    bool filePrintAgain();
    bool hasPrintableFiles();
    void printPixelPage(QPainter* dc, void* hWnd, intPtr_t* nbPrintedPages, intPtr_t* totalPrintedLines, bool oneBased = true);
    void printAllPage(QPainter* dc, intPtr_t* nbPages);
    void doPrint(bool isMain = true);
    bool render(int page);
private:
    void* _hParent = nullptr;
    ScintillaEditView** _ppEditView = nullptr;
    QPrinter _printer;
    std::vector<bool> _pagesPrinted;
};