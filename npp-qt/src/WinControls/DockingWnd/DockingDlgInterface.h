// Semantic Lift: Win32 DockingDlgInterface → Qt6 QWidget subclass
// Original: PowerEditor/src/WinControls/DockingWnd/DockingDlgInterface.h
// Target: npp-qt/WinControls/DockingWnd/DockingDlgInterface.h

#pragma once

#include <QWidget>
#include <QString>
#include <QStringBuilder>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QEvent>
#include <QArray>
#include <QGuiApplication>
#include <QFileInfo>

#include "Docking.h"
#include "StaticDialog.h"
#include "NppDarkMode.h"
#include "dockingResource.h"

// =============================================================================
// DockingDlgInterface — base class for plugin/plugin panel dialogs that
// dock into the Notepad++ main window.
// Lifts StaticDialog + WM_NOTIFY-based docking notifications → Qt6 signals.
// =============================================================================
class DockingDlgInterface : public StaticDialog
{
    Q_OBJECT

public:
    DockingDlgInterface() = default;
    explicit DockingDlgInterface(int dlgID) : _dlgID(dlgID) {}

    // ── Initialisation ────────────────────────────────────────────────────

    // In Qt6, hInst is replaced by the QApplication* and parent by QWidget*.
    // The module name is derived from applicationFilePath().
    void init(QApplication* /*hInst*/, QWidget* parent) override {
        StaticDialog::init(nullptr, parent);
        _moduleName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    }

    // ── Dialog creation ───────────────────────────────────────────────────

    // Main create — subclasses should call this once after init().
    // data carries the plugin's docking configuration back to the manager.
    virtual void create(DockedWidgetData* data, bool isRTL = false) {
        Q_ASSERT(data != nullptr);
        StaticDialog::create(_dlgID, isRTL);
        _pluginName = windowTitle();

        // Return data to docking manager
        data->hClient      = this;
        data->name         = _pluginName;
        data->uMask        = 0;
        data->addInfo.clear();
    }

    // Overload with icon IDs for the tab/toolbar
    virtual void create(DockedWidgetData* data, std::array<int, 3> iconIDs, bool isRTL = false) {
        create(data, isRTL);
        _iconIDs = iconIDs;
    }

    // ── Docking manager integration ─────────────────────────────────────────

    // Notify the docking manager to refresh display info.
    virtual void updateDockingDlg() {
        // Emit a signal that NppMainWindow connects to NPPM_DMMUPDATEDISPINFO
        emit updateDisplayInfo(this);
    }

    virtual void setBackgroundColor(QRgb /*color*/) {}
    virtual void setForegroundColor(QRgb /*color*/) {}

    // Show/hide — maps to DMMSHOW/DMMHIDE in the docking manager
    void display(bool toShow = true) override {
        if (toShow) {
            show();
            emit visibilityChanged(this, true);
        } else {
            hide();
            emit visibilityChanged(this, false);
        }
    }

    bool isClosed() const { return _isClosed; }
    void setClosed(bool toClose) { _isClosed = toClose; }

    // ── Accessors ───────────────────────────────────────────────────────────

    QString pluginFileName() const { return _moduleName; }
    const std::array<int, 3>& iconIDs() const { return _iconIDs; }
    int dockedPos() const { return _iDockedPos; }
    bool isFloating() const { return _isFloating; }

Q_SIGNALS:
    // Emitted when the dialog visibility changes
    void visibilityChanged(QWidget* self, bool visible);
    // Emitted to trigger NPPM_DMMUPDATEDISPINFO in the docking manager
    void updateDisplayInfo(QWidget* self);

    // Docking state change notifications
    void dockClosed(QWidget* self);
    void dockFloated(QWidget* self);
    void dockDocked(QWidget* self, int dockPos);

protected:
    // ── Dialog procedure — Qt6 event overrides ─────────────────────────────

    // WM_ERASEBKGND → paintEvent / eraseEvent
    bool event(QEvent* e) override {
        if (e->type() == QEvent::UpdateRequest || e->type() == QEvent::Paint) {
            if (!NppDarkMode::isEnabled_Static()) {
                return QWidget::event(e);
            }
        }
        return StaticDialog::event(e);
    }

    // Custom event for docking manager notifications (DMN_CLOSE, DMN_FLOAT, DMN_DOCK)
    // The docking manager emits a custom Qt event with type in the DMN_* range.
    void customEvent(QEvent* event) override {
        auto* notifEvent = dynamic_cast<QNotifyEvent*>(event);
        if (!notifEvent) {
            return;
        }

        int code = static_cast<int>(event->type());

        if (code == DMN_CLOSE) {
            emit dockClosed(this);
        } else if (code == DMN_FLOAT) {
            _isFloating = true;
            emit dockFloated(this);
        } else if (code == DMN_DOCK) {
            // High word carries the dock position (CONT_* constant)
            _iDockedPos = HIWORD(code);
            _isFloating = false;
            emit dockDocked(this, _iDockedPos);
        }
    }

    // ── Dark-mode background painting ───────────────────────────────────────
    // Replaces WM_ERASEBKGND + FillRect with NppDarkMode brush.
    // Subclasses can call paintDarkBackground() in their paintEvent.
    void paintDarkBackground(QPainter* painter) {
        if (!NppDarkMode::isEnabled_Static()) {
            return;
        }
        painter->fillRect(rect(), NppDarkMode::getDlgBackgroundBrush());
    }

private:
    int _dlgID = -1;
    int _iDockedPos = 0;
    QString _moduleName;
    QString _pluginName;
    std::array<int, 3> _iconIDs{};
    bool _isFloating = true;
    bool _isClosed = false;
};
