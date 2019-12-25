// Description: Вычисление контрольных сумм
#pragma once

#define DEBUGGIN_CRC //TODO: вынести из кода
#ifdef DEBUGGIN_CRC
//#define IGNORE_CRC_ERROR
#include <QDebug>
#endif
extern uchar    POLY8(QByteArray &data, int offset);
extern uint     POLY32(QByteArray &data, int offset);
extern ushort   MOD2_WORD(QByteArray& data, int offset/*, const int firstPosition = 2*/);
extern uchar    CRC_256(QByteArray& data, int offset);
extern uchar    CRC_DUMMY(QByteArray& data, int offset);
extern uchar    MOD256(QByteArray& data, int offset);
extern ushort CRC16(QByteArray& data, int offset);

typedef long FARPTR;

template< unsigned int MOD, typename RetT = uchar>
static RetT CalcCRC(QByteArray & data, int offset)
{
#ifdef DEBUGGIN_CRC
    const int mod = MOD;
    Q_UNUSED(mod)
#endif
    uint sum = 0;
    for ( int i = 0; i < offset ; i++ )
        sum += (uchar)data[i];
    return sum % MOD;
}
template<typename T>
QByteArray ToHex(T a)
{
    return QByteArray( QString( "%1").arg( a ).toLocal8Bit() ).toHex();
}
template<typename RetT = uchar, int OFFSET = -1 >
class CRC_OFFSET_CALC
{
public:
    typedef RetT(* CalcProc)(QByteArray & data, int offset);
    static void CalcAndSet(QByteArray & data, CalcProc Calc)
    {
        FARPTR addr = (FARPTR)GetCrcAddress(data);
       *((RetT*)addr) = Calc (data, GetOffset(data));
    }
    static int GetCrcSize()
    { return sizeof(RetT); }
    static int GetOffset(QByteArray & data)
    {
        int offset = OFFSET == -1?
            data.size() - GetCrcSize():
            OFFSET;
        return offset;
    }
    static char* GetCrcAddress(QByteArray & data)
    {
        return data.data() + GetOffset(data);
    }
    static bool IsCrcOK(QByteArray & data, CalcProc Calc)
    {
        FARPTR addr = ( (FARPTR)GetCrcAddress(data) );
        RetT crcData = *((RetT*)addr),
        crcCalc = Calc (data, GetOffset(data));
#ifdef DEBUGGIN_CRC
        if (crcData != crcCalc)
        {
            DebugCrc(Calc, addr, data, crcCalc, crcData);
    #ifdef IGNORE_CRC_ERROR
            qDebug().noquote() << "Ignoring CRC error for now";
            return true;
    #endif
        }
#endif
        return ( crcData == crcCalc);//Calc (data, GetOffset(data));
    }
    static
    void DebugCrc(CalcProc Calc, FARPTR addr, QByteArray& data, RetT crcCalc, RetT crcData)
    {
      qDebug().noquote() << "CRC Error! Data: " << QString::number( crcData, 16 )
        << " Calc: " << QString::number( crcCalc, 16 )
        << " offset: " << GetOffset( data )
        << " size: " << GetCrcSize();
      addr = ( (FARPTR)GetCrcAddress(data) );
      crcData = *((RetT*)addr);
      crcCalc = Calc (data, GetOffset(data));
    }
};

//TODO: calc and set crc assuming it is at the end, mb we want to configure that as well?
template< unsigned int MOD, typename RetT = uchar >
class CRCMOD
{
public:
    static RetT Calc(QByteArray & data, int crcSize)
    {
        if( data.isEmpty() )
            return 0;

        if(crcSize > data.size())
            return 0;

        uint sum = 0;
        for ( int i = 0; i < data.size() - crcSize ; i++ )
            sum += (uchar)data[i];
        return sum % MOD;
    }
    static void CalcAndSet(QByteArray & data)
    {
        if(data.isEmpty())
            return;
        FARPTR addr = (FARPTR)GetCrcAddress(data);
        if( addr != 0 )
            *((RetT*)addr) = Calc (data, GetCrcSize());
    }
    static int GetCrcSize()
    { return sizeof(RetT); }
    static int GetOffset(QByteArray & data)
    {
        return (/*data.data() + */data.size() - GetCrcSize());
    }
    static char* GetCrcAddress(QByteArray & data)
    //FIXME: will work only if CRC field at the end of the data
    {
        int size = CRCMOD::GetOffset( data );
        return data.isEmpty() ?
                    nullptr :
                    ( data.size() > size ) ?
                        reinterpret_cast<char*>(data.data()+size) :
                        nullptr;
    }

    static bool IsCrcOK(QByteArray & data)
    {
        if( data.isEmpty())
            return false;
        FARPTR addr = (FARPTR)GetCrcAddress(data);
        RetT crcData = *((RetT*)addr ),
             crcCalc = Calc (data, GetCrcSize());

#ifdef DEBUGGIN_CRC
        if (crcData != crcCalc)
        {
            DebugCrc(addr, crcData, crcCalc, data);
    #ifdef IGNORE_CRC_ERROR
            qDebug().noquote() << "Ignoring CRC error for now";
            return true;
    #endif
        }

#endif
        return  crcData == crcCalc;// Calc (data, GetCrcSize());
    }
    static
    void DebugCrc(FARPTR addr, RetT crcData, RetT crcCalc, QByteArray& data)
    {
       qDebug().noquote() << "CRC Error! Data: " << ToHex<RetT>( crcData )
                << " Calc: " << ToHex<RetT>( crcCalc )
                << " offset: "<< GetOffset( data )
                << " size: " << GetCrcSize();
      addr = ( (FARPTR)GetCrcAddress(data) );
      crcData = *((RetT*)addr);
      crcCalc = Calc (data, GetCrcSize());
    }
};

class CRC_EMPTY
{
public:
    static void CalcAndSet(QByteArray & )
    {
    }
    static bool IsCrcOK(QByteArray &)
    {
        return true;
    }
};

template<typename RetT = uchar, int OFFSET = -1,
         RetT (*CalcFunc)(QByteArray &, int) = ::POLY8 >
class CRC_OFFSET_CALC1
{
public:
    typedef RetT(* CalcProc)(QByteArray & data, int offset);
    static void CalcAndSet(QByteArray & data)
    {
        FARPTR addr = (FARPTR)GetCrcAddress(data);
       *((RetT*)addr) = CalcFunc (data, GetOffset(data));
    }
    static int GetCrcSize()
    { return sizeof(RetT); }
    static int GetOffset(QByteArray & data)
    {
        int offset = OFFSET == -1?
            data.size() - GetCrcSize():
            OFFSET;
        return offset;
    }
    static char* GetCrcAddress(QByteArray & data)
    {
        return data.data() + GetOffset(data);
    }
    static bool IsCrcOK(QByteArray & data)
    {
        FARPTR addr = ( (FARPTR)GetCrcAddress(data) );
        RetT crcData = *((RetT*)addr),
        crcCalc = CalcFunc (data, GetOffset(data));
#ifdef DEBUGGIN_CRC
        if (crcData != crcCalc)
        {
            DebugCrc(CalcFunc, addr, data, crcCalc, crcData);
    #ifdef IGNORE_CRC_ERROR
            qDebug().noquote() << "Ignoring CRC error for now";
            return true;
    #endif
        }
#endif
        return ( crcData == crcCalc);//Calc (data, GetOffset(data));
    }
    static
    void DebugCrc(CalcProc Calc, FARPTR addr, QByteArray& data, RetT crcCalc, RetT crcData)
    {
      qDebug().noquote() << "CRC Error! Data: " << QString::number( crcData, 16 )
        << " Calc: " << QString::number( crcCalc, 16 )
        << " offset: " << GetOffset( data )
        << " size: " << GetCrcSize();
      addr = ( (FARPTR)GetCrcAddress(data) );
      crcData = *((RetT*)addr);
      crcCalc = Calc (data, GetOffset(data));
    }
};

template
<
    typename CrcType,
    int Offset,
    CrcType (*CrcFunc)(QByteArray &, int),
    class CrcTypes = CRC_EMPTY
>
class CRCCMDVAR1
{
    public:
    static void CalcAndSet(QByteArray & data)
    {
        CRC_OFFSET_CALC1<CrcType,Offset,CrcFunc>::CalcAndSet(data);
        CrcTypes::CalcAndSet(data);
    }
    static bool IsCrcOK(QByteArray & data)
    {
        return CRC_OFFSET_CALC1<CrcType,Offset,CrcFunc>::IsCrcOK(data) &&
        CrcTypes::IsCrcOK(data);
    }
};
template
<
    typename CrcType1,
    typename CrcType2
>
class CRCCMDVAR2
{
    public:
    static void CalcAndSet(QByteArray & data)
    {
        CrcType1::CalcAndSet(data);
        CrcType2::CalcAndSet(data);
    }
    static bool IsCrcOK(QByteArray & data)
    {
        return CrcType1::IsCrcOK(data) &&  CrcType2::IsCrcOK(data);
    }
};

template< typename... CrcTypes >
class CRCCMDVAR
{
public:
    static void CalcAndSet(QByteArray & data)
    {
        (void)std::initializer_list<int>
        {
            (CrcTypes::CalcAndSet(data), 0)...
        };
    }
    static bool IsCrcOK(QByteArray & data)
    {
        bool list[] = { CrcTypes::IsCrcOK(data)...};
    #if __GNUC__ == 4 && __GNUC_MINOR__ > 7
        return (std::all_of(std::begin(list), std::end(list),
                        [](bool v){return v;}));
    #else
        return (std::all_of(list, list + sizeof...(CrcTypes),
                        [](bool v){return v;}));
    #endif
    }
};

