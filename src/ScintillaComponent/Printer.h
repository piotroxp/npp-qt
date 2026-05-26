// Printer.h - Qt port
#pragma once
#include <QPrinter>
#include <QPrintPreviewWidget>
#include <QPrintDialog>
#include <QPainter>
#include "ScintillaEditView.h"
class Printer : public QWidget {
    Q_OBJECT
public:
    Printer() : QWidget() {}
    void init(HINSTANCE hInst, HWND hwndApp, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); _hParent = hwndApp; _ppEditView = ppEditView; }
    bool filePrint(); bool filePrintPreview(); bool filePrintAgain(); bool hasPrintableFiles();
    void printPixelPage(QPainter* dc, HWND hWnd, intPtr_t* nbPrintedPages, intPtr_t* totalPrintedLines, bool oneBased = true);
    void printAllPage(QPainter* dc, intPtr_t* nbPages);
    void doPrint(bool isMain = true);
    bool render(int page);

private:
    HWND _hParent = nullptr;
    ScintillaEditView** _ppEditView = nullptr;
    QPrinter _printer;
    std::vector<bool> _pagesPrinted;
};
HeaderEOF
echo "Printer.h created"

cat > /workspace/piotro/npp-qt/src/ScintillaComponent/columnEditor.h << 'HEADEREOF'
// columnEditor.h - Qt port
#pragma once
#include <QDialog>
#include <QTableWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include "ScintillaEditView.h"
enum Order { Incr, Decr, Insert };
enum Action { Fill_Every_N, Fill_On_Selection, Fill_On_All_Found, Delete, Revert };
class columnEditor : public QDialog {
    Q_OBJECT
public:
    columnEditor() : QDialog() {}
    void init(HINSTANCE hInst, HWND hParent, ScintillaEditView** ppEditView) { Q_UNUSED(hInst); Q_UNUSED(hParent); _ppEditView = ppEditView; }
    void doModal(bool isRTL = false);
protected:
    int run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) { Q_UNUSED(message); Q_UNUSED(wParam); Q_UNUSED(lParam); return 0; }
private:
    ScintillaEditView** _ppEditView = nullptr;
    int _fillColumn;
    int _fillIncDec;
    int _numericValue;
    int _numericStep;
    int _nbRows;
    int _currentIndex;
};
HeaderEOF
echo "columnEditor created"