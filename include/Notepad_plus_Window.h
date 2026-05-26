// Notepad_plus_Window.h - Qt6 port
// Manages the main window wrapper for Notepad_plus
#pragma once

#include "Notepad_plus.h"

class Notepad_plus_Window {
public:
    Notepad_plus_Window() : _pNppWindow(nullptr) {}
    ~Notepad_plus_Window() { destroy(); }

    void init(HINSTANCE hInst, HWND hwnd) {
        _pNppWindow = new Notepad_plus();
        // Register with system
    }

    void destroy() {
        if (_pNppWindow) {
            delete _pNppWindow;
            _pNppWindow = nullptr;
        }
    }

    Notepad_plus* getNppHandle() const { return _pNppWindow; }

private:
    Notepad_plus* _pNppWindow;
};

// Global window instance
extern Notepad_plus_Window nppWindow;