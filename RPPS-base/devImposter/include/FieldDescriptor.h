// Description: Структура описания поля команды
#ifndef FIELDDESCRIPTOR
#define FIELDDESCRIPTOR

#include <functional>
//#define USE_FUNCTION_VIEW
#ifdef USE_FUNCTION_VIEW
#include "function_view.h"
#endif
#include "CalcCRC.h"

enum class FieldValueCheckStatus
{
    MatchFail,
    Skip,
    Match,
    RelativeOffsetMode,
    InvalidCRC,
    Valid,
    NeedMoreData
};

class PacketFieldDescriptor
{
protected:
    const QString _name;
    uint _offset;
    uint _value;
    uint _size;
    uint _auxData;
public:
    PacketFieldDescriptor
    (
    ){}
    PacketFieldDescriptor
    (
        const QString & name,
        uint offset,
        uint value,
        uint size,
        uint data = 0
    ):
    _name(name),
    _offset(offset),
    _value(value),
    _size(size),
    _auxData(data)
    {}
    virtual
    FieldValueCheckStatus Match(const uchar *buffer, uint size, uint relativeOffset) const;
    QString Name() const
    { return _name; }
    uint Size() const
    { return _size; }
    uint Offset() const
    { return _offset; }
    uint Value() const
    { return _value; }
    virtual
    uint GetOffsetToSizeStorage() const
    { return 0; }
    uint auxData() const
    {
        return _auxData;
    }
};
class VarSizeFieldDescriptor : public PacketFieldDescriptor
{
    uint _offsetToSizeStorage;
    uint multiplier;
public:
    VarSizeFieldDescriptor
    (
        const QString & name,
        uint offset,
        uint offsetToStorage,
        uint size = 1,
        uint auxData = 0,
        uint multiplier = 1
    ):
    PacketFieldDescriptor (name, offset, 0, size, auxData),
    _offsetToSizeStorage(offsetToStorage),
    multiplier(multiplier)
    {}
    FieldValueCheckStatus Match(const uchar *, uint size, uint relativeOffset) const;
    uint GetOffsetToSizeStorage() const
    { return _offsetToSizeStorage; }
    uint GetMultiplier() const {
        return multiplier;
    }
};


template<typename RetT = unsigned char, int OFFSET=-1>
struct CRCFieldDescriptor : public PacketFieldDescriptor
{
    typedef RetT(* CalcProc)(QByteArray & data, int offset);
    CalcProc calcProc;
public:
    CRCFieldDescriptor
    (
        const QString name,
        uint offset,
        uint value,
        uint size,
        CalcProc calcProc = CalcCRC<256, RetT>
    ):
    PacketFieldDescriptor (name, offset, value, size),
      calcProc(calcProc)
    {}
    virtual
    FieldValueCheckStatus Match(const uchar *buffer, uint size, uint relativeOffset) const
    {
        if (size < _offset + relativeOffset + _size)
            return FieldValueCheckStatus::NeedMoreData;
        else if (IsCrcValid(buffer, _offset + _size + relativeOffset))//zero-based index
            return FieldValueCheckStatus::Valid;
        return FieldValueCheckStatus::InvalidCRC;
    }
private:
    bool IsCrcValid(const uchar *buffer, uint size) const
    {
        QByteArray bytearray((char*)(buffer), (int)size);
        return CRC_OFFSET_CALC<RetT, OFFSET>::IsCrcOK(bytearray, calcProc);
    }
};
struct DynCRCFieldDescriptor : public PacketFieldDescriptor
{
public:
    DynCRCFieldDescriptor
    (
        const QString name,
        uint offset,
        uint value,
        uint size
    ):
    PacketFieldDescriptor (name, offset, value, size){}
    virtual
    FieldValueCheckStatus Match(const uchar *buffer, uint size, uint relativeOffset) const
    {
        if (size < _offset + relativeOffset + _size)
            return FieldValueCheckStatus::NeedMoreData;
        else if (IsCrcValid(buffer, _offset + _size + relativeOffset))//zero-based index
            return FieldValueCheckStatus::Valid;
        return FieldValueCheckStatus::InvalidCRC;
    }
private:
    bool IsCrcValid(const uchar *buffer, uint size) const;
};
using PacketDispatcherMethodCall =
#ifdef USE_FUNCTION_VIEW
   function_view<TransportRequestStatus (const uchar *, uint, uint &)>;
#else
   std::function<TransportRequestStatus (const uchar *buffer, uint buffSize, uint & bytesRead)>;
#endif
//the below is not working for DynamicPacketDispatcher
//TransportRequestStatus (*) (const uchar *buffer, uint buffSize, uint & bytesRead);
class PacketDispatcher;
struct PacketDescriptor
{
    const PacketFieldDescriptor ** rowDef;
    uint count;
    PacketDispatcherMethodCall callMethod;
    //QString methodName;
};
#endif // FIELDDESCRIPTOR

