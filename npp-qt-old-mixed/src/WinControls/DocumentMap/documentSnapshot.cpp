// Semantic Lift: documentSnapshot — Win32 snapshot of document view → Qt6 QPixmap cache
// Original: PowerEditor/src/WinControls/DocumentMap/documentSnapshot.cpp (196 lines)

#include "documentSnapshot.h"
#include <QPainter>
#include <QScrollBar>

DocumentSnapshot::DocumentSnapshot()
    : _snapshotLabel(new QLabel())
    , _isVisible(false)
{
}

DocumentSnapshot::~DocumentSnapshot()
{
}

void DocumentSnapshot::init(QWidget* parent, ScintillaEdit* pEditView)
{
    _parent = parent;
    _pEditView = pEditView;
    _snapshotLabel->setParent(parent);
}

void DocumentSnapshot::showSnapshot()
{
    if (!_pEditView)
        return;

    // Capture current view
    QPixmap pixmap = _pEditView->grab();
    _snapshotLabel->setPixmap(pixmap.scaled(200, 100, Qt::KeepAspectRatio));
    _snapshotLabel->show();
    _isVisible = true;
}

void DocumentSnapshot::hideSnapshot()
{
    _snapshotLabel->hide();
    _isVisible = false;
}

void DocumentSnapshot::move(int x, int y)
{
    _snapshotLabel->move(x, y);
}

void DocumentSnapshot::setVisible(bool visible)
{
    if (visible)
        showSnapshot();
    else
        hideSnapshot();
}

bool DocumentSnapshot::isVisible() const
{
    return _isVisible;
}
