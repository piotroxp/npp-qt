// This file is part of Notepad++ project
// DPI awareness manager - stubbed for non-Windows

#pragma once
#include <QWidget>

class DpiManagerV2 final
{
public:
    static DpiManagerV2& getInstance() {
        static DpiManagerV2 inst;
        return inst;
    }
    int getDpi() const { return 96; }
    double getScaleFactor() const { return 1.0; }
    void onDpiChange(int) {}
private:
    DpiManagerV2() = default;
};
