// GoToLineDlg.cpp - Qt port
#include "GoToLineDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
GoToLineDlg::GoToLineDlg() : QDialog() {}
void GoToLineDlg::doDialog(bool isRTL) {
    Q_UNUSED(isRTL);
    if (!isVisible()) {
        setWindowTitle("Go to Line");
        auto* vLayout = new QVBoxLayout(this);
        auto* label = new QLabel("Line number:", this);
        _lineEdit = new QLineEdit(this);
        _lineEdit->setMaxLength(256);
        auto* hLayout = new QHBoxLayout();
        hLayout->addWidget(label);
        hLayout->addWidget(_lineEdit);
        vLayout->addLayout(hLayout);
        _maxLabel = new QLabel(this);
        vLayout->addWidget(_maxLabel);
        auto* okBtn = new QPushButton("OK", this);
        auto* cancelBtn = new QPushButton("Cancel", this);
        connect(okBtn, &QPushButton::clicked, this, [this]() { accept(); });
        connect(cancelBtn, &QPushButton::clicked, this, [this]() { reject(); });
        hLayout = new QHBoxLayout();
        hLayout->addStretch();
        hLayout->addWidget(okBtn);
        hLayout->addWidget(cancelBtn);
        vLayout->addLayout(hLayout);
    }
    updateLinesNumbers();
    exec();
}
void GoToLineDlg::display(bool toShow) const {
    if (toShow) { updateLinesNumbers(); _lineEdit->setFocus(); }
    else { _lineEdit->clear(); }
}
void GoToLineDlg::updateLinesNumbers() const {
    if (_ppEditView && *_ppEditView && _maxLabel) {
        int lineCount = (*_ppEditView)->execute(SCI_GETLINECOUNT);
        _maxLabel->setText(QString("Maximum line: %1").arg(lineCount));
    }
}
intptr_t GoToLineDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
    Q_UNUSED(message); Q_UNUSED(wParam); Q_UNUSED(lParam);
    return 0;
}
