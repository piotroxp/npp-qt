// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <cassert>

// Qt6-compatible CRC16 implementation — pure algorithm, no Win32 dependency.

class CRC16_ISO_3309
{
public:
    explicit CRC16_ISO_3309(uint16_t polynom = 0x1021, uint16_t initVal = 0xFFFF)
        : _polynom(polynom), _initVal(initVal) {}

    void set(uint16_t polynom, uint16_t initVal) {
        _polynom = polynom;
        _initVal = initVal;
    }

    uint16_t calculate(const uint8_t* data, uint16_t count)
    {
        uint16_t fcs = _initVal;
        for (uint16_t i = 0; i < count; ++i)
        {
            uint16_t d = static_cast<uint16_t>(data[i] << 8);
            for (int k = 0; k < 8; ++k)
            {
                if ((fcs ^ d) & 0x8000)
                    fcs = static_cast<uint16_t>((fcs << 1) ^ _polynom);
                else
                    fcs = static_cast<uint16_t>(fcs << 1);
                d <<= 1;
            }
        }
        return fcs;
    }

private:
    uint16_t _polynom = 0;
    uint16_t _initVal = 0;
};

inline constexpr bool bits8  = true;
inline constexpr bool bits16 = false;

class CRC16 : public CRC16_ISO_3309
{
public:
    CRC16() = default;

    uint16_t calculate(const uint8_t* data, uint16_t count)
    {
        assert(data != nullptr);
        assert(count != 0);

        // Reverse all bits of each byte
        uint8_t* pBuffer = new uint8_t[count];
        for (int i = 0; i < count; ++i)
            pBuffer[i] = reverseByte<uint8_t>(data[i]);

        // Calculate CRC (default: polynom = 0x1021, init val = 0xFFFF)
        uint16_t wordResult = CRC16_ISO_3309::calculate(pBuffer, count);

        // Reverse the WORD bits
        wordResult = reverseByte<uint16_t>(wordResult);

        // XOR FFFF
        wordResult ^= 0xFFFF;

        // Invert MSB/LSB (byte swap)
        wordResult = static_cast<uint16_t>((wordResult << 8) | (wordResult >> 8));

        delete[] pBuffer;
        return wordResult;
    }

private:
    template <class IntType>
    IntType reverseByte(IntType val2Reverses) const
    {
        IntType reversedValue = 0;
        long mask = 1;
        int nBits = static_cast<int>(sizeof(IntType) * 8);
        for (int i = 0; i < nBits; ++i)
        {
            if ((mask << i) & val2Reverses)
                reversedValue += (mask << (nBits - 1 - i));
        }
        return reversedValue;
    }
};
