// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Stub implementation of UserDefineDialog (full port pending)
// Full Win32→Qt6 port requires:
//   - UDL XML serialization/deserialization
//   - Scintilla lexer override for UDL
//   - Keywords management across 8 keyword lists
//   - Operator/folder/comment character configuration
//   - Real-time preview in Scintilla view
#include "UserDefineDialog.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include "NppDarkMode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

UserDefineDialog UserDefineDialog::_self;

UserDefineDialog::UserDefineDialog() : QWidget()
{
    setWindowTitle("User Defined Language");
    setMinimumSize(600, 500);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    _tab = new QTabWidget(this);
    mainLayout->addWidget(_tab);
    setupKeywordsTab();
    setupOperatorsTab();
    setupFoldersTab();
    setupCommentTab();
    setupStylingTab();

    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* okBtn = new QPushButton("OK", this);
    QPushButton* applyBtn = new QPushButton("Apply", this);
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(okBtn, &QPushButton::clicked, this, &UserDefineDialog::onOk);
    connect(applyBtn, &QPushButton::clicked, this, &UserDefineDialog::onApply);
    connect(cancelBtn, &QPushButton::clicked, this, &UserDefineDialog::onCancel);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(applyBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    NppDarkMode::instance().applyToWidget(this);
}

UserDefineDialog::~UserDefineDialog() = default;

void UserDefineDialog::init(QWidget* parent)
{
    Q_UNUSED(parent);
}

void UserDefineDialog::doDialog(bool willBeShown, bool isRTL)
{
    _isRTL = isRTL;
    if (willBeShown) show(); else hide();
}

void UserDefineDialog::doUserDefineDlg(bool willBeShown, bool isRTL)
{
    _self.doDialog(willBeShown, isRTL);
}

void UserDefineDialog::setupKeywordsTab()
{
    QWidget* tab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(tab);
    _extEdit = new QLineEdit(tab);
    fl->addRow("Extension:", _extEdit);
    for (int i = 0; i < 8; ++i) {
        QTextEdit* te = new QTextEdit(tab);
        te->setPlaceholderText(QString("Keyword list %1 (space-separated)").arg(i + 1));
        fl->addRow(QString("Words%1:").arg(i), te);
        _keywordsEdits[i] = te;
    }
    _tab->addTab(tab, "Keywords");
}

void UserDefineDialog::setupOperatorsTab()
{
    QWidget* tab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(tab);
    QLineEdit* opEdit = new QLineEdit(tab);
    opEdit->setPlaceholderText("Operators (e.g. + - * / = < > ! & |)");
    fl->addRow("Operators:", opEdit);
    _tab->addTab(tab, "Operators & Delimiters");
}

void UserDefineDialog::setupFoldersTab()
{
    QWidget* tab = new QWidget(this);
    QVBoxLayout* vl = new QVBoxLayout(tab);
    vl->addWidget(new QLabel("Folder/Code-folding markers (open/close pairs):", tab));
    QFormLayout* fl = new QFormLayout;
    QLineEdit* o1 = new QLineEdit(tab); o1->setPlaceholderText("{");
    QLineEdit* c1 = new QLineEdit(tab); c1->setPlaceholderText("}");
    QLineEdit* o2 = new QLineEdit(tab); o2->setPlaceholderText("/*");
    QLineEdit* c2 = new QLineEdit(tab); c2->setPlaceholderText("*/");
    fl->addRow("Code 1:", o1); fl->addRow("Code 1 close:", c1);
    fl->addRow("Comment 1:", o2); fl->addRow("Comment 1 close:", c2);
    vl->addLayout(fl);
    _tab->addTab(tab, "Folders");
}

void UserDefineDialog::setupCommentTab()
{
    QWidget* tab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(tab);
    QLineEdit* slEdit = new QLineEdit(tab);
    slEdit->setPlaceholderText("Single-line comment prefix (e.g. // or #)");
    QLineEdit* mlOpenEdit = new QLineEdit(tab);
    mlOpenEdit->setPlaceholderText("Multi-line comment open (e.g. /*)");
    QLineEdit* mlCloseEdit = new QLineEdit(tab);
    mlCloseEdit->setPlaceholderText("Multi-line comment close (e.g. */)");
    fl->addRow("Single-line:", slEdit);
    fl->addRow("Multi-line open:", mlOpenEdit);
    fl->addRow("Multi-line close:", mlCloseEdit);
    _tab->addTab(tab, "Comment");
}

void UserDefineDialog::setupStylingTab()
{
    QWidget* tab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(tab);
    _extColorLabel = new QLabel("Color:", tab);
    _extColorBtn = new QPushButton("Color...", tab);
    fl->addRow(_extColorLabel, _extColorBtn);
    QSpinBox* fontSize = new QSpinBox(tab);
    fontSize->setRange(1, 256);
    fontSize->setValue(10);
    fl->addRow("Font size:", fontSize);
    QCheckBox* boldChk = new QCheckBox("Bold", tab);
    QCheckBox* italicChk = new QCheckBox("Italic", tab);
    QCheckBox* underlineChk = new QCheckBox("Underline", tab);
    fl->addRow("Style:", boldChk);
    fl->addRow("", italicChk);
    fl->addRow("", underlineChk);
    _tab->addTab(tab, "Styling");
}

void UserDefineDialog::updateDialog()
{
    // Load UDL settings from Parameters
}

void UserDefineDialog::setKeywords(size_t listIndex, const QString& keywords)
{
    if (listIndex < 8 && _keywordsEdits[listIndex])
        _keywordsEdits[listIndex]->setPlainText(keywords);
}

void UserDefineDialog::onApply()
{
    // Collect keywords from all lists and save
    QStringList allKeywords;
    for (int i = 0; i < 8; ++i) {
        if (_keywordsEdits[i])
            allKeywords << _keywordsEdits[i]->toPlainText();
    }
    // NOTE: UDL persistence requires NppXml (QXmlStreamWriter) serialization of
    // UserLangContainer → XML. This is stubbed until NppXml is ported.
    // The signal below notifies listeners (ScintillaComponent) to reload keywords.
    emit userLangChanged();
}

void UserDefineDialog::onOk()
{
    onApply();
    hide();
}

void UserDefineDialog::onCancel()
{
    hide();
}

void UserDefineDialog::onTabChanged(int index)
{
    Q_UNUSED(index);
}
