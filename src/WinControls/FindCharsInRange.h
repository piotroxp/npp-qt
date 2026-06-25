// Semantic Lift: Win32 FindCharsInRange → Qt6 QDialog
// Original: PowerEditor/src/WinControls/FindCharsInRange/*.{h,cpp}
// Target: npp-qt/src/WinControls/FindCharsInRange.h

#pragma once

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

// =============================================================================
// FindCharsInRangeDlg — find character in range dialog (lifted from FindCharsInRange.h)
// Translates Win32 radio buttons + edit controls → Qt radio buttons + spin boxes
// =============================================================================

class FindCharsInRangeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindCharsInRangeDlg(QWidget* parent = nullptr);
    ~FindCharsInRangeDlg() override = default;

    void doDialog();

protected:
    // event() mirrors Win32 run_dlgProc message switch
    bool event(QEvent* event) override;

private slots:
    void onFindNext();
    void onCancel();
    void onRadioChanged();

private:
    void setupUi();
    bool findCharInRange(unsigned char beginRange, unsigned char endRange, bool direction, bool wrap);
    bool getRangeFromUI(unsigned char& startRange, unsigned char& endRange);
    void getDirectionFromUI(bool& direction, bool& isWrap);

    // Editor reference (set by main window)
    QWidget* _editor = nullptr;

    QRadioButton* _nonAsciiRadio = nullptr;
    QRadioButton* _asciiRadio = nullptr;
    QRadioButton* _myRangeRadio = nullptr;
    QSpinBox* _rangeStartSpin = nullptr;
    QSpinBox* _rangeEndSpin = nullptr;
    QRadioButton* _dirDownRadio = nullptr;
    QRadioButton* _dirUpRadio = nullptr;
    QCheckBox* _wrapCheck = nullptr;
    QPushButton* _findNextBtn = nullptr;
    QPushButton* _cancelBtn = nullptr;
    QLabel* _staticLabel = nullptr;
};
