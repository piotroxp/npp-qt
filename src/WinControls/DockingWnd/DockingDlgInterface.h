// DockingDlgInterface.h — Qt6 translation
#pragma once

#include <QDialog>
#include <QByteArray>

class DockedWidgetData;

class DockingDlgInterface : public QDialog
{
    Q_OBJECT

public:
    DockingDlgInterface(QWidget* parent = nullptr);
    explicit DockingDlgInterface(int dlgID, QWidget* parent = nullptr);

    // Create with docking data
    virtual void create(DockedWidgetData* data, bool isRTL = false);
    virtual void create(DockedWidgetData* data, QVector<int> iconIDs, bool isRTL = false);

    // Update display info
    virtual void updateDockingDlg();

    // Colors
    virtual void setBackgroundColor(QRgb color) { Q_UNUSED(color); }
    virtual void setForegroundColor(QRgb color) { Q_UNUSED(color); }

    // Display state
    void display(bool toShow = true) override;

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

// Dockable dialog data structure
struct DockedWidgetData
{
    QWidget* client = nullptr;
    QString name;
    int dlgID = 0;
    unsigned mask = 0;
    QIcon iconTab;
    QString addInfo;
    QRect floatRect;
    int prevCont = 0;
    QString moduleName;
};
