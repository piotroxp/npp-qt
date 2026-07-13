// npp-qt: GoToLineDlg implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/GoToLineDlg.cpp
// Target: npp-qt/src/ScintillaComponent/GoToLineDlg.cpp

#include "GoToLineDlg.h"
#include "ScintillaComponent.h"
#include "NppDarkMode.h"
#include "Parameters.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QApplication>

void GoToLineDlg::doDialog(bool isRTL)
{
	if (!isVisible())
	{
		setupUi(this);
		setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint
		               | Qt::WindowCloseButtonHint);
		setAttribute(Qt::WA_DeleteOnClose);

		_radioLine = new QRadioButton(tr("Line &Number"), this);
		_radioOffset = new QRadioButton(tr("&Offset"), this);
		_radioLine->setChecked(true);

		_lineEdit = new QLineEdit(this);
		_lineEdit->setMaxLength(256);
		_lineEdit->setPlaceholderText(tr("Enter line number or offset"));

		_currentLineLabel = new QLabel(this);
		_lastLineLabel = new QLabel(this);
		_currentLineLabel->setStyleSheet("QLabel { font-family: monospace; }");
		_lastLineLabel->setStyleSheet("QLabel { font-family: monospace; }");

		QPushButton* goButton = new QPushButton(tr("&Go"), this);
		QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
		buttonBox->addButton(goButton, QDialogButtonBox::AcceptRole);
		buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

		// Mode radio group
		QGroupBox* modeGroup = new QGroupBox(tr("Mode"), this);
		QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);
		modeLayout->addWidget(_radioLine);
		modeLayout->addWidget(_radioOffset);

		QLabel* currentLabel = new QLabel(tr("Current:"), this);
		QLabel* lastLabel = new QLabel(tr("Last:"), this);

		QGridLayout* infoLayout = new QGridLayout;
		infoLayout->addWidget(currentLabel, 0, 0);
		infoLayout->addWidget(_currentLineLabel, 0, 1);
		infoLayout->addWidget(lastLabel, 1, 0);
		infoLayout->addWidget(_lastLineLabel, 1, 1);

		QGridLayout* editLayout = new QGridLayout;
		editLayout->addWidget(new QLabel(tr("Go to:")), 0, 0);
		editLayout->addWidget(_lineEdit, 0, 1);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(modeGroup);
		mainLayout->addLayout(editLayout);
		mainLayout->addLayout(infoLayout);
		mainLayout->addWidget(buttonBox);

		// Connections
		connect(goButton, &QPushButton::clicked, this, &GoToLineDlg::onGoClicked);
		connect(cancelButton, &QPushButton::clicked, this, &GoToLineDlg::onCancelClicked);
		connect(_radioLine, &QRadioButton::toggled, this, &GoToLineDlg::onModeChanged);
		connect(_radioOffset, &QRadioButton::toggled, this, &GoToLineDlg::onModeChanged);
		connect(_lineEdit, &QLineEdit::returnPressed, this, &GoToLineDlg::onGoClicked);

		// Dark mode
		NppDarkMode::autoSubclassAndThemeChildControls(this);
	}

	// Update line numbers display
	updateLinesNumbers();
	_lineEdit->clear();
	_lineEdit->setFocus();
	exec();
}

void GoToLineDlg::updateLinesNumbers() const
{
	if (!_ppEditView || !*_ppEditView)
		return;

	ScintillaComponent* sci = *_ppEditView;

	if (_mode == go2line)
	{
		_currentLineLabel->setText(QString::number(sci->getCurrentLineNumber() + 1));
		_lastLineLabel->setText(QString::number(sci->send(SCI_GETLINECOUNT)));
	}
	else
	{
		_currentLineLabel->setText(QString::number(sci->send(SCI_GETCURRENTPOS)));
		size_t currentDocLength = sci->getCurrentDocLen();
		_lastLineLabel->setText(QString::number(currentDocLength > 0 ? currentDocLength - 1 : 0));
	}
}

int64_t GoToLineDlg::getLine() const
{
	QString text = _lineEdit->text().trimmed();
	if (text.isEmpty())
		return -1;
	bool ok = false;
	int64_t val = text.toLongLong(&ok);
	return ok ? val : -1;
}

void GoToLineDlg::onGoClicked()
{
	if (!_ppEditView || !*_ppEditView)
		return;

	ScintillaComponent* sci = *_ppEditView;

	// Disable notifications during goto
	sci->send(SCI_SETMODEVENTMASK, 0);

	int64_t line = getLine();
	if (line != -1)
	{
		if (_mode == go2line)
		{
			sci->send(SCI_ENSUREVISIBLE, static_cast<uptr_t>(line - 1));
			sci->send(SCI_GOTOLINE, static_cast<uptr_t>(line - 1));
		}
		else
		{
			size_t posToGoto = 0;
			if (line > 0)
			{
				auto before = sci->send(SCI_POSITIONBEFORE, static_cast<uptr_t>(line));
				posToGoto = sci->send(SCI_POSITIONAFTER, before);
			}
			auto sci_line = sci->send(SCI_LINEFROMPOSITION, posToGoto);
			sci->send(SCI_ENSUREVISIBLE, sci_line);
			sci->send(SCI_GOTOPOS, posToGoto);
		}
	}

	// Restore notification mask
	int MODEVENTMASK_ON = NppParameters::getInstance().getScintillaModEventMask();
	sci->send(SCI_SETMODEVENTMASK, MODEVENTMASK_ON);

	sci->grabFocus();
	accept();
}

void GoToLineDlg::onCancelClicked()
{
	reject();
}

void GoToLineDlg::onModeChanged()
{
	if (_radioLine->isChecked())
		_mode = go2line;
	else
		_mode = go2offsset;

	updateLinesNumbers();
}
