// StaticDialog.h — Qt6 translation of CDialog → QDialog
#pragma once

#include <QDialog>
#include <QRect>
#include <QPoint>
#include <QVector>

enum class PosAlign { Left, Right, Top, Bottom };

class StaticDialog : public QDialog
{
    Q_OBJECT

public:
    StaticDialog(QWidget* parent = nullptr, Qt::WindowFlags f = {});
    ~StaticDialog() override;

    // Create the dialog
    void create(int dialogId, bool isRTL = false, bool msgDestParent = true);

    // Check if created
    bool isCreated() const { return _isCreated; }

    // Get mapped rect for child widgets
    QRect getMappedChildRect(QWidget* child) const;
    QRect getMappedChildRect(int childId) const;

    // Redraw a child control
    void redrawDlgItem(int nIDDlgItem, bool forceUpdate = false) const;

    // Position helpers
    void goToCenter(unsigned swpFlags = Qt::WindowFlags(Qt::ShowWindow));
    bool moveForDpiChange();

    // Display/hide
    void display(bool toShow = true) override;
    void displayEnhanced(bool toShow);

    // Viewable position rect for multi-monitor
    QRect getViewablePositionRect(const QRect& testPositionRc) const;

    // Get top point of a child
    QPoint getTopPoint(QWidget* hwnd, bool isLeft = true) const;

    // Check/uncheck button
    bool isCheckedOrNot(int checkControlID) const;
    void setChecked(int checkControlID, bool checkOrNot = true) const;

    // DPI
    void setDpi() { /* handled by Qt */ }
    void setPositionDpi(WPARAM wParam, LPARAM lParam);

    // Get rect
    QRect getRect() const { return _rc; }

    // Destroy
    void destroy() override;

signals:
    void dpiChanged();

protected:
    // Override run_dlgProc in subclasses
    virtual int run_dlgProc(QEvent* event) = 0;

    bool event(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    QRect _rc;
    bool _isCreated = false;
};

// DlgInfo for ControlsTab
struct DlgInfo
{
    QWidget* _widget = nullptr;
    QString _name;
    QString _internalName;

    DlgInfo(QWidget* widget, const QString& name, const QString& internalName = QString())
        : _widget(widget), _name(name), _internalName(internalName) {}
};

using WindowVector = QVector<DlgInfo>;
