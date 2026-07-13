#pragma once

#include <QWidget>
#include <QLabel>

class ScintillaEdit;

class DocumentSnapshot : public QObject
{
    Q_OBJECT
public:
    DocumentSnapshot();
    ~DocumentSnapshot() override;

    void init(QWidget* parent, ScintillaEdit* pEditView);
    void showSnapshot();
    void hideSnapshot();
    void move(int x, int y);
    void setVisible(bool visible);
    bool isVisible() const;

private:
    QLabel* _snapshotLabel = nullptr;
    QWidget* _parent = nullptr;
    ScintillaEdit* _pEditView = nullptr;
    bool _isVisible = false;
};

