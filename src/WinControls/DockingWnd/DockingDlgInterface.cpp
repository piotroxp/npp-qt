#include "DockingDlgInterface.h"
#include <QEvent>

DockingDlgInterface::DockingDlgInterface(QWidget* parent) : QDialog(parent), _hParent(parent) {}
DockingDlgInterface::DockingDlgInterface(int dlgID, QWidget* parent) : QDialog(parent), _dlgID(dlgID), _hParent(parent) {}

void DockingDlgInterface::create(DockedWidgetData* data, bool isRTL) { (void)data; (void)isRTL; }
void DockingDlgInterface::create(DockedWidgetData* data, QList<int> iconIDs, bool isRTL) { (void)data; (void)iconIDs; (void)isRTL; }
void DockingDlgInterface::updateDockingDlg() {}
void DockingDlgInterface::display(bool toShow) { setVisible(toShow); _isClosed = !toShow; }

bool DockingDlgInterface::event(QEvent* event)
{
    return QDialog::event(event);
}
