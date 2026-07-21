#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QColor>

class ScintillaEditor;

class IncrementalSearchDialog : public QDialog {
    Q_OBJECT
public:
    explicit IncrementalSearchDialog(QWidget* parent = nullptr);
    ~IncrementalSearchDialog() override;
    void setSearchText(const QString& text);
    void showAtTop();
    void setEditor(ScintillaEditor* editor);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void hideEvent(QHideEvent* event) override;

Q_SIGNALS:
    void searchNext(const QString& text);
    void searchPrev(const QString& text);
    void closeRequested();
    void searchTextChanged(const QString& text);

public Q_SLOTS:
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

    // Highlight styling
    int _highlightIndicator = 3;  // Scintilla indicator for isearch matches
    bool _highlightActive = false;
};
