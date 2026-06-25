// Semantic Lift: Win32 CustomFileDialog → Qt6 CustomFileDialog
// Original: PowerEditor/src/WinControls/OpenSaveFileDialog/CustomFileDialog.cpp
// Target: npp-qt/src/WinControls/CustomFileDialog.cpp

#include "CustomFileDialog.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDialogButtonBox>

// -----------------------------------------------------------------------
// NPP parameter persistence (Qt settings-based)
// -----------------------------------------------------------------------
namespace NppParameters {
    static const QString orgName  = QStringLiteral("Notepad++");
    static const QString appName  = QStringLiteral("npp-qt");

    inline QString lastUsedDir() {
        return QSettings(orgName, appName).value(QStringLiteral("lastUsedDir"), QString()).toString();
    }

    inline void setLastUsedDir(const QString& dir) {
        QSettings(orgName, appName).setValue(QStringLiteral("lastUsedDir"), dir);
    }
}

// -----------------------------------------------------------------------
// FluentColor enum (mirrors NPP toolbar colour settings)
// -----------------------------------------------------------------------
enum class FluentColor {
    accent = 0, red, green, blue, purple, cyan, olive, yellow, custom, defaultColor
};

// -----------------------------------------------------------------------
// CustomFileDialog
// -----------------------------------------------------------------------

CustomFileDialog::CustomFileDialog(QWidget* parent)
    : QObject(parent)
    , _parentWidget(parent)
{
    // Seed fallback folder from the persisted last-used directory.
    _fallbackFolder = NppParameters::lastUsedDir();
    if (_fallbackFolder.isEmpty()) {
        _fallbackFolder = QDir::homePath();
    }
}

CustomFileDialog::~CustomFileDialog() = default;

void CustomFileDialog::setTitle(const QString& title)
{
    _title = title;
}

void CustomFileDialog::setExtFilter(const QString& extText, const QString& exts)
{
    // Expand bare dots in extension patterns to *.<ext>
    QString expanded = expandExtPatterns(exts);

    // Check if this is a wildcard (*.*) filter.
    if (expanded.contains(QStringLiteral("*.*")) && !expanded.contains(u" "*_s)) {
        _wildcardIndex = _filterSpec.size();
    }

    _filterSpec.append({extText, expanded});
}

QString CustomFileDialog::expandExtPatterns(const QString& exts)
{
    QString result = exts;
    const QStringList parts = exts.split(u';', Qt::SkipEmptyParts);
    result.clear();
    for (const QString& part : parts) {
        QString p = part.trimmed();
        if (p.isEmpty()) continue;
        // If it starts with * but not *.*, pass through.
        // If it starts with . expand to *.<ext>.
        if (p.startsWith(QStringLiteral(".")) && !p.startsWith(QStringLiteral(".."))) {
            p = QStringLiteral("*") + p;
        }
        if (!result.isEmpty()) result += u';'_s;
        result += p;
    }
    return result;
}

void CustomFileDialog::setDefExt(const QString& ext)
{
    _defExt = ext;
}

void CustomFileDialog::setDefFileName(const QString& fileName)
{
    _initialFileName = fileName;
    // If the file name lacks an extension, append the default extension.
    if (!hasExt(fileName) && !_defExt.isEmpty()) {
        _initialFileName = fileName + u'.'_s + _defExt;
    }
}

void CustomFileDialog::setFolder(const QString& folder)
{
    _initialFolder = folder;
}

void CustomFileDialog::setExtIndex(int extTypeIndex)
{
    if (extTypeIndex >= 0 && extTypeIndex < _filterSpec.size()) {
        _fileTypeIndex = extTypeIndex;
    }
}

void CustomFileDialog::setCheckbox(const QString& text, bool isActive)
{
    _checkboxLabel = text;
    _checkboxActive = isActive;
}

void CustomFileDialog::enableFileTypeCheckbox(const QString& text, bool value)
{
    Q_ASSERT(!text.isEmpty());
    if (text.isEmpty()) return;
    _fileTypeCheckboxLabel = text;
    _fileTypeCheckboxEnabled = true;
    _fileTypeCheckboxValue = value;
}

void CustomFileDialog::setSaveAsCopy(bool isSavingAsCopy)
{
    _saveAsCopy = isSavingAsCopy;
}

bool CustomFileDialog::getOpenTheCopyAfterSaveAsCopy() const
{
    // On Win32 this checks if Shift was held when Save was pressed.
    // In Qt we check the Shift modifier via QApplication::keyboardModifiers().
    // This must be called immediately after the dialog is accepted, before
    // the event loop continues.
    return (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier) != 0;
}

bool CustomFileDialog::getCheckboxState() const
{
    // Returns state of IDC_FILE_CUSTOM_CHECKBOX. This is stored during show()
    // and retrieved via a signal or stored member. For now it's always false
    // as we use the Qt native dialog which doesn't expose custom checkboxes.
    // Subclasses can override to return a stored value.
    return false;
}

bool CustomFileDialog::getFileTypeCheckboxValue() const
{
    return _fileTypeCheckboxValue;
}

bool CustomFileDialog::isReadOnly() const
{
    return _hasReadonly;
}

QString CustomFileDialog::buildQtFilter() const
{
    QStringList filters;
    for (const auto& f : _filterSpec) {
        filters << QString(QStringLiteral("%1 (%2)")).arg(f.first, f.second);
    }
    return filters.join(QStringLiteral(";;"));
}

QString CustomFileDialog::firstExt(const QString& extSpec)
{
    if (extSpec.isEmpty()) return QString();
    // Return the first extension: everything from the first '.' up to
    // the first ';' (or end of string).
    int dotPos = extSpec.indexOf(u'.'_s);
    if (dotPos < 0) return extSpec;
    int semiPos = extSpec.indexOf(u';'_s, dotPos);
    if (semiPos < 0) return extSpec.mid(dotPos);
    return extSpec.mid(dotPos, semiPos - dotPos);
}

bool CustomFileDialog::replaceExt(QString& name, const QString& ext)
{
    if (name.isEmpty() || ext.isEmpty()) return false;
    int dotPos = name.lastIndexOf(u'.'_s);
    if (dotPos >= 0) {
        name.truncate(dotPos);
    }
    name += ext;
    return true;
}

bool CustomFileDialog::hasExt(const QString& name)
{
    return name.lastIndexOf(u'.'_s) >= 0;
}

QString CustomFileDialog::directoryOf(const QString& filePath)
{
    QFileInfo fi(filePath);
    return fi.absolutePath();
}

QString CustomFileDialog::resolveInitialFolder() const
{
    if (!_initialFolder.isEmpty() && QDir(_initialFolder).exists()) {
        return _initialFolder;
    }
    if (!_fallbackFolder.isEmpty() && QDir(_fallbackFolder).exists()) {
        return _fallbackFolder;
    }
    return QDir::homePath();
}

// -----------------------------------------------------------------------
// Dialog runners
// -----------------------------------------------------------------------

QString CustomFileDialog::doSaveDlg()
{
    QString startFolder = resolveInitialFolder();

    // Build the file name with extension if missing.
    QString suggestedName = _initialFileName;
    if (!hasExt(suggestedName) && _fileTypeIndex >= 0
        && _fileTypeIndex < _filterSpec.size()) {
        QString ext = firstExt(_filterSpec[_fileTypeIndex].second);
        if (!ext.contains(QStringLiteral("*.*"))) {
            suggestedName += ext;
        }
    }

    // Build the parent window handle for the native dialog.
    WId wid = 0;
    if (_parentWidget) {
        wid = _parentWidget->windowHandle()
              ? _parentWidget->windowHandle()->winId()
              : _parentWidget->winId();
    }

    QString selectedFilter;
    QString result = QFileDialog::getSaveFileName(
        _parentWidget,
        _title.isEmpty() ? QStringLiteral("Save") : _title,
        suggestedName.isEmpty()
            ? startFolder
            : (QFileInfo(suggestedName).isAbsolute() ? suggestedName
                                                     : startFolder + u'/' + suggestedName),
        buildQtFilter(),
        &selectedFilter,
        QFileDialog::Option(0));

    if (result.isEmpty()) {
        return QString();
    }

    // If user picked a filter type, record it.
    if (!selectedFilter.isEmpty()) {
        for (int i = 0; i < _filterSpec.size(); ++i) {
            QString filterLine = QString(QStringLiteral("%1 (%2)")).arg(_filterSpec[i].first, _filterSpec[i].second);
            if (filterLine == selectedFilter) {
                _fileTypeIndex = i;
                break;
            }
        }
    }

    // If the chosen file has no extension, append the default.
    if (!hasExt(result)) {
        if (_fileTypeIndex >= 0 && _fileTypeIndex < _filterSpec.size()) {
            QString ext = firstExt(_filterSpec[_fileTypeIndex].second);
            if (!ext.contains(QStringLiteral("*"))) {
                result += ext;
            }
        } else if (!_defExt.isEmpty()) {
            result += u'.'_s + _defExt;
        }
    }

    // Record the last used directory.
    QString usedFolder = directoryOf(result);
    NppParameters::setLastUsedDir(usedFolder);
    _lastUsedFolder = usedFolder;

    emit accepted(result);
    return result;
}

QString CustomFileDialog::doOpenSingleFileDlg()
{
    QString startFolder = resolveInitialFolder();

    WId wid = 0;
    if (_parentWidget) {
        wid = _parentWidget->windowHandle()
              ? _parentWidget->windowHandle()->winId()
              : _parentWidget->winId();
    }

    QString selectedFilter;
    QString result = QFileDialog::getOpenFileName(
        _parentWidget,
        _title.isEmpty() ? QStringLiteral("Open") : _title,
        startFolder,
        buildQtFilter(),
        &selectedFilter,
        QFileDialog::Option(0));

    if (result.isEmpty()) {
        return QString();
    }

    // Check if the result file is read-only.
    QFileInfo fi(result);
    _hasReadonly = !fi.isWritable();

    // Record last used directory.
    QString usedFolder = fi.absolutePath();
    NppParameters::setLastUsedDir(usedFolder);
    _lastUsedFolder = usedFolder;

    emit accepted(result);
    return result;
}

QStringList CustomFileDialog::doOpenMultiFilesDlg()
{
    QString startFolder = resolveInitialFolder();

    QStringList results = QFileDialog::getOpenFileNames(
        _parentWidget,
        _title.isEmpty() ? QStringLiteral("Open") : _title,
        startFolder,
        buildQtFilter(),
        nullptr,
        QFileDialog::Option(0));

    if (results.isEmpty()) {
        return QStringList();
    }

    // Record last used directory from the first selected file.
    QFileInfo fi(results.first());
    QString usedFolder = fi.absolutePath();
    NppParameters::setLastUsedDir(usedFolder);
    _lastUsedFolder = usedFolder;

    // Check read-only status of the first file.
    _hasReadonly = !fi.isWritable();

    for (const QString& r : results) {
        emit accepted(r);
    }
    return results;
}

QString CustomFileDialog::pickFolder()
{
    QString startFolder = resolveInitialFolder();

    QString result = QFileDialog::getExistingDirectory(
        _parentWidget,
        _title.isEmpty() ? QStringLiteral("Choose Folder") : _title,
        startFolder,
        QFileDialog::Option(0));

    if (result.isEmpty()) {
        return QString();
    }

    NppParameters::setLastUsedDir(result);
    _lastUsedFolder = result;

    emit accepted(result);
    return result;
}
