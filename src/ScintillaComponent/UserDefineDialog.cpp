// UserDefineDialog.cpp — Qt6 stub implementation
#include "ScintillaComponent/UserDefineDialog.h"

void UserDefineDialog::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
}

void UserDefineDialog::destroy()
{
}

void UserDefineDialog::setCurrentLangName(const QString& langName)
{
    _langName = langName;
}

QString UserDefineDialog::getCurrentLangName() const
{
    return _langName;
}

void UserDefineDialog::setKeywords(int listId, const QString& keywords)
{
    Q_UNUSED(listId);
    Q_UNUSED(keywords);
}

QString UserDefineDialog::getKeywords(int listId) const
{
    Q_UNUSED(listId);
    return QString();
}

void UserDefineDialog::setComment(const QString& blockOpen, const QString& blockClose,
                                   const QString& lineOpen, const QString& prefix)
{
    Q_UNUSED(blockOpen);
    Q_UNUSED(blockClose);
    Q_UNUSED(lineOpen);
    Q_UNUSED(prefix);
}

void UserDefineDialog::setOperators(const QString& opers)
{
    Q_UNUSED(opers);
}

void UserDefineDialog::setFolderProperties(const QString& beginFold, const QString& endFold,
                                            const QString& beginFoldRegex, const QString& endFoldRegex)
{
    Q_UNUSED(beginFold);
    Q_UNUSED(endFold);
    Q_UNUSED(beginFoldRegex);
    Q_UNUSED(endFoldRegex);
}

void UserDefineDialog::setEnabled(bool enable)
{
    _enabled = enable;
}

bool UserDefineDialog::isEnabled() const
{
    return _enabled;
}

// static
UserDefineDialog* UserDefineDialog::getUserDefineDlg()
{
    static UserDefineDialog instance;
    return &instance;
}

intptr_t UserDefineDialog::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(message);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return 0;
}
