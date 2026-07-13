#pragma once

#include <QDialog>

class SizeableDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SizeableDlg(QWidget* parent = nullptr);
    ~SizeableDlg() override;

    void setMinSize(int cx, int cy);
    void setMaxSize(int cx, int cy);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    int _minCX = 0, _minCY = 0;
    int _maxCX = 99999, _maxCY = 99999;
};

