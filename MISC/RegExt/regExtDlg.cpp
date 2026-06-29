// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "MISC/RegExt/regExtDlg.h"
#include "Parameters.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QGuiApplication>

// NppName used to tag registry entries — kept as a constant string for
// the QSettings-based association storage on Unix.
static const QString nppName   = QStringLiteral("Notepad++_file");
static const QString nppBackup = QStringLiteral("Notepad++_backup");
static const QString nppDoc    = QStringLiteral("Notepad++ Document");

// Language / extension table (identical to Win32 defExtArray)
// Last entry is "customize" with no predefined extensions.
const QString RegExtDlg::defExtArray[nbSupportedLang][nbExtMax] = {
    {QStringLiteral("Notepad"),
     QStringLiteral(".txt"), QStringLiteral(".log")
    },
    {QStringLiteral("ms ini/inf"),
     QStringLiteral(".ini"), QStringLiteral(".inf")
    },
    {QStringLiteral("c, c++, objc"),
     QStringLiteral(".h"), QStringLiteral(".hh"), QStringLiteral(".hpp"), QStringLiteral(".hxx"),
     QStringLiteral(".c"), QStringLiteral(".cpp"), QStringLiteral(".cxx"), QStringLiteral(".cc"),
     QStringLiteral(".m"), QStringLiteral(".mm"),
     QStringLiteral(".vcxproj"), QStringLiteral(".vcproj"), QStringLiteral(".props"),
     QStringLiteral(".vsprops"), QStringLiteral(".manifest")
    },
    {QStringLiteral("java, c#, pascal"),
     QStringLiteral(".java"), QStringLiteral(".cs"), QStringLiteral(".pas"),
     QStringLiteral(".pp"), QStringLiteral(".inc")
    },
    {QStringLiteral("web script"),
     QStringLiteral(".html"), QStringLiteral(".htm"), QStringLiteral(".shtml"), QStringLiteral(".shtm"),
     QStringLiteral(".hta"),
     QStringLiteral(".asp"), QStringLiteral(".aspx"),
     QStringLiteral(".css"), QStringLiteral(".js"), QStringLiteral(".json"), QStringLiteral(".mjs"),
     QStringLiteral(".jsm"), QStringLiteral(".jsp"),
     QStringLiteral(".php"), QStringLiteral(".php3"), QStringLiteral(".php4"), QStringLiteral(".php5"),
     QStringLiteral(".phps"), QStringLiteral(".phpt"), QStringLiteral(".phtml"),
     QStringLiteral(".xml"), QStringLiteral(".xhtml"), QStringLiteral(".xht"), QStringLiteral(".xul"),
     QStringLiteral(".kml"), QStringLiteral(".xaml"), QStringLiteral(".xsml")
    },
    {QStringLiteral("public script"),
     QStringLiteral(".sh"), QStringLiteral(".bsh"), QStringLiteral(".bash"), QStringLiteral(".bat"),
     QStringLiteral(".cmd"), QStringLiteral(".nsi"), QStringLiteral(".nsh"), QStringLiteral(".lua"),
     QStringLiteral(".pl"), QStringLiteral(".pm"), QStringLiteral(".py")
    },
    {QStringLiteral("property script"),
     QStringLiteral(".rc"), QStringLiteral(".as"), QStringLiteral(".mx"),
     QStringLiteral(".vb"), QStringLiteral(".vbs")
    },
    {QStringLiteral("fortran, TeX, SQL"),
     QStringLiteral(".f"), QStringLiteral(".for"), QStringLiteral(".f90"), QStringLiteral(".f95"),
     QStringLiteral(".f2k"), QStringLiteral(".tex"), QStringLiteral(".sql")
    },
    {QStringLiteral("misc"),
     QStringLiteral(".nfo"), QStringLiteral(".mak")
    },
    {QStringLiteral("customize")}
};

// =============================================================================
// Dialog setup
// =============================================================================

void RegExtDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    // Build the dialog UI programmatically (no .ui needed).
    // In Qt, RTL is handled via QLayout::setLayoutDirection.
    myCreateDialogBox();   // StaticDialog base method
}

intptr_t RegExtDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    NppParameters& nppParam = NppParameters::getInstance();

    switch (message)
    {
    case WM_INITDIALOG:
    {
        populateRegisteredExts();
        populateDefSupportedExts();

        // Find child widgets and disable initially
        auto addBtn = findChild<QPushButton*>(QString::number(IDC_ADDFROMLANGEXT_BUTTON));
        auto removeBtn = findChild<QPushButton*>(QString::number(IDC_REMOVEEXT_BUTTON));
        if (addBtn)    addBtn->setEnabled(false);
        if (removeBtn) removeBtn->setEnabled(false);

        if (!nppParam.isAdmin())
        {
            auto langList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANG_LIST));
            auto langExtList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANGEXT_LIST));
            auto regList = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
            if (langList)   langList->setEnabled(false);
            if (langExtList) langExtList->setEnabled(false);
            if (regList)    regList->setEnabled(false);
        }
        else
        {
            auto adminMsg = findChild<QLabel*>(QString::number(IDC_ADMINMUSTBEONMSG_STATIC));
            if (adminMsg) adminMsg->hide();

            auto customEdit = findChild<QLineEdit*>(QString::number(IDC_CUSTOMEXT_EDIT));
            if (customEdit) customEdit->setMaxLength(extNameMax - 1);
        }

        NppDarkMode::autoSubclassAndThemeChildWidgets(this);
        return TRUE;
    }

    case WM_CTLCOLOREDIT:
    {
        return reinterpret_cast<intptr_t>(NppDarkMode::onCtlColorCtrl(reinterpret_cast<QPaintDevice*>(wParam)));
    }

    case WM_CTLCOLORLISTBOX:
    {
        return reinterpret_cast<intptr_t>(NppDarkMode::onCtlColorListbox(wParam, lParam));
    }

    case WM_CTLCOLORDLG:
    {
        return reinterpret_cast<intptr_t>(NppDarkMode::onCtlColorDlg(reinterpret_cast<QPaintDevice*>(wParam)));
    }

    case WM_CTLCOLORSTATIC:
    {
        auto hdcStatic = reinterpret_cast<QPaintDevice*>(wParam);
        // In Qt we identify static labels by objectName, not GetDlgCtrlID.
        // Use the static text label as a proxy.
        bool isStaticText = (wParam == IDC_SUPPORTEDEXTS_STATIC) || (wParam == IDC_REGISTEREDEXTS_STATIC);
        if (isStaticText)
            return reinterpret_cast<intptr_t>(NppDarkMode::onCtlColorDlgStaticText(hdcStatic, nppParam.isAdmin()));
        return reinterpret_cast<intptr_t>(NppDarkMode::onCtlColorDlg(hdcStatic));
    }

    case WM_PRINTCLIENT:
    {
        if (NppDarkMode::isEnabled())
            return TRUE;
        break;
    }

    case WM_COMMAND:
    {
        const int low = LOWORD(wParam);
        const int high = HIWORD(wParam);

        // Handle double-click on language-ext or registered-exts list
        if (low == IDC_REGEXT_LANGEXT_LIST || low == IDC_REGEXT_REGISTEREDEXTS_LIST)
        {
            if (high == LBN_DBLCLK)
            {
                // Check whether click happened on an item (not empty area)
                auto listWidget = findChild<QListWidget*>(QString::number(low));
                if (listWidget && listWidget->currentRow() != -1)
                {
                    if (low == IDC_REGEXT_LANGEXT_LIST)
                        onAddFromLangExtClicked();
                    else
                        onRemoveExtClicked();
                }
                return TRUE;
            }
        }

        switch (low)
        {
        case IDC_ADDFROMLANGEXT_BUTTON:
        {
            onAddFromLangExtClicked();
            return TRUE;
        }

        case IDC_REMOVEEXT_BUTTON:
        {
            onRemoveExtClicked();
            return TRUE;
        }

        case IDCANCEL:
        {
            accept();
            return TRUE;
        }
        }

        if (high == EN_CHANGE && low == IDC_CUSTOMEXT_EDIT)
        {
            onCustomExtTextChanged(QString());
            return TRUE;
        }

        if (high == LBN_SELCHANGE)
        {
            onLangListCurrentRowChanged(-1);
            return TRUE;
        }

        [[fallthrough]];
    }

    default:
        return FALSE;
    }
    return FALSE;
}

// =============================================================================
// Qt slots — wired from the dialog's child widgets
// =============================================================================

void RegExtDlg::onAddFromLangExtClicked()
{
    writeNppPath();

    QString ext2Add;
    if (!_isCustomize)
    {
        auto langExtList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANGEXT_LIST));
        if (!langExtList) return;
        auto item = langExtList->currentItem();
        if (!item) return;
        ext2Add = item->text();
        if (ext2Add.length() > extNameMax - 1)
            return;
        addExt(ext2Add);
        delete langExtList->takeItem(langExtList->currentRow());
    }
    else
    {
        auto customEdit = findChild<QLineEdit*>(QString::number(IDC_CUSTOMEXT_EDIT));
        if (!customEdit) return;
        ext2Add = customEdit->text();
        auto regList = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
        if (regList && regList->findItems(ext2Add, Qt::MatchExactly).size() > 0)
            return;
        addExt(ext2Add);
        customEdit->clear();
    }

    auto regList = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
    if (regList)
        regList->addItem(ext2Add);

    auto addBtn = findChild<QPushButton*>(QString::number(IDC_ADDFROMLANGEXT_BUTTON));
    if (addBtn) addBtn->setEnabled(false);
}

void RegExtDlg::onRemoveExtClicked()
{
    auto regList = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
    if (!regList) return;
    auto item = regList->currentItem();
    if (!item) return;

    QString ext2Sup = item->text();
    if (ext2Sup.length() > extNameMax - 1)
        return;

    if (deleteExts(ext2Sup))
    {
        delete regList->takeItem(regList->currentRow());
    }

    auto langList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANG_LIST));
    auto removeBtn = findChild<QPushButton*>(QString::number(IDC_REMOVEEXT_BUTTON));
    if (removeBtn) removeBtn->setEnabled(false);

    if (langList)
    {
        int langIndex = langList->currentRow();
        if (langIndex != -1)
        {
            for (int i = 1; i < nbExtMax; ++i)
            {
                if (defExtArray[langIndex][i].isEmpty())
                    break;
                if (ext2Sup.compare(defExtArray[langIndex][i], Qt::CaseInsensitive) == 0)
                {
                    auto langExtList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANGEXT_LIST));
                    if (langExtList)
                        langExtList->addItem(ext2Sup);
                    return;
                }
            }
        }
    }
}

void RegExtDlg::onCustomExtTextChanged(const QString& /*text*/)
{
    auto customEdit = findChild<QLineEdit*>(QString::number(IDC_CUSTOMEXT_EDIT));
    auto addBtn = findChild<QPushButton*>(QString::number(IDC_ADDFROMLANGEXT_BUTTON));
    if (!customEdit || !addBtn) return;

    QString text = customEdit->text();
    // If single char without dot, prepend dot
    if (text.length() == 1 && text[0] != '.')
    {
        text = "." + text;
        customEdit->setText(text);
        customEdit->setSelection(2, 0);  // place cursor after dot
    }
    addBtn->setEnabled(text.length() > 1);
}

void RegExtDlg::onLangListCurrentRowChanged(int /*row*/)
{
    auto langList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANG_LIST));
    auto langExtList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANGEXT_LIST));
    auto customEdit = findChild<QLineEdit*>(QString::number(IDC_CUSTOMEXT_EDIT));
    auto addBtn = findChild<QPushButton*>(QString::number(IDC_ADDFROMLANGEXT_BUTTON));
    if (!langList) return;

    int i = langList->currentRow();
    if (i == -1) return;

    if (defExtArray[i][0] == QStringLiteral("customize"))
    {
        if (langExtList) langExtList->hide();
        if (customEdit) customEdit->show();
        _isCustomize = true;
    }
    else
    {
        if (_isCustomize)
        {
            if (langExtList) langExtList->show();
            if (customEdit) customEdit->hide();
            _isCustomize = false;
        }
        if (langExtList)
        {
            langExtList->clear();
            auto regList = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
            for (int j = 1; j < nbExtMax; ++j)
            {
                if (defExtArray[i][j].isEmpty())
                    break;
                bool alreadyRegistered = false;
                if (regList)
                {
                    for (int k = 0; k < regList->count(); ++k)
                    {
                        if (regList->item(k)->text().compare(defExtArray[i][j], Qt::CaseInsensitive) == 0)
                        {
                            alreadyRegistered = true;
                            break;
                        }
                    }
                }
                if (!alreadyRegistered)
                    langExtList->addItem(defExtArray[i][j]);
            }
        }
    }

    if (addBtn) addBtn->setEnabled(false);
}

void RegExtDlg::onLangExtListCurrentRowChanged(int row)
{
    auto addBtn = findChild<QPushButton*>(QString::number(IDC_ADDFROMLANGEXT_BUTTON));
    if (addBtn) addBtn->setEnabled(row != -1);
}

void RegExtDlg::onRegisteredExtsListCurrentRowChanged(int row)
{
    auto removeBtn = findChild<QPushButton*>(QString::number(IDC_REMOVEEXT_BUTTON));
    if (removeBtn) removeBtn->setEnabled(row != -1);
}

void RegExtDlg::onAccepted()
{
    accept();
}

// =============================================================================
// Private helpers
// =============================================================================

void RegExtDlg::populateRegisteredExts()
{
    // On Unix, registered extensions are stored in QSettings under
    // [Extensions] group. Each value is the extension name.
    QSettings settings(QStringLiteral("Notepad++"), QStringLiteral("FileAssociations"));
    settings.beginGroup(QStringLiteral("Extensions"));
    const QStringList exts = settings.childKeys();
    auto listWidget = findChild<QListWidget*>(QString::number(IDC_REGEXT_REGISTEREDEXTS_LIST));
    if (listWidget)
    {
        for (const QString& ext : exts)
        {
            if (ext.startsWith('.'))
                listWidget->addItem(ext);
        }
    }
    settings.endGroup();
}

void RegExtDlg::populateDefSupportedExts()
{
    auto langList = findChild<QListWidget*>(QString::number(IDC_REGEXT_LANG_LIST));
    if (langList)
    {
        for (int i = 0; i < nbSupportedLang; ++i)
            langList->addItem(defExtArray[i][0]);
    }
}

void RegExtDlg::addExt(const QString& ext)
{
    // Store extension association in QSettings on Unix.
    QSettings settings(QStringLiteral("Notepad++"), QStringLiteral("FileAssociations"));
    settings.beginGroup(QStringLiteral("Extensions"));

    // If extension already has a value, back it up under nppBackup key
    QString existingValue = settings.value(ext, QString()).toString();
    if (!existingValue.isEmpty())
        settings.setValue(nppBackup, existingValue);

    settings.setValue(ext, nppName);
    settings.endGroup();

    // TODO: [unhandled] On Unix, also create/update the .desktop file in
    // ~/.local/share/applications/ and run update-desktop-database for
    // MIME-type registration. This requires non-trivial file I/O and
    // DBus/XDG calls that go beyond a simple QSettings shim.
    Q_UNUSED(writeNppPath);
}

bool RegExtDlg::deleteExts(const QString& ext2Delete)
{
    QSettings settings(QStringLiteral("Notepad++"), QStringLiteral("FileAssociations"));
    settings.beginGroup(QStringLiteral("Extensions"));

    // Count sub-keys and values — on Unix we only have this flat group.
    int nbValue = settings.childKeys().size();

    if (nbValue <= 1)
    {
        settings.remove(ext2Delete);
    }
    else
    {
        // Restore backed-up value if present
        QString backupVal = settings.value(nppBackup, QString()).toString();
        if (!backupVal.isEmpty())
        {
            settings.setValue(ext2Delete, backupVal);
            settings.remove(nppBackup);
        }
        else
        {
            settings.remove(ext2Delete);
        }
    }
    settings.endGroup();
    return true;
}

void RegExtDlg::writeNppPath()
{
    // On Unix, write the Notepad++ path into the .desktop file stub
    // stored in QSettings. Full implementation would write a .desktop
    // entry to ~/.local/share/applications/.
    QString nppPath = QCoreApplication::applicationFilePath();
    QSettings settings(QStringLiteral("Notepad++"), QStringLiteral("FileAssociations"));
    settings.setValue(QStringLiteral("Notepad++_file/shell/open/command"), "\"" + nppPath + "\" \"%1\"");
    settings.setValue(QStringLiteral("Notepad++_file/DefaultIcon"), "\"" + nppPath + "\",0");
    settings.setValue(QStringLiteral("Notepad++_file"), nppDoc);

    // TODO: [unhandled] Create ~/.local/share/applications/notepad-plus-plus.desktop
    // with proper MIME types and XDG associations. Requires QSaveFile + desktop-entry
    // format generation and update-desktop-database call.
}

int RegExtDlg::getNbSubKey(const QString& /*keyPath*/) const
{
    // On Unix, we use a flat QSettings structure, not nested keys.
    // Return 0 — Win32-specific HKEY_CLASSES_ROOT enumeration is N/A.
    return 0;
}

int RegExtDlg::getNbSubValue(const QString& /*keyPath*/) const
{
    // On Unix, sub-value enumeration maps to QSettings::childKeys().
    QSettings settings(QStringLiteral("Notepad++"), QStringLiteral("FileAssociations"));
    settings.beginGroup(QStringLiteral("Extensions"));
    int nb = settings.childKeys().size();
    settings.endGroup();
    return nb;
}
