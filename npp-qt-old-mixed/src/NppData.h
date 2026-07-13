// NppData.h — Notepad++ plugin interface data
// Mirrors the Win32 NPPDATASTRUCT passed to plugins.
// Used by PluginsManager::init(const NppData&).

#pragma once

#include <QWidget>

struct NppData {
    QWidget* _nppHandle = nullptr;
    QWidget* _scintillaMainHandle = nullptr;
    QWidget* _scintillaSecondHandle = nullptr;
};
