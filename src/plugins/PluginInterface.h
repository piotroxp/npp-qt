#pragma once
#include <string>

// Plugin API version — increment when incompatible changes are made
#define NPPQT_PLUGIN_API_VERSION 1

// Forward declarations
class ScintillaEditor;

// Plugin function table — plugins receive this struct and use it to interact with the editor
struct NppQtFuncs {
    // Document
    const char* (*getCurrentText)(int* length);          // Returns current document text
    void (*setCurrentText)(const char* text, int length);  // Replace current document
    int (*getCurrentLine)();                                // Current cursor line (0-based)
    int (*getCurrentColumn)();                             // Current cursor column
    void (*setCursorPosition)(int line, int column);

    // Buffer
    int (*getBufferCount)();                               // Number of open buffers
    const char* (*getBufferPath)(int bufferId);            // Get buffer file path

    // Editor
    void (*addToOutput)(const char* msg);                 // Print to output panel
    void (*insertText)(const char* text);                  // Insert at cursor
    void (*replaceLine)(int line, const char* newText);    // Replace entire line

    // Menu
    void (*registerMenuItem)(const char* label, void (*callback)());
    void (*unregisterMenuItem)(const char* label);

    // Language
    void (*setLanguage)(const char* langName);             // e.g. "python", "cpp"

    // Version
    int (*getVersion)();                                   // Returns NPPQT_PLUGIN_API_VERSION
};

// Plugin must export these functions:
struct NppQtPlugin {
    int apiVersion;                                        // Must be NPPQT_PLUGIN_API_VERSION
    const char* pluginName;                                // Display name
    void (*init)(NppQtFuncs* funcs);                       // Called on load
    void (*cleanup)();                                     // Called on unload
};

// Notepad++ compatible structures (for compatibility with NPP plugins)
struct ShortcutKey {
    bool _isCtrl;
    bool _isAlt;
    bool _isShift;
    unsigned char _key;
};

struct FuncItem {
    char _itemName[64];
    void (*_pFunc)();
    int _cmdID;
    bool _init2Check;
    ShortcutKey* _pShKey;
};

// Notepad++ data structure (stubbed for compatibility)
struct NppData {
    void* _nppHandle;      // HWND - Notepad++ main window
    void* _scintillaMainHandle;  // HWND - Scintilla first instance
    void* _scintillaSecondHandle;  // HWND - Scintilla second instance
};

// Notepad++ plugin export (legacy compatibility)
struct FuncItem* npp_getFunctionItems();
void npp_setInfo(NppData* nppData);
void npp_beNotified(void* notifyCode);
int npp_messageProc(int message, int wParam, int lParam);

// Helper macro for plugins to export their plugin struct
#define NPPQT_PLUGIN(name, init_fn, cleanup_fn) \
    extern "C" NppQtPlugin nppqt_getPlugin() { \
        return NppQtPlugin{ \
            .apiVersion = NPPQT_PLUGIN_API_VERSION, \
            .pluginName = name, \
            .init = init_fn, \
            .cleanup = cleanup_fn \
        }; \
    }

// Legacy NPP plugin export macro (for compatibility)
#define NPP_PLUGIN(name) \
    extern "C" __declspec(dllexport) FuncItem* npp_getFunctionItems(); \
    extern "C" __declspec(dllexport) void npp_setInfo(NppData*); \
    FuncItem g_funcItems[1]; \
    const char g_pluginName[] = name;
