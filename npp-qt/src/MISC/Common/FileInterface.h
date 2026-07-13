// This file is part of Notepad++ project
// Copyright (C)2021 Pavel Nedev (pg.nedev@gmail.com)

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Ported to Qt6

#pragma once

#include <QFile>
#include <QString>
#include <cstdint>

class NppIO_File
{
public:
    explicit NppIO_File(const QString& fname);
    NppIO_File() = default;

    NppIO_File(const NppIO_File&) = delete;
    NppIO_File& operator=(const NppIO_File&) = delete;

    ~NppIO_File() {
        close();
    }

    bool isOpened() const { return _file && _file->isOpen(); }

    void close();
    bool write(const void* wbuf, size_t bufSize);

    bool writeStr(const QByteArray& str) {
        return write(str.constData(), static_cast<size_t>(str.size()));
    }

    QString errorString() const {
        return _errorString;
    }

private:
    QScopedPointer<QFile> _file;
    QString _path;
    bool _written = false;
    QString _errorString;
};

// Alias for source compatibility
using Win32_IO_File = NppIO_File;
