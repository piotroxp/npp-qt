// FunctionCallTip.h - Qt6 stub
#pragma once
#include <QString>
#include <QVector>

class FunctionCallTip {
public:
    FunctionCallTip(void* editView = nullptr) : _editView(editView) {}
    void show(const QString& tip, int startPos, int lineHeight) {
        Q_UNUSED(tip); Q_UNUSED(startPos); Q_UNUSED(lineHeight);
    }
    void showUTF8(const char* tip, int startPos, int lineHeight) {
        Q_UNUSED(tip); Q_UNUSED(startPos); Q_UNUSED(lineHeight);
    }
    void cancel() {}
    void setHighlight(const char* start, const char* end) { Q_UNUSED(start); Q_UNUSED(end); }
    int pos();
    bool isVisible() const { return false; }
private:
    void* _editView = nullptr;
};