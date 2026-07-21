// StaticDialog.h - Qt6 base class for all Notepad--Qt dialogs
// INTENT: source uses CDialog with a Win32 window procedure. Target uses QDialog as the base.
// Provides: centering, DPI management, child window mapping, dialog positioning.

#pragma once

#include <QDialog>
#include <QRect>
#include <QPoint>
#include <QWidget>
#include <QWindow>

// Position alignment for child windows
enum class PosAlign { left, right, top, bottom };

// StaticDialog is the Qt6 equivalent of Notepad++'s StaticDialog.
// All dialogs in Notepad--Qt should inherit from this.
class StaticDialog : public QDialog {
    Q_OBJECT

public:
    explicit StaticDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~StaticDialog() override;

    // Show or hide the dialog
    void display(bool show = true);
    void displayEnhanced(bool show);

    // Check if dialog is currently visible
    bool isVisible() const { return QDialog::isVisible(); }

    // Check/uncheck a child control by objectName
    bool isChecked(const QString& objectName) const;
    void setChecked(const QString& objectName, bool checked);

    // Get/set DPI for the dialog
    void updateDpi(int dpi);

    // Get the geometry of a child widget in dialog coordinates
    QRect childRect(const QString& objectName) const;

    // Center the dialog on its parent
    void goToCenter();

    // Get the rectangle of the dialog
    QRect dialogRect() const;

    // Get the center point of the dialog
    QPoint centerPoint() const;

    // Map a child widget's rect to dialog coordinates
    QRect mapChildToDialog(QWidget* child) const;

    // Redraw a child control
    void redrawChild(const QString& objectName);

    // Move dialog when DPI changes
    void moveForDpiChange(int newDpi);

signals:
    void dialogShown();
    void dialogHidden();

protected:
    // Handle show/hide events
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    int _dpi = 96;
    QRect _initialRect;
    bool _isInitialized = false;
};
