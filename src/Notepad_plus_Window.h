// npp-qt: forwarding header — Notepad_plus_Window is semantically lifted to MainWindow
// Notepad_plus.h uses Notepad_plus_Window* as its public interface pointer.
// In Qt6, this is replaced by MainWindow* (QMainWindow subclass).
// This header keeps the Notepad_plus_Window name available as a type alias.
#pragma once
#include "MainWindow.h"
using Notepad_plus_Window = MainWindow;
