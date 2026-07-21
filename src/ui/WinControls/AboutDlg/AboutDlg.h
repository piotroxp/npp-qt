// AboutDlg.h - Qt6 about dialog
// INTENT: source shows app info + GPL. Target uses QDialog.
#pragma once
#include "../StaticDialog/StaticDialog.h"

class AboutDlg : public StaticDialog {
    Q_OBJECT
public:
    static void showAbout(QWidget* parent);

private:
    explicit AboutDlg(QWidget* parent);
    ~AboutDlg() override = default;
};
