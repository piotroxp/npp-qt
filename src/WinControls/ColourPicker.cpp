// Semantic Lift: Win32 ColourPicker → Qt6 implementation
// Original: PowerEditor/src/WinControls/ColourPicker/*.h/*.cpp
// Target: npp-qt/src/WinControls/ColourPicker.h + .cpp

#include "ColourPicker.h"
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QDebug>

// =============================================================================
// ColourPopup
// =============================================================================

ColourPopup::ColourPopup(const QColor& defaultColor, QWidget* parent)
    : QDialog(parent, Qt::Popup | Qt::FramelessWindowHint)
    , _colour(defaultColor)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    setMouseTracking(true);
    setFixedSize(132, 113);

    // Find the global position of the parent (the colour picker button)
    if (parent) {
        QPoint globalPos = parent->mapToGlobal(QPoint(0, parent->height()));
        move(globalPos);
    }

    installEventFilter(this);
}

QRect ColourPopup::colorRect(int index) const
{
    // 6 columns × 8 rows
    int col = index % 6;
    int row = index / 6;
    int x = 7 + col * 19;
    int y = 7 + row * 10;
    return QRect(x, y, 16, 8);
}

QColor ColourPopup::colorAt(const QPoint& pt) const
{
    for (int i = 0; i < 48; ++i) {
        if (colorRect(i).contains(pt)) {
            return colourItemsData[i];
        }
    }
    return QColor();
}

void ColourPopup::selectColor(const QColor& color)
{
    _colour = color;
    emit colorSelected(color);
    hide();
}

void ColourPopup::showMoreColors()
{
    QColorDialog dlg(_colour, this);
    dlg.setOption(QColorDialog::ShowAlphaChannel, false);
    dlg.setWindowTitle(QStringLiteral("Choose Color"));

    if (dlg.exec() == QDialog::Accepted) {
        selectColor(dlg.selectedColor());
    }
}

bool ColourPopup::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        if (!_isMoreColorsShown) {
            QRect moreBtnRect(30, 92, 72, 14);
            if (moreBtnRect.contains(me->pos())) {
                _isMoreColorsShown = true;
                showMoreColors();
                return true;
            }

            QColor c = colorAt(me->pos());
            if (c.isValid()) {
                selectColor(c);
                return true;
            }

            if (!rect().contains(me->pos())) {
                hide();
                return true;
            }
        }
    }

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        int prevHover = _hoverIndex;
        _hoverIndex = -1;
        for (int i = 0; i < 48; ++i) {
            if (colorRect(i).contains(me->pos())) {
                _hoverIndex = i;
                break;
            }
        }
        if (prevHover != _hoverIndex)
            update();
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
    }

    if (event->type() == QEvent::FocusOut) {
        hide();
    }

    return false;
}

void ColourPopup::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Background
    p.fillRect(rect(), palette().color(QPalette::Window));

    // Draw each color swatch
    for (int i = 0; i < 48; ++i) {
        QRect r = colorRect(i).adjusted(0, 0, -1, -1);
        p.fillRect(r, colourItemsData[i]);

        // Border
        QPen borderPen(i == _hoverIndex ? Qt::darkGray : Qt::black);
        borderPen.setWidth(1);
        p.setPen(borderPen);
        p.drawRect(r);

        // Selection highlight
        if (i == _selectedIndex) {
            p.setPen(Qt::white);
            p.drawLine(r.topLeft(), r.bottomLeft());
            p.drawLine(r.topLeft(), r.topRight());
            p.setPen(Qt::darkGray);
            p.drawLine(r.bottomRight(), r.bottomLeft());
            p.drawLine(r.bottomRight(), r.topRight());
        }
    }

    // "More Colors..." button
    QRect moreBtnRect(30, 92, 72, 14);
    p.fillRect(moreBtnRect, palette().button().color());
    p.setPen(Qt::darkGray);
    p.drawRect(moreBtnRect.adjusted(0, 0, -1, -1));
    p.drawText(moreBtnRect, Qt::AlignCenter, QStringLiteral("More Colors..."));
}

void ColourPopup::mousePressEvent(QMouseEvent* event)
{
    QRect moreBtnRect(30, 92, 72, 14);
    if (moreBtnRect.contains(event->pos())) {
        showMoreColors();
        return;
    }

    QColor c = colorAt(event->pos());
    if (c.isValid()) {
        selectColor(c);
        return;
    }

    if (!rect().contains(event->pos())) {
        hide();
    }
}

// =============================================================================
// ColourPicker
// =============================================================================

ColourPicker::ColourPicker(QWidget* parent)
    : Window(parent)
{
    setFixedSize(25, 25);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TintedBackground, false);
}

void ColourPicker::init(QApplication* app, QWidget* parent)
{
    Window::init(app, parent);
    _hSelf = this;
}

void ColourPicker::destroy()
{
    destroyColorPopup();
    deleteLater();
}

void ColourPicker::destroyColorPopup()
{
    if (_pColourPopup) {
        _pColourPopup->hide();
        delete _pColourPopup;
        _pColourPopup = nullptr;
    }
}

void ColourPicker::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    QRect r = rect().adjusted(1, 1, -2, -2);

    // Background — the color swatch
    if (_isEnabled) {
        p.fillRect(r, _currentColour);
    } else {
        // Disabled: show hatching
        p.fillRect(r, Qt::lightGray);
        p.save();
        p.setPen(Qt::darkGray);
        // Diagonal hatch pattern
        for (int i = -r.height(); i < r.width(); i += 4) {
            p.drawLine(r.left() + i, r.top(), r.left(), r.top() + i);
        }
        p.restore();
    }

    // Border
    p.setPen(Qt::darkGray);
    p.drawRect(r.adjusted(0, 0, -1, -1));
}

void ColourPicker::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Single click: show popup on left click down (like Win32 BN_CLICKED)
        showColorPopup();
    } else if (event->button() == Qt::RightButton) {
        if (!_disableRightClick) {
            _isEnabled = !_isEnabled;
            emit rightClicked();
            update();
        }
    }
}

void ColourPicker::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        showColorPopup();
    }
}

void ColourPicker::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void ColourPicker::showColorPopup()
{
    destroyColorPopup();

    _pColourPopup = new ColourPopup(_currentColour, this);
    connect(_pColourPopup, &ColourPopup::colorSelected,
            this, [this](const QColor& color) {
                if (_currentColour != color) {
                    _currentColour = color;
                    emit colorPicked(color);
                    update();
                }
            });
    _pColourPopup->show();
}
