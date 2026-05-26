// StaticDialog.cpp — Qt6 translation of CDialog → QDialog
#include "StaticDialog.h"
#include <QScreen>
#include <QApplication>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QCloseEvent>

StaticDialog::StaticDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setSizeGripEnabled(true);
    setModal(false); // Can be modal or modeless
}

StaticDialog::~StaticDialog()
{
    if (isCreated())
    {
        // Prevent run_dlgProc from doing anything
        StaticDialog::destroy();
    }
}

void StaticDialog::create(int /*dialogId*/, bool isRTL, bool /*msgDestParent*/)
{
    if (isRTL)
        setLayoutDirection(Qt::RightToLeft);

    _isCreated = true;
    _rc = geometry();
}

void StaticDialog::destroy()
{
    close();
    _isCreated = false;
}

QRect StaticDialog::getMappedChildRect(QWidget* child) const
{
    if (!child)
        return QRect();

    QRect rect = child->rect();
    rect.translate(child->mapTo(this, QPoint(0, 0)));
    return rect;
}

QRect StaticDialog::getMappedChildRect(int childId) const
{
    QWidget* child = findChild<QWidget*>(QString("dlg_%1").arg(childId));
    return getMappedChildRect(child);
}

void StaticDialog::redrawDlgItem(int nIDDlgItem, bool forceUpdate) const
{
    QWidget* child = findChild<QWidget*>(QString("dlg_%1").arg(nIDDlgItem));
    if (child)
    {
        child->update();
        if (forceUpdate)
            child->repaint();
    }
}

void StaticDialog::goToCenter(unsigned /*swpFlags*/)
{
    QWidget* parentWidget = parentWidget();
    if (!parentWidget)
        parentWidget = QApplication::activeWindow();

    if (parentWidget)
    {
        QRect parentRect = parentWidget->geometry();
        QPoint center(parentRect.center());
        move(center.x() - width() / 2, center.y() - height() / 2);
    }
    else
    {
        // Center on screen
        QScreen* screen = QApplication::screenAt(QCursor::pos());
        if (screen)
        {
            QRect screenGeom = screen->geometry();
            move((screenGeom.width() - width()) / 2 + screenGeom.x(),
                 (screenGeom.height() - height()) / 2 + screenGeom.y());
        }
    }
}

bool StaticDialog::moveForDpiChange()
{
    // Check if parent DPI changed
    QWidget* parent = parentWidget();
    if (parent)
    {
        QScreen* oldScreen = screen();
        QScreen* newScreen = parent->screen();
        if (oldScreen != newScreen)
        {
            hide();
            goToCenter();
            show();
            return true;
        }
    }
    return false;
}

void StaticDialog::display(bool toShow) const
{
    if (toShow)
    {
        // Check multi-monitor bounds
        QRect geom = const_cast<StaticDialog*>(this)->geometry();
        QScreen* screen = QApplication::screenAt(geom.center());
        if (screen)
        {
            QRect workArea = screen->availableGeometry();
            int margin = 31; // caption + border

            int newLeft = geom.left();
            int newTop = geom.top();

            if (newLeft > screen->geometry().right() - margin)
                newLeft = workArea.right() - geom.width();
            if (newLeft + geom.width() < screen->geometry().left() + margin)
                newLeft = workArea.left();
            if (newTop > screen->geometry().bottom() - margin)
                newTop = workArea.bottom() - geom.height();
            if (newTop + geom.height() < screen->geometry().top() + margin)
                newTop = workArea.top();

            if (newLeft != geom.left() || newTop != geom.top())
                const_cast<StaticDialog*>(this)->move(newLeft, newTop);
        }
        show();
    }
    else
    {
        hide();
    }
}

void StaticDialog::displayEnhanced(bool toShow)
{
    if (toShow)
    {
        QRect testPositionRc = geometry();
        QRect candidateRc = getViewablePositionRect(testPositionRc);
        if (testPositionRc.left() != candidateRc.left() ||
            testPositionRc.top() != candidateRc.top())
        {
            setGeometry(candidateRc);
        }
    }
    display(toShow);
}

QRect StaticDialog::getViewablePositionRect(const QRect& testPositionRc) const
{
    QScreen* screen = QApplication::screenAt(testPositionRc.center());
    if (!screen)
        return testPositionRc;

    QRect workArea = screen->availableGeometry();

    // Check if title bar is visible
    int margin = 31;
    if (testPositionRc.top() >= workArea.top() &&
        testPositionRc.top() + margin <= workArea.bottom() &&
        testPositionRc.right() - 2 * margin > workArea.left() &&
        testPositionRc.left() + 2 * margin < workArea.right())
    {
        return testPositionRc; // Already visible
    }

    // Reposition to be viewable
    QRect result = testPositionRc;

    int rectWidth = result.width();
    int rectHeight = result.height();
    int screenWidth = workArea.width();
    int screenHeight = workArea.height();

    result.moveLeft(workArea.left() + qMax(0, (screenWidth - rectWidth) / 2));
    result.moveTop(workArea.top() + qMax(0, (screenHeight - rectHeight) / 2));

    return result;
}

QPoint StaticDialog::getTopPoint(QWidget* widget, bool isLeft) const
{
    if (!widget)
        return QPoint();

    QPoint pt = widget->mapToGlobal(QPoint(0, 0));
    return QPoint(isLeft ? pt.x() : pt.x() + widget->width(), pt.y());
}

bool StaticDialog::isCheckedOrNot(int checkControlID) const
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(
        findChild<QObject*>(QString("dlg_%1").arg(checkControlID)));
    return btn && btn->isChecked();
}

void StaticDialog::setChecked(int checkControlID, bool checkOrNot) const
{
    QAbstractButton* btn = qobject_cast<QAbstractButton*>(
        findChild<QObject*>(QString("dlg_%1").arg(checkControlID)));
    if (btn)
        btn->setChecked(checkOrNot);
}

void StaticDialog::setPositionDpi(WPARAM /*wParam*/, LPARAM lParam)
{
    QRect rc;
    rc.setCoords(
        LOWORD(lParam) & 0xFFFF,
        (LOWORD(lParam) >> 16) & 0xFFFF,
        HIWORD(lParam) & 0xFFFF,
        (HIWORD(lParam) >> 16) & 0xFFFF
    );
    // Would need platform-specific LOWORD/HIWORD - simplified here
    Q_UNUSED(rc);
}

bool StaticDialog::event(QEvent* event)
{
    return QDialog::event(event);
}

void StaticDialog::showEvent(QShowEvent* event)
{
    _rc = geometry();
    QDialog::showEvent(event);
}

void StaticDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);
}

void StaticDialog::resizeEvent(QResizeEvent* event)
{
    _rc = geometry();
    QDialog::resizeEvent(event);
}
