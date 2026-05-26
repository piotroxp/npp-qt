// RunMacroDlg.h — Qt6 translation
#pragma once
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

class RunMacroDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RunMacroDlg(QWidget* parent = nullptr);
    ~RunMacroDlg() override;

    void doDialog(bool isRTL = false);
    void initMacroList();

    bool isMulti() const { return _multiCheck->isChecked(); }
    int getTimes() const { return _times; }
    int getMacroToExec() const;

signals:
    void runMacro(int macroIndex, int times);

protected:
    int run_dlgProc(QEvent* event);
    bool event(QEvent* event) override;

private slots:
    void onOKClicked();
    void onCancelClicked();
    void onMultiToggled(bool checked);
    void onMacroChanged(int index);

private:
    QComboBox* _macroCombo = nullptr;
    QCheckBox* _multiCheck = nullptr;
    QSpinBox* _timesSpin = nullptr;
    int _times = 1;
    int _macroIndex = 0;
};
