// MISC/Common/FileInterface.cpp - Qt6 port of Notepad++ file interface
#include "FileInterface.h"
#include <QFile>
#include <QTextStream>

Win32_IO_File::Win32_IO_File(const QString& fname)
{
    if (fname.isEmpty()) return;

    _path = fname;
    _file.setFileName(fname);

    if (!_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        _dwErrorCode = 1;
    }
}

Win32_IO_File::~Win32_IO_File()
{
    close();
}

void Win32_IO_File::close()
{
    _dwErrorCode = 0;
    if (_file.isOpen()) {
        if (_written) {
            _file.flush();
        }
        _file.close();
    }
}

bool Win32_IO_File::write(const void* wbuf, size_t buf_size)
{
    _dwErrorCode = 0;

    if (!_file.isOpen() || wbuf == nullptr)
        return false;

    qint64 written = _file.write(static_cast<const char*>(wbuf), buf_size);
    if (written == -1) {
        _dwErrorCode = 1;
        return false;
    }

    _written = true;
    return static_cast<size_t>(written) == buf_size;
}