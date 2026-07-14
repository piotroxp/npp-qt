#pragma once
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMap>

struct CallTipInfo {
    QString functionName;
    QString signature;
    QString description;
    int currentParam = 0;
    QList<QString> paramNames;
};

class FunctionCallTip : public QFrame {
    Q_OBJECT
public:
    explicit FunctionCallTip(QWidget* parent = nullptr);
    ~FunctionCallTip();
    
    void setCallTip(const CallTipInfo& info);
    void setCurrentParameter(int index);
    void showTip(const QPoint& pos, const CallTipInfo& info);
    void hideTip();
    bool isVisible() const { return visible_; }
    
signals:
    void callTipClosed();
    void parameterHighlighted(int paramIndex);
    
private:
    void applyStyle();
    void highlightCurrentParam();
    
    QVBoxLayout* layout_ = nullptr;
    QLabel* signatureLabel_ = nullptr;
    QTextEdit* descEdit_ = nullptr;
    CallTipInfo currentInfo_;
    bool visible_ = false;
    bool highlighting_ = true;
    int maxWidth_ = 400;
};
