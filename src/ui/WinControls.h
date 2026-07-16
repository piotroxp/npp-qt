// WinControls.h — Qt6 widgets replicating Win32 common controls
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QSize>
#include <QPoint>
#include <QIcon>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QCursor>
#include <QToolTip>
#include <QClipboard>
#include <QApplication>

// Forward declaration
class DpiManager;

// ColourPicker — full implementation in src/ui/ColourPicker.h
class ColourPicker;
// ColourPopup — full implementation in src/ui/ColourPopup.h
class ColourPopup;

// ToolTipButton — tool button with extended tip (tip + tip2)
class ToolTipButton : public QToolButton {
    Q_OBJECT
public:
    explicit ToolTipButton(QWidget* parent = nullptr);
    void setTip(const QString& t) { setToolTip(t); }
    void setTip2(const QString& t) { m_tip2 = t; }
    void setDpiManager(DpiManager* dm) { m_dpiManager = dm; }
protected:
    bool event(QEvent* e) override;
private:
    QString m_tip2;
    DpiManager* m_dpiManager = nullptr;
};

// NativeListBox — list box wrapper (Win32 LB_* → Qt)
class NativeListBox : public QListWidget {
    Q_OBJECT
public:
    explicit NativeListBox(QWidget* parent = nullptr);
    int addString(const QString& text, int data = -1);
    void setCurSel(int i) { setCurrentRow(i); }
    int getCurSel() const { return currentRow(); }
    void resetContent() { clear(); }
    QString getText(int i) const;
};

// EditableLabel — clickable label that becomes editable
class EditableLabel : public QWidget {
    Q_OBJECT
public:
    explicit EditableLabel(QWidget* parent = nullptr);
    QString text() const { return m_label->text(); }
    void setText(const QString& text);
Q_SIGNALS:
    void editingFinished(const QString& text);
    void clicked();
private Q_SLOTS:
    void startEditing();
    void finishEditing();
private:
    QLabel* m_label = nullptr;
    QLineEdit* m_lineEdit = nullptr;
};

// ProgressIndicator — animated progress indicator for status bar
class ProgressIndicator : public QWidget {
    Q_OBJECT
public:
    explicit ProgressIndicator(QWidget* parent = nullptr);
    void setProgress(int value, int maximum = 100);
    void startAnimation();
    void stopAnimation();
    void setText(const QString& text);
    int value() const { return m_value; }
    int maximum() const { return m_maximum; }
    bool isAnimating() const { return m_animating; }
Q_SIGNALS:
    void cancelled();
private:
    void paintEvent(QPaintEvent* event) override;
    int m_value = 0;
    int m_maximum = 100;
    bool m_animating = false;
    QString m_text;
};
