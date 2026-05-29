// Qt stub for CustomFileDialog on non-Win32 builds.

#include "CustomFileDialog.h"

#include <QFileDialog>
#include <QString>
#include <QWidget>

#include <vector>

class CustomFileDialog::Impl
{
public:
	HWND _hwndOwner = nullptr;
	QString _title;
	QString _folder;
	QString _defFileName;
	QString _defExt;
	QString _checkboxLabel;
	QString _fileTypeCheckboxLabel;
	bool _isCheckboxActive = false;
	bool _enableFileTypeCheckbox = false;
	bool _fileTypeCheckboxValue = false;
	bool _hasReadonly = false;
	bool _openCopyAfterSaveAsCopy = false;
	unsigned char _savingAsCopyInfo = 0;
	int _fileTypeIndex = 0;

	bool getCheckboxState(int) const { return _isCheckboxActive; }
};

CustomFileDialog::CustomFileDialog(HWND hwnd) : _impl{ std::make_unique<Impl>() }
{
	_impl->_hwndOwner = hwnd;
}

CustomFileDialog::~CustomFileDialog() = default;

void CustomFileDialog::setTitle(const wchar_t* title)
{
	_impl->_title = QString::fromWCharArray(title ? title : L"");
}

void CustomFileDialog::setExtFilter(const wchar_t*, const wchar_t*) {}

void CustomFileDialog::setExtFilter(const wchar_t*, std::initializer_list<const wchar_t*>) {}

void CustomFileDialog::setDefExt(const wchar_t* ext)
{
	_impl->_defExt = QString::fromWCharArray(ext ? ext : L"");
}

void CustomFileDialog::setDefFileName(const wchar_t* fn)
{
	_impl->_defFileName = QString::fromWCharArray(fn ? fn : L"");
}

void CustomFileDialog::setFolder(const wchar_t* folder)
{
	_impl->_folder = QString::fromWCharArray(folder ? folder : L"");
}

void CustomFileDialog::setCheckbox(const wchar_t* text, bool isActive)
{
	_impl->_checkboxLabel = QString::fromWCharArray(text ? text : L"");
	_impl->_isCheckboxActive = isActive;
}

void CustomFileDialog::setExtIndex(int extTypeIndex)
{
	_impl->_fileTypeIndex = extTypeIndex;
}

void CustomFileDialog::setSaveAsCopy(bool isSavingAsCopy)
{
	_impl->_savingAsCopyInfo = isSavingAsCopy ? 0x02 : 0;
}

bool CustomFileDialog::getOpenTheCopyAfterSaveAsCopy()
{
	return _impl->_openCopyAfterSaveAsCopy;
}

bool CustomFileDialog::getCheckboxState() const
{
	return _impl->_isCheckboxActive;
}

bool CustomFileDialog::isReadOnly() const
{
	return _impl->_hasReadonly;
}

void CustomFileDialog::enableFileTypeCheckbox(const std::wstring& text, bool value)
{
	_impl->_fileTypeCheckboxLabel = QString::fromStdWString(text);
	_impl->_enableFileTypeCheckbox = true;
	_impl->_fileTypeCheckboxValue = value;
}

bool CustomFileDialog::getFileTypeCheckboxValue() const
{
	return _impl->_fileTypeCheckboxValue;
}

static QWidget* ownerWidget(HWND)
{
	return nullptr;
}

std::wstring CustomFileDialog::doSaveDlg()
{
	const QString filter = _impl->_defExt.isEmpty() ? QString() : QStringLiteral("(*.%1)").arg(_impl->_defExt);
	QString path = QFileDialog::getSaveFileName(ownerWidget(_impl->_hwndOwner), _impl->_title, _impl->_folder, filter);
	return path.isEmpty() ? std::wstring() : path.toStdWString();
}

std::wstring CustomFileDialog::doOpenSingleFileDlg()
{
	QString path = QFileDialog::getOpenFileName(ownerWidget(_impl->_hwndOwner), _impl->_title, _impl->_folder);
	return path.isEmpty() ? std::wstring() : path.toStdWString();
}

std::vector<std::wstring> CustomFileDialog::doOpenMultiFilesDlg()
{
	const QStringList paths = QFileDialog::getOpenFileNames(ownerWidget(_impl->_hwndOwner), _impl->_title, _impl->_folder);
	std::vector<std::wstring> result;
	result.reserve(static_cast<size_t>(paths.size()));
	for (const QString& p : paths)
		result.push_back(p.toStdWString());
	return result;
}

std::wstring CustomFileDialog::pickFolder()
{
	const QString path = QFileDialog::getExistingDirectory(ownerWidget(_impl->_hwndOwner), _impl->_title, _impl->_folder);
	return path.isEmpty() ? std::wstring() : path.toStdWString();
}
