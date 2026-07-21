// ToolTip.cpp
#include "ToolTip.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QTimer>
#include <QStyle>
#include <QScreen>

ToolTip::ToolTip()
    : QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint) {
    setAttribute(Qt::WA_TranslucentBackground);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    auto* label = new QLabel(this);
    label->setWordWrap(true);
    layout->addWidget(label);
    setStyleSheet("ToolTip { background: #ffffd0; border: 1px solid #808080; color: black; }");
}

ToolTip& ToolTip::instance() {
    static ToolTip inst;
    return inst;
}

void ToolTip::showTip(const QPoint& pos, const QString& text) {
    auto* label = findChild<QLabel*>();
    if (label) label->setText(text);
    reposition(pos);
    show();
}

void ToolTip::hideTip() {
    hide();
}

void ToolTip::reposition(const QPoint& pos) {
    QSize sh = sizeHint();
    QRect screen;
    if (QScreen* s = QApplication::screenAt(pos)) {
        screen = s->availableGeometry();
    } else {
        screen = QApplication::primaryScreen()->availableGeometry();
    }
    int x = pos.x();
    int y = pos.y() + 24;
    if (x + sh.width() > screen.right())
        x = screen.right() - sh.width();
    if (y + sh.height() > screen.bottom())
        y = pos.y() - sh.height();
    move(x, y);
}

void ToolTip::showText(const QPoint& pos, const QString& text, QWidget* w) {
    Q_UNUSED(w);
    instance().showTip(pos, text);
}
