#include "FindCharsInRange.h"
#include <QVBoxLayout>

void FindCharsInRange::doDialog()
{
    if (!layout()) {
        auto* l = new QVBoxLayout(this);
        _charFromEdit = new QLineEdit(this);
        _charToEdit = new QLineEdit(this);
        _rangeSpin = new QSpinBox(this);
        l->addWidget(_charFromEdit);
        l->addWidget(_charToEdit);
        l->addWidget(_rangeSpin);
    }
    show();
}

void FindCharsInRange::findAllCharsIn(bool next)
{
    (void)next;
}

void FindCharsInRange::findNextChar()
{
}
