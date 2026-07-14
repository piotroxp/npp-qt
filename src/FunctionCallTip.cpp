#include "FunctionCallTip.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStyleOption>
#include <QKeyEvent>
#include <QEnterEvent>
#include <QTimer>

FunctionCallTip::FunctionCallTip(QWidget* parent)
    : QFrame(parent)
    , visible_(false)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_TranslucentBackground, false);
    
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(8, 6, 8, 6);
    layout_->setSpacing(4);
    
    signatureLabel_ = new QLabel(this);
    signatureLabel_->setTextFormat(Qt::RichText);
    signatureLabel_->setWordWrap(true);
    signatureLabel_->setTextInteractionFlags(Qt::NoTextInteraction);
    
    descEdit_ = new QTextEdit(this);
    descEdit_->setReadOnly(true);
    descEdit_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    descEdit_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    descEdit_->setFrameShape(QFrame::NoFrame);
    descEdit_->setBackgroundRoles(QPalette::Window);
    descEdit_->document()->setMaximumBlockCount(20);
    
    layout_->addWidget(signatureLabel_);
    layout_->addWidget(descEdit_);
    
    applyStyle();
    
    setMinimumWidth(200);
    setMaximumWidth(maxWidth_);
    setMaximumHeight(300);
    
    setFocusPolicy(Qt::NoFocus);
}

FunctionCallTip::~FunctionCallTip() {
}

void FunctionCallTip::applyStyle() {
    QString style = R"(
        QLabel {
            color: #e0e0e0;
            font-size: 13px;
            font-family: "Segoe UI", "Consolas", monospace;
            padding: 2px 0;
        }
        QTextEdit {
            background-color: transparent;
            border: none;
            color: #b0b0b0;
            font-size: 12px;
            font-family: "Segoe UI", "Consolas", monospace;
            padding: 4px 0;
        }
        QTextEdit QScrollBar:vertical {
            background: #3a3a3a;
            width: 8px;
            border-radius: 4px;
        }
        QTextEdit QScrollBar::handle:vertical {
            background: #606060;
            border-radius: 4px;
            min-height: 20px;
        }
        QTextEdit QScrollBar::handle:horizontal {
            background: #606060;
            border-radius: 4px;
            min-width: 20px;
        }
    )";
    signatureLabel_->setStyleSheet(style);
    descEdit_->setStyleSheet(style);
    
    setStyleSheet(R"(
        QFrame {
            background-color: #2b2b2b;
            border: 1px solid #404040;
            border-radius: 4px;
        }
    )");
    
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);
}

void FunctionCallTip::setCallTip(const CallTipInfo& info) {
    currentInfo_ = info;
    
    QString funcNameHtml = QString("<b style="color: #7ec8e3;">%1</b>").arg(info.functionName);
    
    QString paramsHtml;
    QStringList params = info.signature.split('(', Qt::SkipEmptyParts);
    if (params.size() > 1) {
        QString paramStr = params.at(1).remove(')');
        QStringList paramList = parseParameters(paramStr);
        
        for (int i = 0; i < paramList.size(); ++i) {
            if (i > 0) paramsHtml += ", ";
            
            if (i == info.currentParam && highlighting_) {
                paramsHtml += QString("<span style="background-color: #3a3a2a; color: #ffcc00;">%1</span>")
                              .arg(Qt::escape(paramList.at(i)));
            } else {
                paramsHtml += Qt::escape(paramList.at(i));
            }
        }
    } else {
        paramsHtml = Qt::escape(info.signature);
    }
    
    signatureLabel_->setText(funcNameHtml + " (" + paramsHtml + ")");
    
    if (!info.description.isEmpty()) {
        descEdit_->setHtml(formatDescription(info.description));
        descEdit_->setVisible(true);
    } else {
        descEdit_->setVisible(false);
    }
    
    adjustSize();
    updateGeometry();
}

QStringList FunctionCallTip::parseParameters(const QString& paramStr) {
    QStringList result;
    QString current;
    int depth = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (int i = 0; i < paramStr.size(); ++i) {
        QChar c = paramStr.at(i);
        
        if (!inString && (c == '\'' || c == '"')) {
            inString = true;
            stringChar = c.toLatin1();
            current += c;
        } else if (inString && c == stringChar) {
            inString = false;
            stringChar = 0;
            current += c;
        } else if (!inString && c == '<') {
            depth++;
            current += c;
        } else if (!inString && c == '>') {
            depth--;
            current += c;
        } else if (!inString && depth == 0 && c == ',') {
            result.append(current.trimmed());
            current.clear();
        } else {
            current += c;
        }
    }
    
    if (!current.trimmed().isEmpty()) {
        result.append(current.trimmed());
    }
    
    return result;
}

QString FunctionCallTip::formatDescription(const QString& desc) {
    QString html = desc;
    html = html.toHtmlEscaped();
    html.replace("\n\n", "</p><p>");
    html.replace("\n", "<br/>");
    html = "<p style="margin: 0; line-height: 1.4;">" + html + "</p>";
    
    QRegExp paramRegex("\\b(param|parameter|arg|argument)\\s+(\\w+)\\b", Qt::CaseInsensitive);
    html.replace(paramRegex, "<span style="color: #ffcc00;">\\1</span> <span style="color: #7ec8e3;">\\2</span>");
    
    QRegExp typeRegex("\\b(int|void|bool|float|double|string|char|QString|QList|QMap|std::\\w+)\\b");
    html.replace(typeRegex, "<span style="color: #98c379;">\\1</span>");
    
    return html;
}

void FunctionCallTip::highlightCurrentParam() {
    if (!currentInfo_.paramNames.isEmpty()) {
        emit parameterHighlighted(currentInfo_.currentParam);
    }
}

void FunctionCallTip::setCurrentParameter(int index) {
    if (index >= 0 && index < currentInfo_.paramNames.size()) {
        currentInfo_.currentParam = index;
        setCallTip(currentInfo_);
        highlightCurrentParam();
    }
}

void FunctionCallTip::showTip(const QPoint& pos, const CallTipInfo& info) {
    setCallTip(info);
    
    QPoint adjustedPos = pos;
    
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeom = screen->availableGeometry();
        
        int tipWidth = width();
        int tipHeight = height();
        
        if (adjustedPos.x() + tipWidth > screenGeom.right() - 10) {
            adjustedPos.setX(screenGeom.right() - tipWidth - 10);
        }
        
        if (adjustedPos.y() + tipHeight > screenGeom.bottom() - 10) {
            adjustedPos.setY(pos.y() - tipHeight - 20);
        }
        
        if (adjustedPos.x() < screenGeom.left() + 10) {
            adjustedPos.setX(screenGeom.left() + 10);
        }
        if (adjustedPos.y() < screenGeom.top() + 10) {
            adjustedPos.setY(screenGeom.top() + 10);
        }
    }
    
    move(adjustedPos);
    
    visible_ = true;
    show();
    
    setWindowOpacity(0);
    auto* fadeIn = new QPropertyAnimation(this, "windowOpacity", this);
    fadeIn->setDuration(100);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOut);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void FunctionCallTip::hideTip() {
    visible_ = false;
    hide();
    emit callTipClosed();
}

void FunctionCallTip::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hideTip();
        event->accept();
    } else {
        QFrame::keyPressEvent(event);
    }
}

void FunctionCallTip::leaveEvent(QEvent* event) {
    QFrame::leaveEvent(event);
}

bool FunctionCallTip::event(QEvent* event) {
    if (event->type() == QEvent::Leave) {
        QTimer::singleShot(200, this, [this]() {
            if (!underMouse()) {
                hideTip();
            }
        });
    }
    return QFrame::event(event);
}
