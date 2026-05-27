// SmartHighlighter.h - Qt6 stub
#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QRegularExpression>

class SmartHighlighter : public QObject {
    Q_OBJECT
public:
    SmartHighlighter(QObject* parent = nullptr) : QObject(parent) {}
    void highlight(const QString& text) { Q_UNUSED(text); }
    void setPattern(const QString& pattern) { _pattern = pattern; }
private:
    QString _pattern;
    QVector<QRegularExpression> _regexes;
};