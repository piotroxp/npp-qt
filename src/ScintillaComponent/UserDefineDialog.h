// UserDefineDialog.h — Qt6 stub for Notepad++ User Define Language dialog
#pragma once

#include "StaticDialog.h"
#include <QString>

class SharedParametersDialog : public StaticDialog
{
public:
    SharedParametersDialog() = default;
    virtual ~SharedParametersDialog() = default;
};

class UserDefineDialog : public SharedParametersDialog
{
public:
    UserDefineDialog() = default;
    virtual ~UserDefineDialog() = default;

    static UserDefineDialog* getUserDefineDlg();  // singleton accessor

    void doDialog(bool isRTL = false);
    void destroy() override;
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    void setCurrentLangName(const QString& langName);
    QString getCurrentLangName() const;

    void setKeywords(int listId, const QString& keywords);
    QString getKeywords(int listId) const;

    void setComment(const QString& blockOpen, const QString& blockClose,
                    const QString& lineOpen, const QString& prefix);

    void setOperators(const QString& opers);

    void setFolderProperties(const QString& beginFold, const QString& endFold,
                              const QString& beginFoldRegex, const QString& endFoldRegex);

    void setEnabled(bool enable);
    bool isEnabled() const;

private:
    QString _langName;
    bool _enabled = true;
};
