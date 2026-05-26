// MISC/Common/FileInterface.h - Qt6 port of Notepad++ file interface
#pragma once

#include <QString>
#include <QFile>

class Win32_IO_File
{
public:
    explicit Win32_IO_File(const QString& fname);
    Win32_IO_File() = delete;
    Win32_IO_File(const Win32_IO_File&) = delete;
    Win32_IO_File& operator=(const Win32_IO_File&) = delete;
    ~Win32_IO_File();

    bool isOpened() const { return _file.isOpen(); }
    void close();
    bool write(const void* wbuf, size_t buf_size);
    bool writeStr(const QString& str) { return write(str.toUtf8().constData(), str.toUtf8().size()); }
    int getLastErrorCode() const { return _dwErrorCode; }

private:
    QFile _file;
    bool _written = false;
    QString _path;
    int _dwErrorCode = 0;
};