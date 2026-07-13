// npp-qt: GoToLineDlg header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/GoToLineDlg.h
// Target: npp-qt/src/ScintillaComponent/GoToLineDlg.h

#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>

class ScintillaComponent;

// Go To Line dialog — Qt6 lift from Win32 StaticDialog
class GoToLineDlg : public QDialog
{
	Q_OBJECT

public:
	GoToLineDlg() = default;

	void init(ScintillaComponent** ppEditView) {
		if (!ppEditView)
			throw std::runtime_error("GoToLineDlg::init: ppEditView is null.");
		_ppEditView = ppEditView;
	}

	void doDialog(bool isRTL = false);

public slots:
	void onGoClicked();
	void onCancelClicked();
	void onModeChanged();

protected:
	enum mode { go2line, go2offsset };
	mode _mode = go2line;
	ScintillaComponent** _ppEditView = nullptr;

	QLineEdit* _lineEdit = nullptr;
	QLabel* _currentLineLabel = nullptr;
	QLabel* _lastLineLabel = nullptr;
	QRadioButton* _radioLine = nullptr;
	QRadioButton* _radioOffset = nullptr;

	void updateLinesNumbers();
	int64_t getLine() const;
};
