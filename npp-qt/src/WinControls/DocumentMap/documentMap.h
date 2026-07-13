#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QTimer>

class ScintillaEdit;

class DocumentMap : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentMap(QWidget* parent = nullptr);
    ~DocumentMap() override;

    void init(ScintillaEdit* pScintilla, QWidget* view, int nbChar = 200);
    void setScintilla(ScintillaEdit* pScintilla);
    void setCurrentPosition(int pos);
    void drawDocumentMap();

private slots:
    void syncScrollBar();
    void onScintillaUpdate(int, int, void*);

protected:
    void updateMiniView();
    void updateCurrentIndicator();
    void resizeEvent(QResizeEvent* event) override;

private:
    ScintillaEdit* _pScintilla = nullptr;
    QWidget* _pView = nullptr;
    QLabel* _miniViewLabel = nullptr;
    QTimer* _syncTimer = nullptr;
    int _nbChar = 200;
    int _currentPos = 0;
};

