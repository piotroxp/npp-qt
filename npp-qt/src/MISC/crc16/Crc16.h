// Semantic Lift: CRC16 checksum utility (no Win32 dependencies)
// Original: PowerEditor/src/MISC/crc16/Crc16.h
// Target: npp-qt/src/MISC/crc16/Crc16.h

#pragma once

#include <cstdint>
#include <cassert>

// =============================================================================
// CRC16_ISO_3309 — ISO 3309 CRC-16 implementation
// Platform-independent: no Win32, pure C++17
// =============================================================================

class CRC16_ISO_3309
{
public:
    CRC16_ISO_3309(uint16_t polynom = 0x1021, uint16_t initVal = 0xFFFF)
        : _polynom(polynom), _initVal(initVal) {}
    ~CRC16_ISO_3309() = default;

    void set(uint16_t polynom, uint16_t initVal) {
        _polynom = polynom;
        _initVal = initVal;
    }

    uint16_t calculate(const uint8_t *data, uint16_t count)
    {
        uint16_t fcs = _initVal;
        uint16_t d, i, k;
        for (i = 0; i < count; i++)
        {
            d = *data++ << 8;
            for (k = 0; k < 8; k++)
            {
                if ((fcs ^ d) & 0x8000)
                    fcs = (fcs << 1) ^ _polynom;
                else
                    fcs = fcs << 1;
                d = d << 1;
            }
        }
        return fcs;
    }

private:
    uint16_t _polynom = 0;
    uint16_t _initVal = 0;
};
