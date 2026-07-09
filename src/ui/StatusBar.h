// StatusBar.h - Status bar widget
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QStatusBar>
#include <QLabel>
#include <QToolButton>

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override;

    void setPosition(int line, int col);
    void setEncoding(const QString& enc);
    void setEol(const QString& eol);
    void setLanguage(const QString& lang);
    void setFileType(const QString& type);
    void setMessage(const QString& msg);
    void setSelection(int start, int end);

private:
    QLabel* _posLabel = nullptr;
    QLabel* _encLabel = nullptr;
    QLabel* _eolLabel = nullptr;
    QLabel* _langLabel = nullptr;
    QLabel* _typeLabel = nullptr;
    QLabel* _selLabel = nullptr;
};
