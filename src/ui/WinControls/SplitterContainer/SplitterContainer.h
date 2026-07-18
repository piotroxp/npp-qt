// SplitterContainer.h - Qt6 panel splitter
// INTENT: source uses CSplitterWnd. Target uses QSplitter.
#pragma once
#include <QSplitter>
#include <QVector>
#include <QByteArray>

class SplitterContainer : public QSplitter {
    Q_OBJECT
public:
    explicit SplitterContainer(QWidget* parent = nullptr);
    ~SplitterContainer() override = default;

    // Save/restore splitter sizes
    QByteArray savePanelState() const;
    void restorePanelState(const QByteArray& state);

    // Add a panel widget
    void addPanel(QWidget* w);
    void removePanel(int index);

    // Active panel tracking
    int activePanel() const { return _activePanel; }
    void setActivePanel(int index);

signals:
    void panelChanged(int index);

private:
    int _activePanel = 0;
};
