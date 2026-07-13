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
