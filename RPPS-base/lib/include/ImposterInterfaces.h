// Description: Модуль разбора полей команды
#ifndef IMPOSTERINTERFACES_H
#define IMPOSTERINTERFACES_H

#include <QObject>
#include <QDebug>
#include "CalcCRC.h"

struct MsgFieldMapInfo
{
    void *storage;
    unsigned int typeSizeOf;
    unsigned int offset;
};
inline //TODO: is it really such a good idea apart from debugging?
void memCopy(void *dst, void *src, uint typeSizeOf)
{
    if (typeSizeOf == 1)
        *((char*)dst) = *((char*)src);
    else
        memcpy( dst, src, typeSizeOf );
}
inline
void InitStructFromBuffer
(
    char * buffer,
    const MsgFieldMapInfo *pMapInfo,
    int offsetInBuffer,
    int from,
    int to
)
{
    for (int i = from; i <= to; i++ )
    {
        MsgFieldMapInfo fieldInfo = pMapInfo[i];
        memCopy( fieldInfo.storage,
                 buffer + offsetInBuffer + fieldInfo.offset,
                 fieldInfo.typeSizeOf );
    }
}
inline
void AppendBytesToBuffer(QByteArray & qbyteArray, MsgFieldMapInfo mapInfo)
{
    qbyteArray.append( (char*)mapInfo.storage, mapInfo.typeSizeOf );
}

#define arraySize( ARRAY ) (sizeof( ARRAY )/sizeof( ARRAY[ 0 ] ))

template< typename RESP, typename varPart, typename... varParts>
QByteArray varSizeMessageToByteArray(RESP & ans)
{
    QByteArray qbyteArray;
    uint offsetOfFieldInfo = 0;
    varSizeMessageToByteArray<RESP, varPart, varParts...>(qbyteArray, ans, offsetOfFieldInfo);
    ans.MapToQByteArray( qbyteArray, offsetOfFieldInfo, ans.GetMapInfoCount());
    return qbyteArray;
}
template< typename RESP, typename varPart, typename... varParts>
QByteArray varSizeMessageToByteArray(QByteArray & qbyteArray, RESP & ans, uint & offsetOfFieldInfo)
{
    int prevOffset = offsetOfFieldInfo;
    varPart & var = ans;
    offsetOfFieldInfo = var.getVarSizedFieldOffset();
    var.updateNob();
    ans.MapToQByteArray( qbyteArray, prevOffset, offsetOfFieldInfo);
    qbyteArray += var.addItemsToByteArray();
    varSizeMessageToByteArray<RESP, varParts...>(qbyteArray, ans, offsetOfFieldInfo);
    return qbyteArray;
}
template< typename RESP, typename... vars>
typename std::enable_if<sizeof...(vars) == 0>::type
varSizeMessageToByteArray(QByteArray & , RESP &, uint & )
{
    return;
}
template< typename RESP>
QByteArray varSizeMessageToByteArray(RESP & ans)
{
    uint offsetOfFieldInfo = ans.getVarSizedFieldOffset();
    ans.updateNob();
    QByteArray qbyteArray;
    ans.MapToQByteArray( qbyteArray, 0, offsetOfFieldInfo );
    ans.addItemsToByteArray(qbyteArray);
    ans.MapToQByteArray( qbyteArray, offsetOfFieldInfo, ans.GetMapInfoCount());
    return qbyteArray;
}
template< typename RESP, typename var, typename... vars>
char * ReadFromBuff(char *pbuff, RESP & ans, uint & offsetOfFieldInfo, uint & fullSize)
{
    int prevOffset = offsetOfFieldInfo;
    offsetOfFieldInfo = ((var)ans).getVarSizedFieldOffset();
    uint bufferRead = fullSize;// ans.GetFieldInfoList(prevOffset).offset;
    int skipToData = ans.calcByteOffset(prevOffset, offsetOfFieldInfo);
    //ans.InitStructFromBuffer( pbuff, offsetOfFieldInfo, prevOffset, bufferRead );
    ans.InitStructFromBuffer2( pbuff, bufferRead, offsetOfFieldInfo + 1, prevOffset );
    //pbuff += skipToData;
    bool equal = skipToData == bufferRead;
    //pbuff =
    ((var&)ans).getItemsFromBuff(pbuff + fullSize + skipToData);
    if (auto count = ((var&)ans).getItemCount())
    {
        fullSize += skipToData +
                ((var&)ans).getVarMsgSize();
    }
    else
    {
       auto sz = ((var&)ans).getVarMsgSize();
       fullSize += skipToData +
               ((var&)ans).getVarMsgSize();
    }

    pbuff = ReadFromBuff<RESP, vars...>(pbuff, ans, offsetOfFieldInfo, fullSize);
    return pbuff;
}
template< typename RESP, typename... vars>
typename std::enable_if<sizeof...(vars) == 0, char>::type
* ReadFromBuff(char *pbuff, RESP & ans, uint & offsetOfFieldInfo, uint & fullSize)
{
    for (uint i = offsetOfFieldInfo; i < ans.GetMapInfoCount(); i++ )
    {
        MsgFieldMapInfo fieldInfo = ans.GetFieldInfoList(i);
        fullSize += fieldInfo.typeSizeOf;
    }
    return pbuff;
}
template
<
    typename STORAGE,
    int VarSizedFieldOffset,
    typename COLLECTION,
    typename NOB = uchar,
    int headerSize = 0,
    int multiplier = 1
>
class VarSizedStorage
{
    COLLECTION **collPtr;
    NOB *nob;
    const uint descListSize;
    const MsgFieldMapInfo *fieldInfoList;
public:
    STORAGE data_storage;
    template< uint ARRSIZE >
    VarSizedStorage(COLLECTION **collPtr, NOB *nob,
                    const MsgFieldMapInfo (&fieldInfoListDataStorage)[ARRSIZE]):
        collPtr{collPtr}, nob{nob},
        descListSize{ARRSIZE},
        fieldInfoList{fieldInfoListDataStorage}
    {}
    void setFieldInfo(const MsgFieldMapInfo *fieldInfoListDataStorage)
    { fieldInfoList = fieldInfoListDataStorage; }
    uint actualItemSize()
    {
        uint sz = 0;
        for(uint i = 0; i < descListSize; ++i)
            sz += fieldInfoList[i].typeSizeOf;
        return sz;
    }
    void updateStorageItem(int index)
    {
        if (*collPtr)
            data_storage = (*collPtr)->at(index);
    }
    int getItemCount()
    {
        return (*collPtr) ? (*collPtr)->count() : 0;
    }
    void addDataItem(const STORAGE & item)
    {
        if (*collPtr == nullptr)
            *collPtr = new COLLECTION();
        if (*collPtr)
            (*collPtr)->append( item );
    }
    void addDataItem()
    { addDataItem(data_storage); }
    int getVarSizedFieldOffset()
    {
        return VarSizedFieldOffset;
    }
    void updateNob()
    {
        if (nob)
            *nob = (getItemCount() * actualItemSize())/qMax(1, multiplier)
                + headerSize;
    }
    uint getVarMsgSize()
    {
        auto result = nob ? *nob : 0;

        if (result > 0)
            result = getMultiplier() * (result - getHeaderSize());

        return result;
    }
    uint getHeaderSize()
    { return headerSize; }

    uint getMultiplier()
    {
        return multiplier;
    }
    uint getItemAndAdd(char *pbuff)
    {
        InitStructFromBuffer(pbuff, fieldInfoList, 0, 0, descListSize - 1);
        addDataItem();
        return actualItemSize();
    }
    QByteArray addItemsToByteArray(QByteArray & qbyteArray)
    {
        uint repeatTimes = getItemCount();
        for (uint j = 0; j < repeatTimes; ++j)
        {
            updateStorageItem(j);
            for(uint i = 0; i < descListSize; ++i)
                AppendBytesToBuffer( qbyteArray, fieldInfoList[i] );
        }
        return qbyteArray;
    }
    QByteArray addItemsToByteArray()
    {
        QByteArray qbyteArray;
        return addItemsToByteArray(qbyteArray);
    }
    char *getItemsFromBuff(char *pbuff)//, int buffSize)
    {
        auto varSize = getVarMsgSize();
        if (varSize > 0)// && (uint)buffSize >= nob)
        {
            const uint dataItemSize = actualItemSize();
            auto itemCount = varSize/dataItemSize;
            (*collPtr)->reserve(itemCount);
            for (uint i = 0; i < itemCount; i++, pbuff += dataItemSize)
                getItemAndAdd(pbuff);
        }
        return pbuff;
    }
};

template
<
    typename Struct,
    typename Crc = CRC_EMPTY
>
class IMsg
{
public:
    IMsg()
    {
        initFields();
    }
    virtual ~IMsg()
    {
        //qDebug().noquote() << metaObject()->className();
    }
    void initFields()
    {
        QByteArray empty(sizeof(_msgStruct.buffer), 0);
        InitStructFromBuffer(empty.data());
    }
    void InitStructFromBuffer
    (
        char * buffer,
        int to = arraySize(_msgStruct.fieldInfoList),
        int from = 0,
        int bufferRead = 0
    )
    {
        for (int i = from; i < to; i++ )
        {
            MsgFieldMapInfo fieldInfo = GetFieldInfoList(i);
            memCopy( fieldInfo.storage, &buffer[fieldInfo.offset - bufferRead], fieldInfo.typeSizeOf );
        }
    }
    void InitStructFromBuffer2
    (
        char * buffer,
        uint & bufferRead,
        int to = arraySize(_msgStruct.fieldInfoList),
        int from = 0
    )
    {
        for (int i = from; i < to; i++ )
        {
            MsgFieldMapInfo fieldInfo = GetFieldInfoList(i);
            memCopy( fieldInfo.storage, &buffer[bufferRead], fieldInfo.typeSizeOf );
            bufferRead += fieldInfo.typeSizeOf;
        }
    }
    IMsg & operator=( char * buffer )
    {
        InitStructFromBuffer(buffer);
        return *this;
    }
    QByteArray ToByteArray()
    {
        fromStructBufferToByteArray( _bufferByteArray );
        return _bufferByteArray;
    }
    bool IsValidMessage()
    {
        if (_bufferByteArray.isEmpty())
            fromStructBufferToByteArray( _bufferByteArray );
        return Crc::IsCrcOK( _bufferByteArray );
    }
    int calcByteOffset(int from, int to)
    {
        int totalBytes = 0;
        for (int i = from; i < to; i++ )
             totalBytes += GetFieldInfoList(i).typeSizeOf;
        return totalBytes;
    }
    void FromByteArray( QByteArray & data )
    {
        memset(&_msgStruct.buffer, '\0', sizeof(_msgStruct.buffer));
        InitStructFromBuffer( data.data() );
    }
    void MapToQByteArray(QByteArray & qbyteArray, uint from, uint upTo) const
    {
        for(uint i = from; i < upTo; i++)
           AppendBytesToBuffer(qbyteArray, GetMapInfo(i));
    }
    virtual void SetDefaultValues() = 0;

    Struct                                  _msgStruct;
    QByteArray                              _bufferByteArray;
    void CalcCrcAndSet(QByteArray & data)
    {
        Crc::CalcAndSet( data );
    }
    virtual
    QByteArray PrepareToSend()
    {
        fromStructBufferToByteArray( _bufferByteArray );
        Crc::CalcAndSet( _bufferByteArray );
        FromByteArray( _bufferByteArray );
        return ToByteArray();
    }
    void fromStructBufferToByteArray( QByteArray & data ) const
    {
        data.clear();
        MapToQByteArray( data, 0, GetMapInfoCount() );
    }
    uint expectedSize() const
    { return sizeof _msgStruct.buffer; }
    virtual
    uint GetMapInfoCount() const
    { return GetFieldInfoListSize(); }
    constexpr int GetFieldInfoListSize() const
    { return arraySize(_msgStruct.fieldInfoList);}
    MsgFieldMapInfo GetMapInfo(uint fieldCount) const
    {
        if (fieldCount < GetMapInfoCount())
            return _msgStruct.fieldInfoList[fieldCount];
        return {nullptr, 0, 0};
    }
    MsgFieldMapInfo GetFieldInfoList(int i) const
    { return GetMapInfo(i);}

    IMsg(const IMsg & other)
    {
        _msgStruct = other._msgStruct;
        adjustLayoutInfo(other);
    }
private:
    void adjustLayoutInfo(const IMsg& other)
    {
        for (uint i = 0; i < GetMapInfoCount(); i++)
            updateStorage(i, other.getStorageOffset(i));
    }
    long getStorageOffset(int i) const
    {
        void *const origAddress = (void *const)&_msgStruct;
        void * storage = GetMapInfo(i).storage;
        return (long)storage - (long)origAddress;
    }
    void updateStorage(int i, int offset)
    {
        void * origAddress = &_msgStruct;
        _msgStruct.fieldInfoList[i].storage = (void*) ((long)origAddress + offset);
    }
};
#endif // IMPOSTERINTERFACES_H
