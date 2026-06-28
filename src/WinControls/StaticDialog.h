// Semantic Lift: Win32 StaticDialog → Qt6 QDialog implementation
// Original: PowerEditor/src/WinControls/StaticDialog/
// Target: npp-qt/src/WinControls/StaticDialog.h + .cpp

#pragma once

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRect>
#include <QPoint>
#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>
#include <QMoveEvent>

// =============================================================================
// StaticDialog — base dialog window (lifted from StaticDialog.h)
// =============================================================================

class StaticDialog : public QWidget
{
    Q_OBJECT

public:
    StaticDialog(QWidget* parent = nullptr);
    ~StaticDialog() override;

    // Dialog lifecycle
    virtual void create(int dialogID, bool isRTL = false, bool msgDestParent = true);
    virtual bool isCreated() const { return _isCreated; }

    // Positioning
    void goToCenter();
    void moveToCenter();

    // Display
    void display(bool toShow = true);
    void showModal();
    void showModeless();

    // Child widget helpers
    QRect childRect(int childId) const;
    void redrawChild(int childId);

    // Checkbox helpers
    bool isChecked(int checkControlID) const;
    void setChecked(int checkControlID, bool checked) const;

    // Size/position for DPI changes
    bool handleDpiChange();
    void updateGeometryForDpi(int newDpi);

    // Dialog resources
    int dialogId() const { return _dialogId; }
    void setDialogId(int id) { _dialogId = id; }

    bool isRtl() const { return _isRTL; }

    // Pure virtual — subclasses must implement
    virtual intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) = 0;

    // Optional override — default no-op
    virtual void destroy() {}

signals:
    void dialogClosed();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool event(QEvent* event) override;

    bool _isCreated = false;
    bool _isRTL = false;
    bool _msgDestParent = true;
    int _dialogId = -1;

    QRect _rc;  // Saved geometry for DPI changes

private:
    void applyFontSize(int fontSize);
};

// =============================================================================
// RunDlg — "Run" command dialog (lifted from RunDlg.h)
// =============================================================================

class RunDialog : public StaticDialog
{
    Q_OBJECT

public:
    RunDialog();
    ~RunDialog() override = default;

    void doDialog(bool isRTL = false);

    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

public slots:
    void onRun();
    void onBrowse();

private:
    void addToHistory(const QString& text);
    void removeFromHistory(const QString& text);

    // Variable expansion
    static QString expandVariables(const QString& input);

    QLineEdit* _commandEdit = nullptr;
    QComboBox* _historyCombo = nullptr;
    QPushButton* _runButton = nullptr;
    QPushButton* _cancelButton = nullptr;
    QPushButton* _browseButton = nullptr;
    QStringList _history;
};
