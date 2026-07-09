// Notepad_plus.h — Compatibility shim for Notepad--Qt
// Forwards legacy calls to Application
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QString>
#include "common/Types.h"

class Notepad_plus : public QObject {
    Q_OBJECT
public:
    static Notepad_plus& getInstance();
    void newFile();
    void openFile(const QString& path = QString());
    void saveFile();
    void saveFileAs(const QString& path = QString());
    void closeFile();
    int getCurrentBufferId() const;
    void setCurrentBuffer(int id);
    void zoomIn();
    void zoomOut();
    void zoomRestore();
    void changeEncoding(int enc);
    void changeEol(int eol);
private:
    explicit Notepad_plus(QObject* parent = nullptr);
    ~Notepad_plus() override;
    Q_DISABLE_COPY_MOVE(Notepad_plus)
};
