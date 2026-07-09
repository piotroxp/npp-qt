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
#include <QColor>
#include <QFont>
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
#include <QColorDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QFontComboBox>

// ClipboardHistoryPanel — shows recent clipboard entries, like Notepad++
class ClipboardHistoryPanel : public QWidget {
    Q_OBJECT
public:
    explicit ClipboardHistoryPanel(QWidget* parent = nullptr);
    void addEntry(const QString& text);
    void clear();
Q_SIGNALS:
    void pasteRequested(int index);
private Q_SLOTS:
    void onClipboardChanged();
private:
    QListWidget* m_list = nullptr;
    static constexpr int MaxEntries = 100;
};

// ColourPicker — colour palette with custom colour picker
class ColourPicker : public QWidget {
    Q_OBJECT
public:
    explicit ColourPicker(QWidget* parent = nullptr);
    QColor currentColor() const { return m_current; }
    void setCurrentColor(const QColor& c);
Q_SIGNALS:
    void colorSelected(const QColor& c);
private:
    void setupPalette();
    QColor m_current = Qt::white;
    QVector<QColor> m_palette;
    QLabel* m_preview = nullptr;
};

// WordStyleDlg — syntax highlighting style editor
class WordStyleDlg : public QWidget {
    Q_OBJECT
public:
    explicit WordStyleDlg(QWidget* parent = nullptr);
Q_SIGNALS:
    void styleChanged(int id, const QString& prop, const QVariant& val);
private:
    void setupUi();
    QComboBox* m_langCombo = nullptr;
    QComboBox* m_styleCombo = nullptr;
    ColourPicker* m_fgPicker = nullptr;
    ColourPicker* m_bgPicker = nullptr;
    QFontComboBox* m_fontCombo = nullptr;
    QSpinBox* m_sizeSpin = nullptr;
    QCheckBox* m_bold = nullptr;
    QCheckBox* m_italic = nullptr;
    QCheckBox* m_underline = nullptr;
};

// ToolTipButton — tool button with extended tip (tip + tip2)
class ToolTipButton : public QToolButton {
    Q_OBJECT
public:
    explicit ToolTipButton(QWidget* parent = nullptr);
    void setTip(const QString& t) { setToolTip(t); }
    void setTip2(const QString& t) { m_tip2 = t; }
protected:
    bool event(QEvent* e) override;
private:
    QString m_tip2;
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
