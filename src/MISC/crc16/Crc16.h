// Crc16.h — Pure C++ CRC-16 implementation (no platform deps)
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace NppCRC16 {

/// Compute CRC-16 (CCITT) of raw bytes
quint16 compute(const QByteArray& data);

/// Compute CRC-16 of a string (UTF-8)
quint16 computeString(const QString& str);

/// Format as hex string (4 chars)
QString toHex(quint16 crc);

/// CRC-16 table (CCITT polynomial 0x1021)
extern const quint16 crc16_table[256];

} // namespace NppCRC16
