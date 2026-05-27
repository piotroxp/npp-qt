// DockingDlgInterface.h — Qt6 translation
#pragma once

#include <QDialog>
#include <QByteArray>
#include <QVector>
#include <QString>
#include <QIcon>
#include <QRect>

// Forward declaration - DockedWidgetData is defined in Docking.h
struct DockedWidgetData;

class DockingDlgInterface : public QDialog
{
    Q_OBJECT

public:
    DockingDlgInterface(QWidget* parent = nullptr);
    explicit DockingDlgInterface(int dlgID, QWidget* parent = nullptr);

    // Create with docking data
    virtual void create(DockedWidgetData* data, bool isRTL = false);
    virtual void create(DockedWidgetData* data, QVector<int> iconIDs, bool isRTL = false);
    
    // Dialog procedure (WinForms → Qt6 shim)
    virtual intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) { Q_UNUSED(message); Q_UNUSED(wParam); Q_UNUSED(lParam); return 0; }

    // Update display info
    virtual void updateDockingDlg();

    // Colors
    virtual void setBackgroundColor(QRgb color) { Q_UNUSED(color); }
    virtual void setForegroundColor(QRgb color) { Q_UNUSED(color); }

    // Display state
    void display(bool toShow = true);

    // Plugin info
    bool isClosed() const { return _isClosed; }
    void setClosed(bool closed) { _isClosed = closed; }
    QString getPluginFileName() const { return _moduleName; }

signals:
    void closeRequested();
    void floatRequested();
    void dockRequested(int position);

protected:
    int _dlgID = -1;
    QString _moduleName;
    QString _pluginName;
    bool _isFloating = true;
    bool _isClosed = false;
    int _dockedPosition = 0;

    bool event(QEvent* event) override;
};
