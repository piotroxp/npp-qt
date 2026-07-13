// Semantic Lift: Win32 AboutDlg → Qt6 QDialog implementation
// Original: PowerEditor/src/WinControls/AboutDlg/
// Target: npp-qt/src/WinControls/AboutDlg.h + .cpp

#pragma once

#include "StaticDialog.h"
#include <QWidget>
#include <QString>
#include <QIcon>
#include <QString>

// Forward declarations
class Window;
class URLCtrl;

// =============================================================================
// AboutDlg — Main "About Notepad++" dialog
// =============================================================================

class AboutDlg : public StaticDialog
{
    Q_OBJECT

public:
    AboutDlg() = default;
    ~AboutDlg() override;

    void doDialog();

    void destroy();

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    void updateIcon();

    QIcon _chameleonIcon;
    bool _iconLoaded = false;
};

// =============================================================================
// DebugInfoDlg — Debug information dialog
// =============================================================================

class DebugInfoDlg : public StaticDialog
{
    Q_OBJECT

public:
    DebugInfoDlg() = default;
    ~DebugInfoDlg() override = default;

    void init(bool isAdmin, const QString& loadedPlugins);
    void doDialog();
    void refreshDebugInfo();
    void destroy() override {}

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    void buildDebugInfoString();

    QString _debugInfoStr;
    QString _debugInfoDisplay;
    QString _loadedPlugins;
    bool _isAdmin = false;
};

// =============================================================================
// CmdLineArgsDlg — Command line arguments help dialog
// =============================================================================

class CmdLineArgsDlg : public StaticDialog
{
    Q_OBJECT

public:
    CmdLineArgsDlg() = default;
    ~CmdLineArgsDlg() override = default;

    void doDialog();
    void destroy() override {}

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    QFont _cmdLineEditFont;
};

// =============================================================================
// DoSaveOrNotBox — "Save file?" confirmation dialog
// =============================================================================

class DoSaveOrNotBox : public StaticDialog
{
    Q_OBJECT

public:
    DoSaveOrNotBox() = default;
    ~DoSaveOrNotBox() override = default;

    void init(const QString& filename, bool isMulti);
    void doDialog(bool isRTL = false);
    void destroy() override {}

    int getClickedButtonId() const { return _clickedButtonId; }
    void changeLang();

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    int _clickedButtonId = -1;
    QString _filename;
    bool _isMulti = false;
};

// =============================================================================
// DoSaveAllBox — "Save all modified?" confirmation dialog
// =============================================================================

class DoSaveAllBox : public StaticDialog
{
    Q_OBJECT

public:
    DoSaveAllBox() = default;
    ~DoSaveAllBox() override = default;

    void doDialog(bool isRTL = false);
    void destroy() override {}

    int getClickedButtonId() const { return _clickedButtonId; }
    void changeLang();

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    int _clickedButtonId = -1;
};