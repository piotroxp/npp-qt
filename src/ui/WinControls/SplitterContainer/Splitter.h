// Splitter.h - Qt6 splitter widget
// INTENT: source uses CSplitterWnd. Target uses QSplitter.
#pragma once
#include <QSplitter>
#include <QByteArray>

// Splitter wraps QSplitter to match Notepad++ CSplitterWnd API.
class Splitter : public QSplitter {
    Q_OBJECT
public:
    explicit Splitter(QWidget* parent = nullptr);
    ~Splitter() override = default;

    // Save/restore splitter position
    void saveStateTo(QByteArray& state) const;
    void restoreStateFrom(const QByteArray& state);

    // Set which panel is the "real" editor (for focus handling)
    void setEditorWidget(QWidget* editor);

    // Panel visibility
    void showPanel(int index);
    void hidePanel(int index);

    // Get the active panel index
    int activePanel() const { return _activePanel; }
    void setActivePanel(int index) { _activePanel = index; }

private:
    int _activePanel = 0;
    QWidget* _editorWidget = nullptr;
};
