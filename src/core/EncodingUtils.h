// core/EncodingUtils.h — pure helpers for mapping between EncodingType
// (the in-memory enum) and Qt6's QStringConverter (the runtime codec API).
//
// Wave 11 task #2 — charset conversion. Both Application::onConvertToCharset
// and tests/test_charset_conversion.cpp drive through these helpers.

#pragma once

#include "../common/Types.h"
#include <QString>
#include <QStringConverter>

namespace EncodingUtils {

// Returns the QStringConverter::Encoding for an EncodingType, or
// QStringConverter::Encoding::Utf8 as a safe default if the encoding
// is system-default (ANSI/OEM) or unknown.
QStringConverter::Encoding encodingForType(EncodingType enc);

// Returns the EncodingType for a codec name (case-insensitive). Returns
// EncodingType::NONE if no match — callers should treat that as "no
// canonical mapping, but QStringConverter may still resolve the name".
EncodingType encodingFromCodecName(const QString& name);

// Encodes `text` as bytes under the encoding named by `codecName` (e.g.
// "UTF-8", "ISO-8859-1", "Windows-1252"). Returns empty QByteArray on
// failure (unknown codec). The encoding name is resolved via
// QStringConverter::encodingForName, which covers a wider range than
// QStringConverter::Encoding (including locale-dependent code pages).
QByteArray encodeToBytes(const QString& text, const QString& codecName);

// Decodes `bytes` as text under the encoding named by `codecName`.
// Returns empty QString on failure.
QString decodeFromBytes(const QByteArray& bytes, const QString& codecName);

} // namespace EncodingUtils