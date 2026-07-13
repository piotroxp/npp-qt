#pragma once

#include <QDialog>
#include <QString>

class FindCharsInRangeDlg : public QDialog
{
    Q_OBJECT
public:
    explicit FindCharsInRangeDlg(QWidget* parent = nullptr);
    ~FindCharsInRangeDlg() override;

    void init();
    void doDialog();
    void display();
    void goToChar(int charCode);
    int getSelectedChar() const;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    int _selectedChar;
};

