// ToolTip.h - Qt6 tooltip widget
// INTENT: source uses CToolTipCtrl. Target uses QLabel with Qt::ToolTip lifetime behavior.
#pragma once
#include <QWidget>
#include <QString>
#include <QPoint>

class ToolTip : public QWidget {
    Q_OBJECT
public:
    static ToolTip& instance();

    void showTip(const QPoint& pos, const QString& text);
    void hideTip();

    static void showText(const QPoint& pos, const QString& text, QWidget* w);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    ToolTip();
    ~ToolTip();
    void reposition(const QPoint& pos);
};
