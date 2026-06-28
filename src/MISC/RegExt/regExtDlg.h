// Semantic Lift: Win32 Register Extensions dialog → Qt6 QDialog
// Original: PowerEditor/src/MISC/RegExt/regExtDlg.h
// Target: npp-qt/src/MISC/RegExt/regExtDlg.h
// Win32→Qt6: StaticDialog → QDialog; HWND → QWidget*; WM_* → signals/slots;
//           Win32 Registry → QSettings stubs (admin-only on Linux)

#pragma once

#include "regExtDlgRc.h"
#include "../../WinControls/StaticDialog.h"
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>

constexpr int extNameLen = 32;

// =============================================================================
// RegExtDlg — Register File Associations dialog
// Maps .ext → Notepad++ via QSettings (admin-only; no-op on Linux)
// =============================================================================

class RegExtDlg : public StaticDialog
{
    Q_OBJECT

public:
    RegExtDlg() = default;
    ~RegExtDlg() override = default;
    void doDialog(bool isRTL = false);

Q_SIGNALS:
    void extensionAdded(const QString &ext);
    void extensionRemoved(const QString &ext);

public Q_SLOTS:
    void onAddFromLangClicked();
    void onRemoveExtClicked();
    void onCustomExtEdited(const QString &text);
    void onLangSelectionChanged(int row);
    void onLangExtSelectionChanged(int row);
    void onRegisteredExtSelectionChanged(int row);
    void onCancelClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    bool _isCustomize = false;

    // Win32 dlgProc → Qt event/slot system
    // intptr_t CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
    // → handled via eventFilter + explicit signal connections above

    void getRegisteredExts();
    void getDefSupportedExts();
    void addExt(const QString &ext);
    bool deleteExts(const QString &ext2Delete);
    void writeNppPath();

    static int getNbSubKey(QSettings &reg) {
        // Linux: QSettings has no key-count API; return 0 (no file associations)
        Q_UNUSED(reg);
        return 0;
    }

    static int getNbSubValue(QSettings &reg) {
        Q_UNUSED(reg);
        return 0;
    }

    // Member widgets — initialized by the UI layer that owns this dialog
    QListWidget* langExtList = nullptr;
    QLineEdit* customExtEdit = nullptr;
    QListWidget* registeredExtList = nullptr;
    QPushButton* addFromLangBtn = nullptr;
    QPushButton* removeExtBtn = nullptr;
    QListWidget* langList = nullptr;
};
