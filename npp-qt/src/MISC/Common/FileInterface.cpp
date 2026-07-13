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

#include "FileInterface.h"

#include <QDir>
#include <QFileInfo>

#include "Common.h"

NppIO_File::NppIO_File(const QString& fname)
{
    if (fname.isEmpty())
        return;

    _path = fname;
    _file.reset(new QFile(fname));

    // Check if file exists
    QFileInfo fi(fname);
    bool fileExists = fi.exists() && fi.isFile();
    bool isFromNetwork = fname.startsWith(QStringLiteral("//")) ||
                         fname.startsWith(QStringLiteral("\\\\"));

    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;

    if (!_file->open(mode))
    {
        _errorString = _file->errorString();
        _file.reset();
        return;
    }

    _written = false;
}

void NppIO_File::close()
{
    if (_file)
    {
        if (_written)
        {
            _file->flush();
        }
        _file->close();
        _file.reset();
    }
    _written = false;
}

bool NppIO_File::write(const void* wbuf, size_t bufSize)
{
    if (!isOpened() || wbuf == nullptr || bufSize == 0)
        return false;

    const char* data = static_cast<const char*>(wbuf);
    qint64 written = _file->write(data, static_cast<qint64>(bufSize));

    if (written != static_cast<qint64>(bufSize))
    {
        _errorString = _file->errorString();
        return false;
    }

    _written = true;
    return true;
}
