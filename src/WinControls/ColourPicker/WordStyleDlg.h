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

#pragma once

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QFontComboBox>
#include <QLabel>
#include <QColorDialog>
#include <QRgb>

#include "../StaticDialog/StaticDialog.h"

// WordStyleDlg - Word style configuration dialog
class WordStyleDlg : public StaticDialog
{
    Q_OBJECT

public:
    WordStyleDlg();
    ~WordStyleDlg() override = default;

    void init(QWidget* parent);
    void destroy() override;
    void doDialog();

    void setValues(const QString& styleName,  bool isCaseSensitive, const QFont& font, QRgb fgColor, QRgb bgColor, int fontStyle);
    void getValues(QString* styleName, bool* isCaseSensitive, QFont* font, QRgb* fgColor, QRgb* bgColor, int* fontStyle);

signals:
    void styleApplied();

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam);

private slots:
    void onForegroundColorClicked();
    void onBackgroundColorClicked();

private:
    QString _styleName;
    bool _isCaseSensitive = true;
    QFont _font;
    QRgb _fgColor = qRgb(0, 0, 0);
    QRgb _bgColor = qRgb(255, 255, 255);
    int _fontStyle = 0;
    
    // Qt widgets
    QLineEdit* _pStyleNameEdit = nullptr;
    QCheckBox* _pCaseSensitiveCheck = nullptr;
    QFontComboBox* _pFontCombo = nullptr;
    QSpinBox* _pFontSizeSpin = nullptr;
    QPushButton* _pFgColorBtn = nullptr;
    QPushButton* _pBgColorBtn = nullptr;
    QComboBox* _pFontStyleCombo = nullptr;
    
    static const int FONTSTYLE_NORMAL = 0;
    static const int FONTSTYLE_BOLD = 1;
    static const int FONTSTYLE_ITALIC = 2;
    static const int FONTSTYLE_BOLDITALIC = 3;
};

namespace WordStyleFontStyles {
    enum {
        Normal = 0,
        Bold = 1,
        Italic = 2,
        BoldItalic = 3
    };
}