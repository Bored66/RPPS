// Description: Функции вычисления контрольных сумм
#pragma once

#include <stdint.h>
#include <QObject>
#include <QMap>
#include <QString>

template <typename crcType>
static crcType inverseBytes(crcType value)
{
    crcType result = 0;
    for(size_t i = 0; i < sizeof(result); i++)
    {
        uint8_t byte = (value & (0xff << (i * 8))) >> (i * 8);
        result |= byte << ((sizeof(result) - (i + 1)) * 8);
    }
    return result;
}

class CRC : public QObject
{
private:
    static QMap<QString, uint64_t *> tablesMap;
    template <typename crcType>
    static uint64_t* getTable(crcType polynomial)
    {
        uint64_t* result = tablesMap.value(polynomialToString(polynomial));
        if(result == NULL)
        {
            result = generateTable(polynomial);
        }
        return result;
    }
    template <typename crcType>
    static QString polynomialToString(crcType polynomial)
    {
        uint8_t byte;
        QString result;
        int size = sizeof(polynomial);
        for(int i = 0; i < size; i++)
        {
            byte = 0xFF & (polynomial >> (size * 8 - 8));
            result.append(byte);
            polynomial <<= 8;
        }
        return result;
    }
    template <typename crcType>
    static uint64_t* generateTable(crcType polynomial)
    {
        uint64_t *table = (uint64_t *)malloc(sizeof(uint64_t) * 256);
        uint8_t size = sizeof(polynomial);
        crcType remainder;
        uint8_t b = 0;
        do
        {
            remainder = b << (size * 8 - 8);
            for (uint8_t bit = 8; bit > 0; --bit)
            {
                if (remainder & (0x80 << (size * 8 - 8)))
                {
                    remainder = (remainder << 1) ^ polynomial;
                }
                else
                    remainder = (remainder << 1);
            }
            table[(size_t)b] = remainder;
        } while(0 != ++b);
        tablesMap.insert(polynomialToString(polynomial), table);
        return table;
    }
    CRC()
    {
    }
public:
    template <typename crcType>
    static crcType get(QByteArray &data, crcType polynomial, bool isLittleEndian = false)
    {
        uint64_t* table = getTable(polynomial);
        crcType result = 0;
        uint8_t size = sizeof(polynomial);

        for(size_t i = 0; i < sizeof(polynomial); i++)
            data.append(char(0));
        for (int i = 0; i < data.size(); i++)
        {
            uint8_t index = (result >> ( size * 8 - 8 )) & 0xFF;
            result = (result << 8) | uint8_t(data[i]);
            result ^= table[index];
        }
        if(isLittleEndian)
            result = inverseBytes(result);
        return result;
    }
    static void clear();
};

uint32_t crc32(QByteArray &data, bool isLittleEndian = true);
uint8_t crc8(QByteArray &data);
uint8_t crc16ibm(QByteArray &data);
uint32_t netbios_len(QByteArray &data);
