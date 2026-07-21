#pragma once
#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

class GoToLineDialog : public QDialog {
    Q_OBJECT
public:
    explicit GoToLineDialog(QWidget* parent = nullptr);
    ~GoToLineDialog() override;
    void setMaxLineNumber(int maxLine);
    void setMaxColumnNumber(int maxCol);
    int getTargetLine() const { return lineSpinBox->value(); }
    int getTargetColumn() const { return columnSpinBox->value(); }
    bool goToOffset() const { return offsetCheckBox->isChecked(); }

protected:
    bool event(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void setVisible(bool visible) override;

private slots:
    void onGoClicked();
    void onCancelClicked();
    void validateInput();

private:
    void setupUi();
    void updateStatusLabel();
    void applyStyle();
    bool _destructing = false;

    QSpinBox* lineSpinBox = nullptr;
    QSpinBox* columnSpinBox = nullptr;
    QCheckBox* offsetCheckBox = nullptr;
    QLabel* statusLabel = nullptr;
    QLabel* rangeLabel = nullptr;
    QPushButton* goBtn = nullptr;
    QPushButton* cancelBtn = nullptr;
    int maxLine = 1;
    int maxColumn = 1;
};
