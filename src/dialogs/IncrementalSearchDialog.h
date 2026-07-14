#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class ScintillaEditor;

class IncrementalSearchDialog : public QDialog {
    Q_OBJECT
public:
    explicit IncrementalSearchDialog(QWidget* parent = nullptr);
    void setSearchText(const QString& text);
    void showAtTop();
    void setEditor(ScintillaEditor* editor);

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
    void performSearch(const QString& text, bool forward);
    void updateMatchCount(const QString& text);
    void clearHighlight();

    QLineEdit* _searchEdit = nullptr;
    QLabel* _countLabel = nullptr;
    ScintillaEditor* _editor = nullptr;
    int _currentMatch = 0;
    int _totalMatches = 0;
    QString _lastText;
};
