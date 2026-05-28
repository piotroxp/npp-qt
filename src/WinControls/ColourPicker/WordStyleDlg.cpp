// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "WordStyleDlg.h"
#include <QVBoxLayout>

// Win32 macro stubs for Linux Qt6 port
#ifndef WM_INITDIALOG
#define WM_INITDIALOG 0x0110
#endif
#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif
#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif
#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef IDOK
#define IDOK 1
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef IDYES
#define IDYES 6
#endif
#ifndef IDNO
#define IDNO 7
#endif
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QLineEdit>

WordStyleDlg::WordStyleDlg()
    : StaticDialog()
{
}

void WordStyleDlg::init(QWidget* parent)
{
    StaticDialog::init(parent);
}

void WordStyleDlg::destroy()
{
    // Clean up
}

void WordStyleDlg::doDialog()
{
    if (!isCreated()) {
        create(0);
        setWindowTitle("Word Style");
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Form layout for controls
        QFormLayout* formLayout = new QFormLayout();
        
        // Style name
        _pStyleNameEdit = new QLineEdit(this);
        formLayout->addRow("&Style Name:", _pStyleNameEdit);
        
        // Case sensitive checkbox
        _pCaseSensitiveCheck = new QCheckBox("Case Sensitive", this);
        formLayout->addRow("", _pCaseSensitiveCheck);
        
        // Font family
        _pFontCombo = new QFontComboBox(this);
        formLayout->addRow("&Font Family:", _pFontCombo);
        
        // Font size
        QHBoxLayout* sizeLayout = new QHBoxLayout();
        _pFontSizeSpin = new QSpinBox(this);
        _pFontSizeSpin->setRange(6, 72);
        _pFontSizeSpin->setValue(10);
        sizeLayout->addWidget(_pFontSizeSpin);
        sizeLayout->addStretch();
        formLayout->addRow("Font &Size:", sizeLayout);
        
        // Foreground color button
        _pFgColorBtn = new QPushButton("Choose Color...", this);
        connect(_pFgColorBtn, &QPushButton::clicked, this, &WordStyleDlg::onForegroundColorClicked);
        formLayout->addRow("&Foreground:", _pFgColorBtn);
        
        // Background color button
        _pBgColorBtn = new QPushButton("Choose Color...", this);
        _pBgColorBtn->setStyleSheet("background-color: white;");
        connect(_pBgColorBtn, &QPushButton::clicked, this, &WordStyleDlg::onBackgroundColorClicked);
        formLayout->addRow("&Background:", _pBgColorBtn);
        
        // Font style combo
        _pFontStyleCombo = new QComboBox(this);
        _pFontStyleCombo->addItems({"Normal", "Bold", "Italic", "Bold Italic"});
        formLayout->addRow("Font &Style:", _pFontStyleCombo);
        
        mainLayout->addLayout(formLayout);
        
        // Add some spacing
        mainLayout->addStretch();
        
        // Buttons
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        
        QPushButton* applyBtn = new QPushButton("&Apply", this);
        connect(applyBtn, &QPushButton::clicked, this, [this]() {
            getValues(&_styleName, &_isCaseSensitive, &_font, &_fgColor, &_bgColor, &_fontStyle);
            emit styleApplied();
            accept();
        });
        btnLayout->addWidget(applyBtn);
        
        QPushButton* okBtn = new QPushButton("&OK", this);
        connect(okBtn, &QPushButton::clicked, this, [this]() {
            getValues(&_styleName, &_isCaseSensitive, &_font, &_fgColor, &_bgColor, &_fontStyle);
            accept();
        });
        btnLayout->addWidget(okBtn);
        
        QPushButton* cancelBtn = new QPushButton("Cancel", this);
        connect(cancelBtn, &QPushButton::clicked, this, [this]() { reject(); });
        btnLayout->addWidget(cancelBtn);
        
        mainLayout->addLayout(btnLayout);
    }
    
    // Set current values
    _pStyleNameEdit->setText(_styleName);
    _pCaseSensitiveCheck->setChecked(_isCaseSensitive);
    _pFontCombo->setCurrentFont(_font);
    _pFontSizeSpin->setValue(_font.pointSize());
    _pFontStyleCombo->setCurrentIndex(_fontStyle);
    
    // Update color buttons
    _pFgColorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
        .arg(qRed(_fgColor)).arg(qGreen(_fgColor)).arg(qBlue(_fgColor)));
    _pBgColorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
        .arg(qRed(_bgColor)).arg(qGreen(_bgColor)).arg(qBlue(_bgColor)));
    
    display(true);
}

void WordStyleDlg::setValues(const QString& styleName, bool isCaseSensitive, const QFont& font, 
                       QRgb fgColor, QRgb bgColor, int fontStyle)
{
    _styleName = styleName;
    _isCaseSensitive = isCaseSensitive;
    _font = font;
    _fgColor = fgColor;
    _bgColor = bgColor;
    _fontStyle = fontStyle;
}

void WordStyleDlg::getValues(QString* styleName, bool* isCaseSensitive, QFont* font, 
                         QRgb* fgColor, QRgb* bgColor, int* fontStyle)
{
    if (styleName) *styleName = _pStyleNameEdit->text();
    if (isCaseSensitive) *isCaseSensitive = _pCaseSensitiveCheck->isChecked();
    if (font) {
        font->setFamily(_pFontCombo->currentFont().family());
        font->setPointSize(_pFontSizeSpin->value());
        *font = _font;  // Fixed: was *font = *_font (double dereference)
    }
    if (fgColor) *fgColor = _fgColor;
    if (bgColor) *bgColor = _bgColor;
    if (fontStyle) *fontStyle = _pFontStyleCombo->currentIndex();
}

void WordStyleDlg::onForegroundColorClicked()
{
    QColorDialog dialog(QColor::fromRgb(_fgColor), this);
    dialog.setWindowTitle("Choose Foreground Color");
    if (dialog.exec()) {
        _fgColor = dialog.currentColor().rgb();
        _pFgColorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
            .arg(qRed(_fgColor)).arg(qGreen(_fgColor)).arg(qBlue(_fgColor)));
    }
}

void WordStyleDlg::onBackgroundColorClicked()
{
    QColorDialog dialog(QColor::fromRgb(_bgColor), this);
    dialog.setWindowTitle("Choose Background Color");
    if (dialog.exec()) {
        _bgColor = dialog.currentColor().rgb();
        _pBgColorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
            .arg(qRed(_bgColor)).arg(qGreen(_bgColor)).arg(qBlue(_bgColor)));
    }
}

intptr_t WordStyleDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
            
        case WM_COMMAND:
            if (wParam == IDOK || wParam == IDCANCEL) {
                display(false);
                return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}