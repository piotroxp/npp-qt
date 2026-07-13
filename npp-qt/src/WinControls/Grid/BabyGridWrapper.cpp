// Semantic Lift: Win32 BabyGridWrapper → Qt6 BabyGridWrapper
// Original: PowerEditor/src/WinControls/Grid/BabyGridWrapper.cpp
// Target: npp-qt/src/WinControls/Grid/BabyGridWrapper.cpp

#include "BabyGridWrapper.h"
#include <QVBoxLayout>
#include <QApplication>

BabyGridWrapper::BabyGridWrapper(QWidget* parent)
    : QWidget(parent)
{
    // BabyGrid will be created in init()
}

void BabyGridWrapper::init(QApplication* app, QWidget* parent, int id)
{
    Q_UNUSED(app);
    Q_UNUSED(id);

    // Create BabyGrid as child widget
    _grid = new BabyGrid(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_grid);
    setLayout(layout);

    // Forward cell click signals
    connect(_grid, &BabyGrid::cellClicked,
            this, &BabyGridWrapper::cellClicked);
    connect(_grid, &BabyGrid::cellDoubleClicked,
            this, &BabyGridWrapper::cellDoubleClicked);
}
