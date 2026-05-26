// MISC/crc16/Crc16.h - Qt6 port of CRC16 implementation
#pragma once

#include <cstdint>

class CRC16_ISO_3309
{
public:
    CRC16_ISO_3309(uint16_t polynom = 0x1021, uint16_t initVal = 0xFFFF)
        : _polynom(polynom), _initVal(initVal) {}

    void set(uint16_t polynom, uint16_t initVal) {
        _polynom = polynom;
        _initVal = initVal;
    }

    uint16_t calculate(const uint8_t* data, uint16_t count)
    {
        uint16_t fcs = _initVal;
        uint16_t d;
        uint16_t k;
        
        for (uint16_t i = 0; i < count; i++)
        {
            d = data[i] << 8;
            for (k = 0; k < 8; k++)
            {
                if ((fcs ^ d) & 0x8000)
                    fcs = (fcs << 1) ^ _polynom;
                else
                    fcs = (fcs << 1);
                d <<= 1;
            }
        }
        return fcs;
    }

private:
    uint16_t _polynom;
    uint16_t _initVal;
};

const bool bits8 = true;
const bool bits16 = false;

class CRC16 : public CRC16_ISO_3309
{
public:
    CRC16() : CRC16_ISO_3309(0x1021, 0xFFFF) {}

    uint16_t calculate(const uint8_t* data, uint16_t count)
    {
        if (!data || count == 0)
            return 0;

        uint8_t* pBuffer = new uint8_t[count];

        // Reverse all bits of the byte
        for (int i = 0; i < count; i++)
            pBuffer[i] = reverseByte<uint8_t>(data[i]);

        // Calculate CRC
        uint16_t wordResult = CRC16_ISO_3309::calculate(pBuffer, count);

        // Reverse the WORD bits
        wordResult = reverseByte<uint16_t>(wordResult);

        // XOR FFFF
        wordResult ^= 0xFFFF;

        // Invert MSB/LSB
        wordResult = (wordResult << 8) | (wordResult >> 8);

        delete[] pBuffer;

        return wordResult;
    }

private:
    template <class IntType>
    IntType reverseByte(IntType val2Reverses)
    {
        IntType reversedValue = 0;
        long mask = 1;
        int nBits = sizeof(IntType) * 8;
        for (int i = 0; i < nBits; i++)
            if ((mask << i) & val2Reverses)
                reversedValue += (mask << (nBits - 1 - i));
        return reversedValue;
    }
};