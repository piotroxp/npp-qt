// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
// UserDefineDialog: User-defined language (UDL) editor dialog
// Win32→Qt6: Complex dialog with tabbed controls for keywords, operators,
//   folders, comment styles, and styling (foreground/background/font).
// This is a large port (2105 lines) — full implementation pending.
// Stub provides the interface; full Qt port requires the UDL data model.
#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include "ScintillaComponent.h"
#include "ColourPicker.h"
#include "NppDarkMode.h"

class UserDefineDialog : public QWidget
{
    Q_OBJECT
public:
    UserDefineDialog();
    ~UserDefineDialog() override;

    void doDialog(bool willBeShown = true, bool isRTL = false);
    static void doUserDefineDlg(bool willBeShown = true, bool isRTL = false);
    static UserDefineDialog* getUserDefineDlg() { return &_self; }
    void setScintilla(ScintillaComponent* sci) { Q_UNUSED(sci); }
    void init(QWidget* parent);
    void updateDialog();
    void setKeywords(size_t listIndex, const QString& keywords);

signals:
    void userLangChanged();

public slots:
    void onApply();
    void onOk();
    void onCancel();
    void onTabChanged(int index);

private:
    void setupKeywordsTab();
    void setupOperatorsTab();
    void setupFoldersTab();
    void setupCommentTab();
    void setupStylingTab();

    QTabWidget* _tab = nullptr;
    ScintillaComponent* _pScintilla = nullptr;
    static UserDefineDialog _self;
    QLineEdit* _extEdit = nullptr;
    QTextEdit* _keywordsEdits[8];
    QPushButton* _extColorBtn = nullptr;
    QLabel* _extColorLabel = nullptr;
    bool _isRTL = false;
};
