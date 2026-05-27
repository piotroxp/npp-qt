// DocumentPeeker.h - Qt6 stub (document peek/preview functionality)
#pragma once
#include <QWidget>

class DocumentPeeker : public QWidget {
    Q_OBJECT
public:
    DocumentPeeker(QWidget* parent = nullptr) : QWidget(parent) {}
    void init() {}
    void peek(int line) { Q_UNUSED(line); }
};