// Semantic lift: lesDlgs.h — Win32 encoding/minimal dialogs → Qt6
// Original: PowerEditor/src/lesDlgs.h + .cpp (103 lines)
// Target: npp-qt/src/lesDlgs.h + .cpp
// Copyright (C) 2021 Don HO — GPLv3

#pragma once

#include "WinControls/StaticDialog.h"

#include <QWidget>
#include <QString>

// Button status flags (matching Win32 original)
constexpr int buttonStatus_nada           = 0;
constexpr int buttonStatus_fullscreen     = 1;
constexpr int buttonStatus_postit         = 2;
constexpr int buttonStatus_distractionFree = 4;

// =============================================================================
// ButtonDlg — "Distraction-free / Full-screen / Post-it" floating button
// =============================================================================
// This dialog shows a single floating button for quick view-mode toggling.
// It matches the Win32 ButtonDlg from lesDlgs.cpp.

class ButtonDlg : public StaticDialog
{
public:
    QWidget* getHSelf() const { return const_cast<ButtonDlg*>(this); }
    Q_OBJECT

public:
    ButtonDlg() = default;

    void doDialog(bool isRTL = false);
    // Win32 compatibility: init(HINSTANCE, HWND) stub (unused in Qt6)
    void init(void* /*hInst*/, QWidget* /*hwnd*/) { /* no-op: use doDialog() */ }

    int getButtonStatus() const { return _buttonStatus; }
    void setButtonStatus(int s) { _buttonStatus = s; }

    void display(bool toShow = true);

    // Pure virtual — implement the dialog procedure
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    void destroy() override {}

Q_SIGNALS:
    // Emitted when the button is clicked; parent decides which view command
    void viewCommand(int cmd);

private:
    int _buttonStatus = buttonStatus_nada;
};

// =============================================================================
// EncodingDlg — minimal encoding info dialog (placeholder for N++ encoding panel)
// =============================================================================

class EncodingDlg : public StaticDialog
{
    Q_OBJECT

public:
    EncodingDlg() = default;

    void doDialog(bool isRTL = false);

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    void destroy() override {}

    void setEncodingInfo(const QString& encodingName, bool hasBOM);

Q_SIGNALS:
    void encodingChanged(const QString& encodingName);

private:
    QString _currentEncoding;
    bool _hasBOM = false;
};
