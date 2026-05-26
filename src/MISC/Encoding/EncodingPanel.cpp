// MISC/Encoding/EncodingPanel.cpp - Qt6 port of encoding selection panel
#include "EncodingPanel.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QMessageBox>

EncodingPanel::EncodingPanel(QWidget* parent)
    : QDialog(parent)
    , _selectedEncoding(65001) // UTF-8
{
    setWindowTitle("Encoding Settings");
    setMinimumSize(500, 400);
    
    auto* mainLayout = new QVBoxLayout(this);
    
    // Encoding selection
    auto* encodingGroup = new QGroupBox("Character Encoding", this);
    auto* encodingLayout = new QVBoxLayout(encodingGroup);
    
    auto* encLabel = new QLabel("Select encoding:", this);
    _encodingCombo = new QComboBox(this);
    populateEncodings();
    encodingLayout->addWidget(encLabel);
    encodingLayout->addWidget(_encodingCombo);
    
    mainLayout->addWidget(encodingGroup);
    
    // EOL mode selection
    auto* eolGroup = new QGroupBox("Line Ending", this);
    auto* eolLayout = new QVBoxLayout(eolGroup);
    
    auto* eolLabel = new QLabel("Select line ending:", this);
    _eolCombo = new QComboBox(this);
    populateEolModes();
    eolLayout->addWidget(eolLabel);
    eolLayout->addWidget(_eolCombo);
    
    mainLayout->addWidget(eolGroup);
    
    // Preview
    auto* previewGroup = new QGroupBox("Preview", this);
    auto* previewLayout = new QVBoxLayout(previewGroup);
    
    _previewLabel = new QLabel("Sample text to show encoding preview", this);
    _previewLabel->setWordWrap(true);
    previewLayout->addWidget(_previewLabel);
    
    mainLayout->addWidget(previewGroup);
    
    // Buttons
    auto* btnLayout = new QHBoxLayout;
    auto* okBtn = new QPushButton("OK", this);
    auto* cancelBtn = new QPushButton("Cancel", this);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(btnLayout);
    
    // Connections
    connect(_encodingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &EncodingPanel::onEncodingChanged);
    connect(okBtn, &QPushButton::clicked, this, &EncodingPanel::onOkClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &EncodingPanel::onCancelClicked);
}

void EncodingPanel::populateEncodings()
{
    _encodingCombo->addItem("UTF-8", 65001);
    _encodingCombo->addItem("UTF-8 BOM", 65001);
    _encodingCombo->addItem("UTF-16 LE", 1200);
    _encodingCombo->addItem("UTF-16 BE", 1201);
    _encodingCombo->addItem("Windows-1252 (Western)", 1252);
    _encodingCombo->addItem("Windows-1250 (Central European)", 1250);
    _encodingCombo->addItem("Windows-1251 (Cyrillic)", 1251);
    _encodingCombo->addItem("Windows-1253 (Greek)", 1253);
    _encodingCombo->addItem("Windows-1254 (Turkish)", 1254);
    _encodingCombo->addItem("Windows-1255 (Hebrew)", 1255);
    _encodingCombo->addItem("Windows-1256 (Arabic)", 1256);
    _encodingCombo->addItem("Windows-1257 (Baltic)", 1257);
    _encodingCombo->addItem("Windows-1258 (Vietnamese)", 1258);
    _encodingCombo->addItem("ASCII", 20127);
    _encodingCombo->addItem("GB2312 (Simplified Chinese)", 936);
    _encodingCombo->addItem("BIG5 (Traditional Chinese)", 950);
    _encodingCombo->addItem("Shift-JIS (Japanese)", 932);
    _encodingCombo->addItem("EUC-KR (Korean)", 949);
    _encodingCombo->addItem("ISO-8859-1 (Latin 1)", 28591);
    _encodingCombo->addItem("ISO-8859-15 (Latin 9)", 28605);
}

void EncodingPanel::populateEolModes()
{
    _eolCombo->addItem("Windows (CRLF)", static_cast<int>(EolMode::Windows));
    _eolCombo->addItem("Unix (LF)", static_cast<int>(EolMode::Unix));
    _eolCombo->addItem("Mac (CR)", static_cast<int>(EolMode::Mac));
}

void EncodingPanel::setCurrentEncoding(int codepage)
{
    for (int i = 0; i < _encodingCombo->count(); ++i) {
        if (_encodingCombo->itemData(i).toInt() == codepage) {
            _encodingCombo->setCurrentIndex(i);
            break;
        }
    }
}

void EncodingPanel::onEncodingChanged(int index)
{
    Q_UNUSED(index);
    // Update preview based on selected encoding
}

void EncodingPanel::onOkClicked()
{
    _selectedEncoding = _encodingCombo->currentData().toInt();
    emit encodingSelected(_selectedEncoding);
    accept();
}

void EncodingPanel::onCancelClicked()
{
    reject();
}