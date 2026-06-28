// Semantic lift: DarkMode.h — Win32 dark mode APIs → Qt6 stub / NppDarkMode bridge
// Original: PowerEditor/src/DarkMode/DarkMode.h
// Target: npp-qt/src/DarkMode/DarkMode.h
// Win32→Qt6: HWND → QWidget*; Win32 theme APIs → NppDarkMode singleton
// Copyright (C) 2021 adzm / Adam D. Walling — GPLv3

#pragma once

#include <QWidget>

// =============================================================================
// Forward declarations
// =============================================================================

namespace NppDarkMode {
    class NppDarkMode;
}

// =============================================================================
// Global dark mode state — mirrors Win32 g_darkModeSupported / g_darkModeEnabled
// Delegated to NppDarkMode::NppDarkMode singleton on Qt6
// =============================================================================

extern bool g_darkModeSupported;
extern bool g_darkModeEnabled;

// =============================================================================
// Dark mode query / enable (Win32 uxtheme.dll ordinals 132-139)
// =============================================================================

// ShouldAppsUseDarkMode() — query system dark mode preference
// Win32: fnShouldAppsUseDarkMode (ordinal 132) from uxtheme.dll
// Qt6:  delegates to NppDarkMode::isEnabled_Static()
bool ShouldAppsUseDarkMode();

// AllowDarkModeForWindow() — per-window dark mode opt-in
// Win32: fnAllowDarkModeForWindow (ordinal 133) from uxtheme.dll
// Qt6:  N/A — dark mode is application-wide via NppDarkMode
bool AllowDarkModeForWindow(QWidget* widget, bool allow);

// IsHighContrast() — accessibility high-contrast mode detection
// Win32: SystemParametersInfoW(SPI_GETHIGHCONTRAST)
// Qt6:  stubs to false on Linux (X11/Wayland handles this natively)
bool IsHighContrast();

// AllowDarkModeForApp() — application-wide dark mode opt-in
// Win32: fnAllowDarkModeForApp / fnSetPreferredAppMode (ordinal 135)
// Qt6:  calls NppDarkMode::setEnabled()
void AllowDarkModeForApp(bool allow);

// =============================================================================
// Title bar theming (Windows 10 1809+ / Windows 11)
// SetWindowCompositionAttribute + WCA_USEDARKMODECOLORS → Qt window flags
// =============================================================================

// RefreshTitleBarThemeColor() — reapply dark/light title bar on theme change
// Win32: uses _IsDarkModeAllowedForWindow + _ShouldAppsUseDarkMode
// Qt6:  emits QEvent::ThemeChange / rebuilds window palette
void RefreshTitleBarThemeColor(QWidget* widget);

// SetTitleBarThemeColor() — apply dark (true) or light (false) title bar
// Win32: SetPropW / SetWindowCompositionAttribute
// Qt6:  QWidget::setStyleSheet / palette on title bar area (N/A on X11/Wayland)
void SetTitleBarThemeColor(QWidget* widget, bool dark);

// =============================================================================
// Colour-scheme change messages (WM_SETTINGCHANGE "ImmersiveColorSet")
// =============================================================================

// IsColorSchemeChangeMessage() — detect WM_SETTINGCHANGE with "ImmersiveColorSet"
// Win32: lstrcmpi(lParam, L"ImmersiveColorSet") + _RefreshImmersiveColorPolicyState
// Qt6:  QEvent::ThemeChange / QStyleHints::colorSchemeChanged
bool IsColorSchemeChangeMessage(long lParam);
bool IsColorSchemeChangeMessage(unsigned int message, long lParam);

// =============================================================================
// Dark scroll bar (per-window, limited scope)
// Win32: hooks comctl32 ScrollBar class to Explorer::ScrollBar
// Qt6:  NppDarkMode::setDarkScrollBar() + stylesheet
// =============================================================================

// EnableDarkScrollBarForWindowAndChildren() — apply dark scrollbar to widget + children
// Win32: inserts hwnd into g_darkScrollBarWindows set
// Qt6:  calls NppDarkMode::enableDarkScrollBarForWindowAndChildren()
void EnableDarkScrollBarForWindowAndChildren(QWidget* widget);

// =============================================================================
// Init / set
// =============================================================================

// InitDarkMode() — resolve uxtheme.dll ordinals (Win32) / stub on Linux
// Qt6:  no-op; NppDarkMode is always available
void InitDarkMode();

// SetDarkMode() — enable/disable dark mode globally
// Win32: AllowDarkModeForApp + FixDarkScrollBar + ShouldAppsUseDarkMode
// Qt6:  NppDarkMode::setEnabled()
void SetDarkMode(bool useDarkMode, bool fixDarkScrollbar);

// =============================================================================
// OS version detection (Win32 build numbers → stubs on Linux)
// =============================================================================

// IsWindows10() — build >= 17763
// Qt6:  always false on Linux
bool IsWindows10();

// IsWindows11() — build >= 22000
// Qt6:  always false on Linux
bool IsWindows11();

// GetWindowsBuildNumber() — return OS build (0 on non-Windows)
// Qt6:  returns 0
unsigned int GetWindowsBuildNumber();
