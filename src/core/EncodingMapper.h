// EncodingMapper.h — Qt6 port of Win32 codepage → encoding name mapping
// Ported from PowerEditor/src/EncodingMapper.h
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QtCore/QString>

struct EncodingUnit {
    int codePage = 0;
    const char* aliasList = nullptr;
};

class EncodingMapper {
public:
    static EncodingMapper& getInstance();

    /// Get codepage from encoding index (maintains Win32 N++ order)
    int getEncodingFromIndex(int index) const;

    /// Get encoding index from codepage
    int getIndexFromEncoding(int encoding) const;

    /// Get codepage from a string alias (e.g. "utf-8", "windows-1252")
    int getEncodingFromString(const char* encodingAlias) const;

    /// Get Qt encoding name from codepage
    static QString qtEncodingFromCodePage(int codePage);

    /// Get codepage from Qt encoding name
    static int codePageFromQtEncoding(const QString& encoding);

private:
    EncodingMapper() = default;
    ~EncodingMapper() = default;
    EncodingMapper(const EncodingMapper&) = delete;
    EncodingMapper& operator=(const EncodingMapper&) = delete;
    EncodingMapper(EncodingMapper&&) = delete;
    EncodingMapper& operator=(EncodingMapper&&) = delete;
};
