// Description: Вычисление контрольных сумм
#include <QByteArray>
#include "crc.h"
#include "CalcCRC.h"

uchar MOD256(QByteArray &data, int offset)
{
    return CalcCRC<256, uchar>(data, offset);
}
ushort MOD2(QByteArray &data, int offset)
{
    return CalcCRC<1<<sizeof(ushort)*8, ushort>(data, offset);
}
uchar POLY8(QByteArray &data, int offset)
{
    if (offset == -1)
        return crc8(data);
    QByteArray dataChopped = data.left(offset);
    return crc8( dataChopped );
}
uint POLY32(QByteArray &data, int offset)
{
    if (offset == -1)
        return crc32(data);
    QByteArray dataChopped = data.left(offset);
    return crc32( dataChopped );
}
ushort MOD2_WORD(QByteArray& data, int offset/*, const int firstPosition*/)
{
    int firstPosition = 2; //TODO:: привести все методы к общему шаблону
    if( (firstPosition < 0) || (offset < 0) )
        return 0;

    if( offset > data.size() )
        offset = data.size() - 1;//TODO: так-ли ?

    short       check_2_16  = 0;
    QByteArray  array       = firstPosition == 0 ?
                                  data.left( offset ) :
                                  data.mid(firstPosition, offset - firstPosition );

    if( array.size()  & 1 )
        array.append(char(0x00));

    auto increment = sizeof( ushort );
    auto limit = array.size();
    for( auto i = 0; i < limit; i += increment )
        check_2_16 ^= *reinterpret_cast<ushort*>(array.data() + i);

    return check_2_16;
}
uchar CRC_256(QByteArray& data, int offset)
{
    uint        check_256  = 0;
    QByteArray  dat         = data.left(offset);

    for( short i = 0; i < dat.size(); i++ )
        check_256 += (uchar)dat[i];

    return (uchar)(check_256 % 256);
}
uchar CRC_DUMMY(QByteArray& data, int offset)
{
    return data[offset];
}
ushort CRC16(QByteArray& data, int offset)
{ return MOD2_WORD(data, offset); }
