#pragma once

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>

class ToolTip : public QLabel
{
    Q_OBJECT
public:
    explicit ToolTip(QWidget* parent = nullptr);
    ~ToolTip() override;

    void init(QWidget* parent);
    void addToToolTip(void* hwnd, const QString& text);
    void delFromToolTip(void* hwnd);
    void showTooltip(const QString& text, int x, int y);
    void hideTooltip();

    static ToolTip* getInstance();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    static ToolTip* _pSelf;
    QWidget* _parentWidget = nullptr;
    QTimer* _hideTimer = nullptr;
};

