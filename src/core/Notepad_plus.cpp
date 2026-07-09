// Notepad_plus.cpp — Compatibility shim for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "Notepad_plus.h"
#include "Application.h"

Notepad_plus& Notepad_plus::getInstance() {
    static Notepad_plus instance;
    return instance;
}

Notepad_plus::Notepad_plus(QObject* parent) : QObject(parent) {}
Notepad_plus::~Notepad_plus() = default;

void Notepad_plus::newFile() { Application::instance().newFile(); }

void Notepad_plus::openFile(const QString& path) {
    if (path.isEmpty()) Application::instance().onOpenFile();
    else Application::instance().openFile(path.toStdString());
}

void Notepad_plus::saveFile() { Application::instance().onSaveFile(); }

void Notepad_plus::saveFileAs(const QString& path) {
    Q_UNUSED(path);
    Application::instance().onSaveFileAs();
}

void Notepad_plus::closeFile() { Application::instance().onCloseFile(); }

int Notepad_plus::getCurrentBufferId() const {
    return reinterpret_cast<std::intptr_t>(Application::instance().getActiveBuffer());
}

void Notepad_plus::setCurrentBuffer(int id) {
    Application::instance().setActiveBuffer(reinterpret_cast<BufferID>(id));
}

void Notepad_plus::zoomIn()    { Application::instance().onMenuCommand("zoomIn"); }
void Notepad_plus::zoomOut()   { Application::instance().onMenuCommand("zoomOut"); }
void Notepad_plus::zoomRestore(){ Application::instance().onMenuCommand("zoomRestore"); }

void Notepad_plus::changeEncoding(int enc) {
    Application::instance().onConvertEncoding(static_cast<EncodingType>(enc));
}

void Notepad_plus::changeEol(int) {
    // TODO: Application needs EOL conversion slot
}
