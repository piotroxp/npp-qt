// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include "ScintillaComponent.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class ScintillaComponent;

class GoToLineDlg : public QDialog
{
    Q_OBJECT
public:
    GoToLineDlg(QWidget* parent = nullptr, Qt::WindowFlags f = {});
    void init(void* hInst, QWidget*& parent, ScintillaComponent** ppEditView);
    void doDialog(bool isRTL = false);
    void updateLinesNumbers();
    bool isCreated() const { return isVisible(); }

public slots:
    void accept() override;
    void onRadioToggled(bool checked);

private:
    enum mode { go2line, go2offset };
    mode _mode = go2line;
    ScintillaComponent** _ppEditView = nullptr;
    QLineEdit* _lineEdit = nullptr;
    QLabel* _currentLabel = nullptr;
    QLabel* _lastLabel = nullptr;
    QRadioButton* _radioLine = nullptr;
    QRadioButton* _radioOffset = nullptr;
    long long getLine() const;
};
