// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

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

#include <QString>
#include <QVector>

struct EncodingUnit {
    int _codePage = 0;
    const char* _aliasList = nullptr;
};

class EncodingMapper {
public:
    static EncodingMapper& getInstance() {
        static EncodingMapper instance;
        return instance;
    }

    int getEncodingFromIndex(int index) const;
    int getIndexFromEncoding(int encoding) const;
    int getEncodingFromString(const char* encodingAlias) const;

    // Qt6 extension: convert code page to QString codec name
    static QString codePageToCodecName(int codePage);

private:
    EncodingMapper() = default;
    ~EncodingMapper() = default;

    EncodingMapper(const EncodingMapper&) = delete;
    EncodingMapper& operator=(const EncodingMapper&) = delete;

    EncodingMapper(EncodingMapper&&) = delete;
    EncodingMapper& operator=(EncodingMapper&&) = delete;
};
