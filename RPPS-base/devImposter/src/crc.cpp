// Description: Функции вычисления контрольных сумм
#include "crc.h"

QMap<QString, uint64_t*> CRC::tablesMap = QMap<QString, uint64_t*>();

void CRC::clear()
{
    foreach (QString key, tablesMap.keys())
    {
        uint64_t * value = tablesMap.value(key);
        tablesMap.remove(key);
        free(value);
    }
}

uint32_t crc32(QByteArray &data, bool isLittleEndian)
{
    return CRC::get( data, uint32_t(0x04c11db7), !isLittleEndian );
}

uint8_t crc8(QByteArray &data)
{
    return CRC::get( data, uint8_t(0x2f) );
}
uint8_t crc16ibm(QByteArray &data)//CRC-16 (IBM) X16+X15+X2+1 см. ИРП
{
    return CRC::get( data, uint8_t(0x8005) );
}
uint32_t netbios_len(QByteArray &data)
{
    union
    {
        uint32_t header;
        uint8_t buffer[4];
    } nb_hdr;
    uint32_t size = (data.length() - 4) & 0x01ffff;
    nb_hdr.buffer[0] = 0;
    nb_hdr.buffer[1] = (size >> 16) & 0xFF;
    nb_hdr.buffer[2] = (size >>  8) & 0xFF;
    nb_hdr.buffer[3] = size & 0xFF;
    return nb_hdr.header;//TODO: 16 or 24?
}
