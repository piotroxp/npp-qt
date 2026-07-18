// StatusBar.h - Qt6 status bar
// INTENT: source uses native Win32 status bar. Target uses QStatusBar.
#pragma once
#include <QStatusBar>
#include <QString>
#include <QLabel>

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override = default;

    void setEncoding(const QString& enc);
    void setEol(const QString& eol);
    void setPosition(int line, int col);
    void setLength(int len);
    void setInfo(const QString& info);
    void setReadOnly(bool ro);
    void setModified(bool modified);

private:
    QLabel* _encodingLabel = nullptr;
    QLabel* _eolLabel = nullptr;
    QLabel* _positionLabel = nullptr;
    QLabel* _lengthLabel = nullptr;
    QLabel* _infoLabel = nullptr;
};
