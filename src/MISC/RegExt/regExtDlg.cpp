// Semantic Lift: Win32 Register Extensions dialog → Qt6 QDialog implementation
// Original: PowerEditor/src/MISC/RegExt/regExtDlg.cpp
// Target: npp-qt/src/MISC/RegExt/regExtDlg.cpp
// Win32→Qt6:
//   WM_INITDIALOG → QDialog::showEvent()
//   WM_COMMAND    → QSignalMapper / direct slot connections
//   ListBox APIs  → QListWidget
//   Edit APIs     → QLineEdit
//   Win32 Registry → QSettings stubs (admin-only / Linux no-op)
//   NppDarkMode   → Qt stylesheet via NppDarkMode bridge

#include "regExtDlg.h"
#include "../../resource.h"
#include "../../Parameters.h"
#include "../../DarkMode/DarkMode.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QDebug>

// =============================================================================
// Extension data — language → associated file extensions
// Pure data, no Win32 dependency
// =============================================================================

constexpr int nbSupportedLang = 10;
constexpr int nbExtMax = 28;
constexpr int extNameMax = 18;

// nppName / nppBackup / nppDoc — written to registry on Win32
// On Linux these are informational only (no system-level file association)
static const QString nppName   = QStringLiteral("Notepad++_file");
static const QString nppBackup = QStringLiteral("Notepad++_backup");
static const QString nppDoc    = QStringLiteral("Notepad++ Document");

// clang-format off
static const QString defExtArray[nbSupportedLang][nbExtMax] = {
    {QStringLiteral("Notepad"),
     QStringLiteral(".txt"), QStringLiteral(".log"), QString() },
    {QStringLiteral("ms ini/inf"),
     QStringLiteral(".ini"), QStringLiteral(".inf"), QString() },
    {QStringLiteral("c, c++, objc"),
     QStringLiteral(".h"), QStringLiteral(".hh"), QStringLiteral(".hpp"),
     QStringLiteral(".hxx"), QStringLiteral(".c"), QStringLiteral(".cpp"),
     QStringLiteral(".cxx"), QStringLiteral(".cc"), QStringLiteral(".m"),
     QStringLiteral(".mm"), QStringLiteral(".vcxproj"), QStringLiteral(".vcproj"),
     QStringLiteral(".props"), QStringLiteral(".vsprops"), QStringLiteral(".manifest"),
     QString() },
    {QStringLiteral("java, c#, pascal"),
     QStringLiteral(".java"), QStringLiteral(".cs"), QStringLiteral(".pas"),
     QStringLiteral(".pp"), QStringLiteral(".inc"), QString() },
    {QStringLiteral("web script"),
     QStringLiteral(".html"), QStringLiteral(".htm"), QStringLiteral(".shtml"),
     QStringLiteral(".shtm"), QStringLiteral(".hta"), QStringLiteral(".asp"),
     QStringLiteral(".aspx"), QStringLiteral(".css"), QStringLiteral(".js"),
     QStringLiteral(".json"), QStringLiteral(".mjs"), QStringLiteral(".jsm"),
     QStringLiteral(".jsp"), QStringLiteral(".php"), QStringLiteral(".php3"),
     QStringLiteral(".php4"), QStringLiteral(".php5"), QStringLiteral(".phps"),
     QStringLiteral(".phpt"), QStringLiteral(".phtml"), QStringLiteral(".xml"),
     QStringLiteral(".xhtml"), QStringLiteral(".xht"), QStringLiteral(".xul"),
     QStringLiteral(".kml"), QStringLiteral(".xaml"), QStringLiteral(".xsml"),
     QString() },
    {QStringLiteral("public script"),
     QStringLiteral(".sh"), QStringLiteral(".bsh"), QStringLiteral(".bash"),
     QStringLiteral(".bat"), QStringLiteral(".cmd"), QStringLiteral(".nsi"),
     QStringLiteral(".nsh"), QStringLiteral(".lua"), QStringLiteral(".pl"),
     QStringLiteral(".pm"), QStringLiteral(".py"), QString() },
    {QStringLiteral("property script"),
     QStringLiteral(".rc"), QStringLiteral(".as"), QStringLiteral(".mx"),
     QStringLiteral(".vb"), QStringLiteral(".vbs"), QString() },
    {QStringLiteral("fortran, TeX, SQL"),
     QStringLiteral(".f"), QStringLiteral(".for"), QStringLiteral(".f90"),
     QStringLiteral(".f95"), QStringLiteral(".f2k"), QStringLiteral(".tex"),
     QStringLiteral(".sql"), QString() },
    {QStringLiteral("misc"),
     QStringLiteral(".nfo"), QStringLiteral(".mak"), QString(), QString() },
    {QStringLiteral("customize"),
     QString(), QString(), QString(), QString(), QString(), QString(), QString(),
     QString(), QString(), QString(), QString(), QString(), QString(), QString(),
     QString(), QString(), QString(), QString(), QString(), QString(), QString(),
     QString(), QString(), QString(), QString(), QString(), QString(), QString() }
};
// clang-format on

// =============================================================================
// Qt widgets — owned by this dialog
// =============================================================================
namespace Ui {
    class RegExtUi;
}

class RegExtUi
{
public:
    QListWidget *langList = nullptr;
    QListWidget *langExtList = nullptr;
    QListWidget *registeredExtList = nullptr;
    QLineEdit *customExtEdit = nullptr;
    QPushButton *addFromLangBtn = nullptr;
    QPushButton *removeExtBtn = nullptr;
    QPushButton *cancelBtn = nullptr;
    QLabel *supportedExtsLabel = nullptr;
    QLabel *registeredExtsLabel = nullptr;
    QLabel *adminMustBeOnLabel = nullptr;
};

// =============================================================================
// RegExtDlg
// =============================================================================

void RegExtDlg::doDialog(bool isRTL)
{
    // Win32: StaticDialog::myCreateDialogBoxIndirectParam(IDD_REGEXT_BOX, isRTL)
    // Qt6:   show() dialog, set RTL direction
    Q_UNUSED(isRTL);
    show();
}

bool RegExtDlg::eventFilter(QObject *obj, QEvent *event)
{
    // Map Win32 WM_* messages to Qt events
    if (event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            onCancelClicked();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void RegExtDlg::getRegisteredExts()
{
    // Win32: enumerate HKEY_CLASSES_ROOT subkeys starting with '.'
    //        and check if their default value == nppName
    // Qt6/Linux: no system registry → enumerate from ~/.local/share/applications/
    //            or XDG desktop file entries (stub: no registered extensions on Linux)

    // On first run, there are no file associations registered on Linux.
    // The UI still shows the available extensions for reference.
    qDebug() << "RegExtDlg: getRegisteredExts — no system registry on Linux; "
                "file associations are not tracked in this Qt6 port.";
}

void RegExtDlg::getDefSupportedExts()
{
    // Win32: populate language listbox with defExtArray[i][0] entries
    // Qt6:   populate langList QListWidget with the same strings
    // (handled in setupUi)
    Q_UNUSED(defExtArray);
}

void RegExtDlg::addExt(const QString &ext)
{
    // Win32: RegCreateKeyEx(HKEY_CLASSES_ROOT, ext) + RegSetValueEx(...)
    //        → register .ext → Notepad++ in Windows registry
    // Qt6/Linux: No-op (admin/system-level file associations require
    //            desktop environment integration beyond app scope).
    //            Could write to ~/.local/share/applications/npp.desktop
    //            as a future enhancement.
    qDebug() << "RegExtDlg: addExt" << ext << "— file association not persisted on Linux";
    Q_EMIT extensionAdded(ext);
}

bool RegExtDlg::deleteExts(const QString &ext2Delete)
{
    // Win32: delete registry key / value for .ext
    // Qt6/Linux: No-op (no registry equivalent)
    Q_UNUSED(ext2Delete);
    qDebug() << "RegExtDlg: deleteExts" << ext2Delete << "— no-op on Linux";
    Q_EMIT extensionRemoved(ext2Delete);
    return true;
}

void RegExtDlg::writeNppPath()
{
    // Win32: write HKEY_CLASSES_ROOT\nppName\shell\open\command
    //        + HKEY_CLASSES_ROOT\nppName\DefaultIcon
    // Qt6/Linux: Write ~/.local/share/applications/npp.desktop (stub)
    qDebug() << "RegExtDlg: writeNppPath — desktop file integration not implemented on Linux";
}

void RegExtDlg::onAddFromLangClicked()
{
    writeNppPath();

    QString ext2Add;
    if (!_isCustomize) {
        // Win32: LB_GETCURSEL + LB_GETTEXT from IDC_REGEXT_LANGEXT_LIST
        // Qt6:   currentItem() from langExtList QListWidget
        auto *item = langExtList->currentItem();
        if (!item) return;
        ext2Add = item->text();
        addExt(ext2Add);
        delete langExtList->takeItem(langExtList->currentRow());
    } else {
        // Win32: WM_GETTEXT from IDC_CUSTOMEXT_EDIT
        // Qt6:   text() from customExtEdit QLineEdit
        ext2Add = customExtEdit->text();
        if (registeredExtList->findItems(ext2Add, Qt::MatchExactly).isEmpty()) {
            addExt(ext2Add);
            registeredExtList->addItem(ext2Add);
        }
        customExtEdit->clear();
    }

    // Win32: EnableWindow(IDC_ADDFROMLANGEXT_BUTTON, false)
    // Qt6:   setEnabled(false)
    addFromLangBtn->setEnabled(false);
}

void RegExtDlg::onRemoveExtClicked()
{
    // Win32: LB_GETCURSEL + LB_GETTEXT from IDC_REGEXT_REGISTEREDEXTS_LIST
    //        → deleteExts(ext2Sup)
    // Qt6:   currentItem() + takeItem()
    auto *item = registeredExtList->currentItem();
    if (!item) return;
    QString ext2Sup = item->text();
    if (deleteExts(ext2Sup)) {
        delete registeredExtList->takeItem(registeredExtList->currentRow());
    }

    // Win32: LB_GETCURSEL from IDC_REGEXT_LANG_LIST to check if ext is in language list
    // Qt6:   search langExtList for the extension
    auto langIdx = langList->currentRow();
    if (langIdx >= 0 && langIdx < nbSupportedLang) {
        for (int i = 1; i < nbExtMax; ++i) {
            if (!defExtArray[langIdx][i].isEmpty() &&
                defExtArray[langIdx][i] == ext2Sup) {
                langExtList->addItem(ext2Sup);
                break;
            }
        }
    }

    removeExtBtn->setEnabled(false);
}

void RegExtDlg::onCustomExtEdited(const QString &text)
{
    // Win32: EN_CHANGE on IDC_CUSTOMEXT_EDIT
    //        Auto-prepend '.' if user typed single char without it
    QString processed = text;
    if (processed.length() == 1 && !processed.startsWith('.')) {
        processed = "." + processed;
        customExtEdit->setText(processed);
        customExtEdit->setCursorPosition(2);
    }
    addFromLangBtn->setEnabled(processed.length() > 1);
}

void RegExtDlg::onLangSelectionChanged(int row)
{
    if (row < 0) return;

    // Win32: LBN_SELCHANGE on IDC_REGEXT_LANG_LIST
    //        If "customize" selected → show customExtEdit; else populate langExtList
    if (!defExtArray[row][1].isEmpty() && defExtArray[row][0] == QStringLiteral("customize")) {
        // "customize" row selected
        langExtList->hide();
        customExtEdit->show();
        _isCustomize = true;
    } else {
        if (_isCustomize) {
            langExtList->show();
            customExtEdit->hide();
            _isCustomize = false;
        }
        // Clear and repopulate langExtList with extensions for selected language
        langExtList->clear();
        for (int j = 1; j < nbExtMax; ++j) {
            if (!defExtArray[row][j].isEmpty()) {
                // Check if already in registered list
                if (registeredExtList->findItems(defExtArray[row][j], Qt::MatchExactly).isEmpty()) {
                    langExtList->addItem(defExtArray[row][j]);
                }
            }
        }
        addFromLangBtn->setEnabled(false);
    }
}

void RegExtDlg::onLangExtSelectionChanged(int row)
{
    Q_UNUSED(row);
    // Win32: LBN_SELCHANGE on IDC_REGEXT_LANGEXT_LIST
    // Qt6:   selectionModel()->currentRow() changed
    addFromLangBtn->setEnabled(langExtList->currentRow() >= 0);
}

void RegExtDlg::onRegisteredExtSelectionChanged(int row)
{
    Q_UNUSED(row);
    // Win32: LBN_SELCHANGE on IDC_REGEXT_REGISTEREDEXTS_LIST
    // Qt6:   selectionModel()->currentRow() changed
    removeExtBtn->setEnabled(registeredExtList->currentRow() >= 0);
}

void RegExtDlg::onCancelClicked()
{
    hide();
}

// =============================================================================
// Factory function — creates the Qt6 dialog from Win32 dialog resource
// Win32: myCreateDialogBoxIndirectParam(IDD_REGEXT_BOX, ...)
// Qt6:   constructs and positions widgets matching the Win32 dialog layout
// =============================================================================

// The actual Qt widget creation is done in a separate UI helper class.
// The dialog layout (matching the Win32 .rc definition) uses:
//   - Left group: Language list + language extension list
//   - Right group: Registered extensions list + add/remove buttons
//   - Bottom: Custom extension edit + Cancel button
// This is structurally equivalent to the Win32 dialog box with two
// listboxes side-by-side, add/remove buttons between them, and a custom
// extension input field below.
//
// Actual widget creation happens in the main Notepad++ window which owns
// and instantiates RegExtDlg. This file provides the logic layer only.
