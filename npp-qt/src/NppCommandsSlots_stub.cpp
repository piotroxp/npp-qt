// STUB FILE — provides linker-needed implementations for disabled .cpp files
// All stubs are no-ops for Qt6 build verification

#include "NppBigSwitch.h"
#include "Parameters.h"
#include "WinControls/ClipboardHistory.h"

// NOTE: NppBigSwitch stubs removed — proper implementations are in NppBigSwitch.cpp
// which defines constructor, destructor, eventFilter, handleNppEvent, run_dlgProc, etc.
// Duplicate symbol conflict: these were also defined in NppCommandsSlots_stub.cpp.

// === FileManager stubs ===
#include "Buffer.h"
Buffer* FileManager::getBufferByID(BufferID id) { return static_cast<Buffer*>(id.get()); }

// === NppParameters stubs ===

// getLangFromExt, getLangExtFromLangType, getPluginCmdsFromXmlTree
// IMPLEMENTED in Parameters.cpp

// writeFileBrowserSettings — IMPLEMENTED in Parameters.cpp
QRgb NppParameters::getIndividualTabColor(int colourIndex, bool isDarkMode, bool saturated)
{
    // 5 custom tab colours, indexed 0-4
    // These are the default NPP tab colours — can be customized in preferences
    static const QRgb darkTabColours[5] = {
        0xCC1111,  // Tab Colour 1: red
        0x11CC11,  // Tab Colour 2: green
        0x1111CC,  // Tab Colour 3: blue
        0xCCCC11,  // Tab Colour 4: yellow
        0x11CCCC,  // Tab Colour 5: cyan
    };
    static const QRgb lightTabColours[5] = {
        0xDD2222,  // Tab Colour 1: red
        0x22DD22,  // Tab Colour 2: green
        0x2222DD,  // Tab Colour 3: blue
        0xDDDD22,  // Tab Colour 4: yellow
        0x22DDDD,  // Tab Colour 5: cyan
    };
    if (colourIndex < 0 || colourIndex >= 5)
        return qRgba(0, 0, 0, 0);

    QRgb base = isDarkMode ? darkTabColours[colourIndex] : lightTabColours[colourIndex];
    if (!saturated) {
        // Desaturate: blend with gray
        int r = qRed(base), g = qGreen(base), b = qBlue(base);
        int gray = (r + g + b) / 3;
        int factor = 60;  // blend factor
        r = (r * (100 - factor) + gray * factor) / 100;
        g = (g * (100 - factor) + gray * factor) / 100;
        b = (b * (100 - factor) + gray * factor) / 100;
        return qRgba(r, g, b, 255);
    }
    return base;
}

// ThemeSwitcher methods (addDefaultThemeFromXml, getElementFromIndex) — IMPLEMENTED in Parameters.cpp
// buildMenuFileName — IMPLEMENTED in MISC/Common/Common.cpp
