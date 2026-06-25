// Semantic Lift: Win32 CustomFileDialog → Qt6 CustomFileDialog
// Original: PowerEditor/src/WinControls/OpenSaveFileDialog/CustomFileDialog.cpp
// Target: npp-qt/src/WinControls/CustomFileDialog.h + .cpp

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QComboBox>
#include <QAbstractButton>

// -----------------------------------------------------------------------
// CustomFileDialog — Qt6 wrapper around QFileDialog with NPP-specific
// extensions: encoding-aware file filters, recent folder memory,
// Save As Copy mode, and type-specific filter checkbox.
//
// Mirrors the Win32 IFileDialog-based CustomFileDialog from NPP.
// -----------------------------------------------------------------------

class CustomFileDialog : public QObject
{
    Q_OBJECT

public:
    // Construct with an optional parent widget.
    explicit CustomFileDialog(QWidget* parent = nullptr);
    ~CustomFileDialog() override;

    // Set the dialog window title.
    void setTitle(const QString& title);

    // Add a file type filter.
    // extText: human-readable name (e.g., "C++ Source Files")
    // exts: wildcard patterns separated by semicolons (e.g., "*.cpp;*.h")
    void setExtFilter(const QString& extText, const QString& exts);

    // Add a file type filter from an initializer list of patterns.
    template <typename... T>
    void setExtFilter(const QString& extText, T... exts);

    // Set the default file extension (e.g., "txt").
    void setDefExt(const QString& ext);

    // Set the initial file name (pre-fill the save/open name field).
    void setDefFileName(const QString& fileName);

    // Set the initial directory to open the dialog in.
    void setFolder(const QString& folder);

    // Set the initial index in the filter combo (0-based).
    void setExtIndex(int extTypeIndex);

    // Add a custom checkbox with a label.
    void setCheckbox(const QString& text, bool isActive = true);

    // Enable a checkbox that locks the file type to *.*.
    // When checked, reverts to the previously selected file type.
    void enableFileTypeCheckbox(const QString& text, bool value = false);

    // Save As Copy mode: the file is saved but the original remains open.
    void setSaveAsCopy(bool isSavingAsCopy);

    // Returns true if Shift was held when the Save As Copy dialog was confirmed.
    bool getOpenTheCopyAfterSaveAsCopy() const;

    // Returns the state of the custom checkbox.
    bool getCheckboxState() const;

    // Returns the state of the file type lock checkbox.
    bool getFileTypeCheckboxValue() const;

    // Returns true if the chosen file has the read-only attribute.
    bool isReadOnly() const;

    // Run a Save dialog. Returns the chosen file path or empty string on cancel.
    QString doSaveDlg();

    // Run an Open dialog (single file). Returns the chosen file path or empty.
    QString doOpenSingleFileDlg();

    // Run an Open dialog (multiple files). Returns list of chosen paths.
    QStringList doOpenMultiFilesDlg();

    // Run a folder-picker dialog. Returns the chosen directory path.
    QString pickFolder();

signals:
    // Emitted when the dialog is accepted (user pressed OK/Save/Open).
    void accepted(const QString& fileName);

    // Emitted when the dialog is rejected (user pressed Cancel or closed).
    void rejected();

    // Emitted when the folder changes (e.g., user navigates to a different directory).
    void folderChanged(const QString& folderPath);

private:
    QWidget* _parentWidget = nullptr;
    QString _title;
    QString _defExt;
    QString _initialFileName;
    QString _initialFolder;
    QString _fallbackFolder;
    QString _checkboxLabel;
    bool _checkboxActive = true;
    bool _saveAsCopy = false;
    bool _hasReadonly = false;
    bool _fileTypeCheckboxEnabled = false;
    bool _fileTypeCheckboxValue = false;
    QString _fileTypeCheckboxLabel;

    int _fileTypeIndex = -1;   // preferred filter index (0-based)
    int _wildcardIndex = -1;  // index of the *.* filter (-1 if none)

    // Internal filter list: {display name, extensions string}
    QVector<QPair<QString, QString>> _filterSpec;

    // The last folder used (persisted via NppParameters on Win32).
    QString _lastUsedFolder;

    // Internal helper to build Qt-compatible filter string.
    QString buildQtFilter() const;

    // Apply wildcard star expansion to patterns that lack it.
    static QString expandExtPatterns(const QString& exts);

    // Returns the first extension from an extension spec string.
    static QString firstExt(const QString& extSpec);

    // Replace or append extension on a file name.
    static bool replaceExt(QString& name, const QString& ext);

    // Returns true if the file name has an extension.
    static bool hasExt(const QString& name);

    // Get the directory part of a path.
    static QString directoryOf(const QString& filePath);

    // Resolve the effective initial directory.
    QString resolveInitialFolder() const;

    // NPP GUI settings keys (Qt-side stubs).
    // On Win32 these read/write NppParameters. Here we use QSettings.
    static QString nppLastUsedDir();
    static void setNppLastUsedDir(const QString& dir);
};

// -----------------------------------------------------------------------
// Template implementation
// -----------------------------------------------------------------------
template <typename... T>
void CustomFileDialog::setExtFilter(const QString& extText, T... exts)
{
    QStringList patterns = {exts...};
    setExtFilter(extText, patterns.join(u';'));
}
