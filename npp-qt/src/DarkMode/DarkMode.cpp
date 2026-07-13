// Semantic lift: DarkMode.cpp — Win32 dark mode → Qt6 stub / NppDarkMode bridge
// Original: PowerEditor/src/DarkMode/DarkMode.cpp
// Target: npp-qt/src/DarkMode/DarkMode.cpp
// Win32→Qt6: uxtheme.dll ordinals → NppDarkMode singleton; HWND → QWidget*
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3

#include "DarkMode.h"

#include <QApplication>
#include <QStyleHints>
#include <QWidget>

// Link against the existing NppDarkMode implementation
#include "NppDarkMode.h"

// =============================================================================
// Global state — mirrors Win32 globals
// On Qt6 these delegate to NppDarkMode singleton
// =============================================================================

bool g_darkModeSupported = false;   // Win32: resolved uxtheme.dll ordinals
bool g_darkModeEnabled = false;      // Win32: ShouldAppsUseDarkMode() && !IsHighContrast()
unsigned int g_buildNumber = 0;      // Win32: from RtlGetNtVersionNumbers

// =============================================================================
// Dark mode query
// =============================================================================

bool ShouldAppsUseDarkMode()
{
    // Win32: _ShouldAppsUseDarkMode() from uxtheme ordinal 132
    // Qt6:   delegate to NppDarkMode singleton
    return NppDarkMode::isEnabled_Static();
}

bool AllowDarkModeForWindow(QWidget* widget, bool allow)
{
    Q_UNUSED(widget);
    Q_UNUSED(allow);
    // Win32: _AllowDarkModeForWindow(hWnd, allow) from uxtheme ordinal 133
    //        returns false if dark mode not supported globally
    // Qt6:   dark mode is application-wide via NppDarkMode; per-window
    //        theming is done via setStyleSheet / applyToWidget
    if (!g_darkModeSupported)
        return false;

    // If allowing dark mode for this specific widget, apply NppDarkMode palette
    if (allow) {
        NppDarkMode::instance().applyToWidget(widget);
    }
    return g_darkModeSupported;
}

bool IsHighContrast()
{
    // Win32: SystemParametersInfoW(SPI_GETHIGHCONTRAST, ..., &highContrast, FALSE)
    //        checks HCF_HIGHCONTRASTON flag
    // Qt6:   on X11/Wayland the window manager handles high-contrast natively;
    //        we can check QStyleHints for accessibility hints if available
    // Stub: return false (accessibility handled by OS/DE)
    return false;
}

void SetTitleBarThemeColor(QWidget* widget, bool dark)
{
    if (!widget)
        return;

    // Win32:
    //   - Build < 18362: SetPropW(hWnd, L"UseImmersiveDarkModeColors", dark)
    //   - Build >= 18362: _SetWindowCompositionAttribute(hWnd, WCA_USEDARKMODECOLORS)
    //
    // Qt6:
    //   On X11/Wayland the title bar colour is controlled by the window manager,
    //   not by the application. We apply a dark palette to the widget itself.
    //   The window manager may or may not honour this for the actual title bar.
    if (dark) {
        NppDarkMode::instance().setTitleBarDark(widget);
    } else {
        NppDarkMode::instance().setTitleBarLight(widget);
    }
}

void RefreshTitleBarThemeColor(QWidget* widget)
{
    if (!widget)
        return;

    // Win32:
    //   BOOL dark = FALSE;
    //   if (_IsDarkModeAllowedForWindow(hWnd) && _ShouldAppsUseDarkMode() && !IsHighContrast())
    //       dark = TRUE;
    //   SetTitleBarThemeColor(hWnd, dark);
    //
    // Qt6:
    //   On non-Windows there is no per-window dark mode permission.
    //   Use the global dark mode state from NppDarkMode.
    bool dark = NppDarkMode::isEnabled_Static() && !IsHighContrast();
    SetTitleBarThemeColor(widget, dark);
}

bool IsColorSchemeChangeMessage(long lParam)
{
    // Win32:
    //   if (lParam && lstrcmpi(lParam, L"ImmersiveColorSet") == 0)
    //       _RefreshImmersiveColorPolicyState();
    //
    // Qt6:
    //   Qt catches WM_SETTINGCHANGE / QEvent::ThemeChange internally.
    //   This function is a no-op on non-Windows.
    Q_UNUSED(lParam);
    return false;
}

bool IsColorSchemeChangeMessage(unsigned int message, long lParam)
{
    // Win32: message == WM_SETTINGCHANGE (0x001A)
    // Qt6:   this is only triggered on Windows WM_SETTINGCHANGE events
    Q_UNUSED(message);
    Q_UNUSED(lParam);
    return false;
}

void AllowDarkModeForApp(bool allow)
{
    // Win32:
    //   if (_AllowDarkModeForApp)
    //       _AllowDarkModeForApp(allow);
    //   else if (_SetPreferredAppMode)
    //       _SetPreferredAppMode(allow ? ForceDark : Default);
    //
    // Qt6:
    //   Delegate to NppDarkMode singleton — setEnabled drives the global palette.
    NppDarkMode::instance().setEnabled(allow);
}

void FlushMenuThemes()
{
    // Win32: _FlushMenuThemes() from uxtheme ordinal 136
    //        forces comctl32 to refresh menu theme
    // Qt6:   no-op — Qt's menu rendering is widget-based, not theme-based
    //         (the Qt style sheet controls everything)
}

// =============================================================================
// Dark scroll bar — per-window scope
// Win32: g_darkScrollBarWindows set + FixDarkScrollBar hooks OpenNcThemeData
// Qt6:  NppDarkMode::enableDarkScrollBarForWindowAndChildren()
// =============================================================================

void EnableDarkScrollBarForWindowAndChildren(QWidget* widget)
{
    // Win32: inserts hwnd into g_darkScrollBarWindows set
    // Qt6:  NppDarkMode static convenience method
    if (widget) {
        NppDarkMode::enableDarkScrollBarForWindowAndChildren(widget);
    }
}

// =============================================================================
// Init / set
// =============================================================================

void InitDarkMode()
{
    // Win32:
    //   1. RtlGetNtVersionNumbers → g_buildNumber
    //   2. Load uxtheme.dll + resolve ordinals 49, 104, 106, 132, 133, 135, 136, 137
    //   3. Load user32.dll + resolve SetWindowCompositionAttribute
    //   4. Set g_darkModeSupported = true if all resolved
    //
    // Qt6:
    //   Dark mode is always available via NppDarkMode (no OS dependency).
    //   g_darkModeSupported mirrors NppDarkMode availability (always true on Qt6).
    g_darkModeSupported = true;
}

void SetDarkMode(bool useDark, bool fixDarkScrollbar)
{
    // Win32:
    //   AllowDarkModeForApp(useDark);
    //   FlushMenuThemes();
    //   if (fixDarkScrollbar) FixDarkScrollBar();
    //   g_darkModeEnabled = ShouldAppsUseDarkMode() && !IsHighContrast();
    //
    // Qt6:
    //   Use NppDarkMode singleton — enable/disable dark mode and apply styles.
    if (g_darkModeSupported) {
        AllowDarkModeForApp(useDark);
        // Apply to the main application widget (main window) and all children
        if (fixDarkScrollbar) {
            // Walk up to find the top-level widget
            QWidget* topLevel = QApplication::topLevelWidgets().value(0);
            if (topLevel) {
                EnableDarkScrollBarForWindowAndChildren(topLevel);
            }
        }
        g_darkModeEnabled = NppDarkMode::isEnabled_Static() && !IsHighContrast();
    }
}

// =============================================================================
// OS version stubs (Win32 → always false / 0 on Linux)
// =============================================================================

bool IsWindows10()
{
    // Win32: return (g_buildNumber >= 17763)
    // Qt6/Linux: not Windows, so always false
    return false;
}

bool IsWindows11()
{
    // Win32: return (g_buildNumber >= 22000)
    // Qt6/Linux: not Windows, so always false
    return false;
}

unsigned int GetWindowsBuildNumber()
{
    // Win32: returns g_buildNumber from RtlGetNtVersionNumbers
    // Qt6:   not applicable on Linux
    return 0;
}
