// ScintillaCtrls.h - Qt port
#pragma once
#include <vector>
#include <QWidget>
#include "ScintillaEditView.h"
class ScintillaCtrls {
public:
    void init(HINSTANCE hInst, HWND hNpp) { _hInst = hInst; _hParent = hNpp; }
    void init(QWidget* parent) { _hInst = nullptr; _hParent = reinterpret_cast<HWND>(parent); }
    HWND createSintilla(HWND hParent);
    ScintillaEditView* getScintillaEditViewFrom(HWND handle2Find);
    void destroy();
private:
    std::vector<ScintillaEditView*> _scintVector;
    HINSTANCE _hInst = nullptr;
    HWND _hParent = nullptr;
    int getIndexFrom(HWND handle2Find);
};
