// ColourPopup.cpp — Popup colour-grid widget implementation
// Converts: PowerEditor/src/WinControls/ColourPicker/ColourPopup.{h,cpp}
// Win32 → Qt6: IDD_COLOUR_POPUP dialog → QDialog, WM_DRAWITEM → paintEvent,
//              ChooseColor → QColorDialog, LB_ADDSTRING → QGridLayout
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ColourPopup.h"
#include "ColourPicker.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QFrame>
#include <QApplication>
#include <QScreen>
#include <QColorDialog>

// 48-colour palette matching N++ (from ColourPopup.cpp upstream)
const QVector<QColor>& ColourPopup::palette()
{
    static const QVector<QColor> cols = {
        QColor(  0,   0,   0), QColor( 64,   0,   0), QColor(128,   0,   0), QColor(128,  64,  64),
        QColor(255,   0,   0), QColor(255, 128, 128), QColor(255, 255, 128), QColor(255, 255,   0),
        QColor(255, 128,  64), QColor(255, 128,   0), QColor(128,  64,   0), QColor(128, 128,   0),
        QColor(128, 128,  64), QColor(  0,  64,   0), QColor(  0, 128,   0), QColor(  0, 255,   0),
        QColor(128, 255,   0), QColor(128, 255, 128), QColor(  0, 255, 128), QColor(  0, 255,  64),
        QColor(  0, 128, 128), QColor(  0, 128,  64), QColor(  0,  64,  64), QColor(128, 128, 128),
        QColor( 64, 128, 128), QColor(  0,   0, 128), QColor(  0,   0, 255), QColor(  0,  64, 128),
        QColor(  0, 255, 255), QColor(128, 255, 255), QColor(  0, 128, 255), QColor(  0, 128, 192),
        QColor(128, 128, 255), QColor(  0,   0, 160), QColor(  0,   0,  64), QColor(192, 192, 192),
        QColor( 64,   0,  64), QColor( 64,   0,  64), QColor(128,   0, 128), QColor(128,   0,  64),
        QColor(128, 128, 192), QColor(255, 128, 192), QColor(255, 128, 255), QColor(255,   0, 255),
        QColor(255,   0, 128), QColor(128,   0, 255), QColor( 64,   0, 128), QColor(255, 255, 255),
    };
    return cols;
}

ColourPopup::ColourPopup(const QColor& initialColour, QWidget* parent)
    : QDialog(parent, Qt::Popup)
    , _currentColour(initialColour)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);

    // Watch for clicks outside the popup so we can close
    qApp->installEventFilter(this);

    buildColourGrid();
    resize(COLS * (SWATCH_SIZE + 4) + 20, ROWS * (SWATCH_SIZE + 4) + 60);
}

ColourPopup::~ColourPopup()
{
    qApp->removeEventFilter(this);
}

void ColourPopup::popupAt(const QPoint& pos, QWidget* anchor)
{
    _anchorWidget = anchor;

    // Adjust so the popup stays on screen
    QScreen* screen = QApplication::screenAt(pos);
    if (!screen)
        screen = QApplication::primaryScreen();
    QRect screenGeo = screen->availableGeometry();

    int cx = pos.x() - width() / 2;
    int cy = pos.y();
    if (cx + width() > screenGeo.right())
        cx = screenGeo.right() - width();
    if (cx < screenGeo.left())
        cx = screenGeo.left();
    if (cy + height() > screenGeo.bottom())
        cy = pos.y() - anchor->height() - height();

    move(cx, cy);
    show();
    raise();
    activateWindow();
}

void ColourPopup::buildColourGrid()
{
    // ── Main layout ────────────────────────────────────────────────────
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(2);

    // ── Colour grid ───────────────────────────────────────────────────
    const QVector<QColor>& cols = palette();
    auto* grid = new QGridLayout();
    grid->setSpacing(2);
    grid->setContentsMargins(0, 0, 0, 0);

    int idx = 0;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (idx >= cols.size())
                break;
            QColor col = cols[idx];

            auto* swatch = new QWidget(this);
            swatch->setFixedSize(SWATCH_SIZE, SWATCH_SIZE);
            swatch->setMouseTracking(true);
            swatch->setCursor(Qt::PointingHandCursor);
            swatch->setObjectName(QString("swatch_%1").arg(idx));
            swatch->installEventFilter(this);

            // Store colour on the widget via Qt property
            swatch->setProperty("colour", col);
            _swatches.append(swatch);
            _swatchColors.append(col);

            grid->addWidget(swatch, r, c);
            ++idx;
        }
    }
    mainLayout->addLayout(grid);

    // ── "More Colours…" button (opens QColorDialog) ───────────────────
    auto* btnMore = new QPushButton("More Colours…", this);
    btnMore->setFocusPolicy(Qt::NoFocus);
    connect(btnMore, &QPushButton::clicked, this, &ColourPopup::onOkClicked);
    mainLayout->addWidget(btnMore);

    // Style: flat, no frame
    setStyleSheet(R"(
        QDialog#ColourPopup {
            background: palette(window);
            border: 1px solid palette(shadow);
        }
        QPushButton {
            border: 1px solid palette(mid);
            border-radius: 2px;
            padding: 2px 8px;
            background: palette(button);
        }
        QPushButton:hover {
            background: palette(light);
        }
    )");
}

bool ColourPopup::eventFilter(QObject* watched, QEvent* ev)
{
    // ── Close if user clicks outside ──────────────────────────────────
    if (ev->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(ev);
        if (me->button() == Qt::LeftButton) {
            QWidget* w = qApp->widgetAt(me->globalPosition().toPoint());
            // Close if click is not inside our popup and not on the anchor
            if (!isAncestorOf(w) && w != _anchorWidget) {
                close();
                return true;
            }
        }
    }
    return QDialog::eventFilter(watched, ev);
}

void ColourPopup::mouseMoveEvent(QMouseEvent* ev)
{
    // Highlight the swatch under the cursor
    QWidget* under = childAt(ev->pos());
    if (_hoveredSwatch != under && _hoveredSwatch) {
        _hoveredSwatch->update();
    }
    _hoveredSwatch = qobject_cast<QWidget*>(under);
    if (_hoveredSwatch) {
        _hoveredSwatch->update();
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QDialog::mouseMoveEvent(ev);
}

void ColourPopup::leaveEvent(QEvent*)
{
    if (_hoveredSwatch) {
        _hoveredSwatch->update();
        _hoveredSwatch = nullptr;
    }
}

void ColourPopup::closeEvent(QCloseEvent* ev)
{
    // Release event filter before close
    qApp->removeEventFilter(this);
    QDialog::closeEvent(ev);
}

void ColourPopup::onColourClicked(int index)
{
    if (index < 0 || index >= _swatchColors.size())
        return;
    _currentColour = _swatchColors[index];
    if (_callback)
        _callback(_currentColour);
    emit colourSelected(_currentColour);
    close();
}

void ColourPopup::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() != Qt::LeftButton) {
        QDialog::mousePressEvent(ev);
        return;
    }
    // Find which swatch was clicked
    QWidget* w = childAt(ev->pos());
    if (!w)
        return;
    // Walk up to find a swatch
    while (w && w != this && !w->property("colour").isValid())
        w = w->parentWidget();
    if (w && w->property("colour").isValid()) {
        int idx = _swatches.indexOf(w);
        onColourClicked(idx);
    } else {
        QDialog::mousePressEvent(ev);
    }
}

void ColourPopup::onOkClicked()
{
    QColorDialog dlg(_currentColour, this);
    dlg.setWindowTitle("Choose Colour");
    dlg.setOption(QColorDialog::ShowAlphaChannel, false);
    dlg.setOption(QColorDialog::NoButtons, false);
    if (dlg.exec() == QDialog::Accepted) {
        _currentColour = dlg.selectedColor();
        if (_currentColour.isValid()) {
            if (_callback)
                _callback(_currentColour);
            emit colourSelected(_currentColour);
        }
    }
    close();
}
