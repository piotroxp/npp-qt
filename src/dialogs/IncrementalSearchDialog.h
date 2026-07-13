#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class IncrementalSearchDialog : public QDialog {
    Q_OBJECT
public:
    explicit IncrementalSearchDialog(QWidget* parent = nullptr);
    void setSearchText(const QString& text);
    void showAtTop();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void searchNext(const QString& text);
    void searchPrev(const QString& text);
    void closeRequested();

private slots:
    void onTextChanged(const QString& text);
    void onNext();
    void onPrev();

private:
    QLineEdit* _searchEdit;
    QLabel* _countLabel;
    int _currentMatch = 0;
    int _totalMatches = 0;
    QString _lastText;
};
