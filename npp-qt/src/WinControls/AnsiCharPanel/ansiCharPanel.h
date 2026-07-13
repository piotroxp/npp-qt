#pragma once

#include <QWidget>
#include <QString>

class AnsiCharPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AnsiCharPanel(QWidget* parent = nullptr);
    ~AnsiCharPanel() override;

    void init(QWidget* parent);
    void display(int asciiCode);
    int getSelectedChar() const { return _selectedChar; }

signals:
    void charSelected(int asciiCode);

private:
    int _selectedChar = -1;
};

