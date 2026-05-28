// ScintillaAccelerator.h - Qt port
#pragma once
#include <QObject>
#include <QVector>
#include <QWidget>
#include <QMenu>

class ScintillaAccelerator : public QObject {
    Q_OBJECT
public:
    explicit ScintillaAccelerator(QObject* parent = nullptr);
    bool init(QVector<QWidget*>* views, QMenuBar* menu, QWidget* parent);
    void updateKeys();
};
