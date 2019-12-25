// Description: Структура описания поля команды
#include "ImposterInterfaces.h"
#include "Transport.h"
#include "FieldDescriptor.h"



FieldValueCheckStatus PacketFieldDescriptor::Match(const uchar *buffer, uint size, uint relativeOffset) const
{
    if (size <= _offset + relativeOffset)
        return FieldValueCheckStatus::NeedMoreData;
    else if (buffer[_offset + relativeOffset] == _value)//TODO: take into consideration the actual size of s field
        return FieldValueCheckStatus::Match;
    else
        return FieldValueCheckStatus::MatchFail;
}

FieldValueCheckStatus VarSizeFieldDescriptor::Match(const uchar *, uint size, uint relativeOffset) const
{
    if (size < _offset + relativeOffset)//FIX: <=
        return FieldValueCheckStatus::NeedMoreData;
    return FieldValueCheckStatus::RelativeOffsetMode;
}

bool DynCRCFieldDescriptor::IsCrcValid(const uchar *buffer, uint size) const
{
    QByteArray bytearray((char*)(buffer), (int)size);
    Q_UNUSED(bytearray);
    return true;//CRC_OFFSET_CALC<RetT, OFFSET>::IsCrcOK(bytearray, calcProc);
}
