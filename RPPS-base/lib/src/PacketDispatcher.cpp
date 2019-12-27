// Description: Разбор структуры пакета
#include <QDebug>
#include <QProcessEnvironment>
#include "PacketDispatcher.h"

const QString DebugPacketDispEnvVarName = "PACKETDISPATCHER_DEBUG";


PacketDispatcher::PacketDispatcher(ITransport *dataProvider, QObject *parent):
    QObject( parent ),
    _garbage_limit(1),
    _tableRowCount( 0 ),
    _result( TransportRequestStatus::REQUEST_SEND_SUCCESS ),
    _status( FieldValueCheckStatus::MatchFail ),
    _dispatcherTable(nullptr),
    _outputDebug(false)
{
    current_buffer_size = 2*0xffff;
    _buffer = new char[current_buffer_size];
    SetTransport( dataProvider );
}
void PacketDispatcher::DebugOutput(const QByteArray &data, int bytesRead)
{
    if (_outputDebug)
    {
        qDebug().noquote() << getLogPrefix()
                 ////<< "Current dispatch buffer: " <<
                    ////(_buffer.size()? insertDelim( _buffer.toHex() ) : "<empty>")
                 << " Received data [" << data.size() << "]: "
                    << insertDelim( data.toHex() );
        qDebug().noquote() << getLogPrefix()
                << "from" << metaObject()->className()
                << "bytesRead: " << bytesRead
                << " Status: " << ITransport::RequestResultToStr( _result );
    }
}

void PacketDispatcher::Dispatch(const QByteArray &data)
{
    uint read = 0;

    emit onDispatch( data );
    appendBuffer(data);
    ////_buffer.append( data );
    int index = 0;
    int i1 = 0;
    do
    {
        ////_result = Dispatch( ( uchar * ) _buffer.data(), uint(_buffer.size()), read );
        //_result = Dispatch( ( uchar * ) (_buffer.data()+offset_buffer), uint(_buffer.size()-offset_buffer), read );
        i1++;
        _result =
            Dispatch((uchar*)_buffer + offset_in_buffer, uint(buffer_upper_bound - offset_in_buffer), read);
        if (_outputDebug)
        {
        if (_result == TransportRequestStatus::REQUEST_SEND_SUCCESS)
            fprintf(stderr, "result=%d read=%d SUCCESS\n", _result, read);

        if (_result!=7)
            fprintf(stderr, "result=%d\n", _result);
        }
        if (i1 % 1000 == 0)
            fprintf(stderr, "into %d %d=%d %d result=%d\n", i1, offset_in_buffer, buffer_upper_bound, read, _result);
        if (_result == TransportRequestStatus::REQUEST_NEED_MORE_DATA)
            break;
        if (_result == TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR)
        {
            ////_buffer.remove(0, 1);
            offset_in_buffer++;
            index++;
        }
        else
        {
            ////_buffer.remove(0, int(read));
            offset_in_buffer += read;
            index += read;
        }

        if (read > 1)
           DebugOutput(data, int(read));

    }while (buffer_upper_bound > offset_in_buffer);
    ////while(_buffer.size() > 0);
    if (_outputDebug) fprintf(stderr, "size %d=%d %d %d result=%d\n", offset_in_buffer, buffer_upper_bound, read, i1, _result);
    memmove(_buffer, _buffer + offset_in_buffer, buffer_upper_bound - offset_in_buffer);
    //_buffer.remove(0, offset_buffer);
    buffer_upper_bound -= offset_in_buffer;
    offset_in_buffer = 0;
    if (_result == TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR)
    {
        ////_buffer = bufferUnknown;
        offset_in_buffer = 0;
    }
    else
    {
        if (_outputDebug) fprintf(stderr,"found i=%d\n",i1);
    }
    if (offset_in_buffer >= buffer_upper_bound)
    {
        if (_outputDebug) fprintf(stderr, "clear buffer\n");
        //_buffer.clear();
        buffer_upper_bound = 0;
        offset_in_buffer = 0;
    }
    emit dispatchResult( _result );
}
void PacketDispatcher::appendBuffer(const QByteArray &data)
{
    uint data_size = uint(data.size());
    if(current_buffer_size < data_size + buffer_upper_bound)
    {
        fprintf(stderr, "resizing buffer from %d to %d\n",
                current_buffer_size, current_buffer_size + data_size);
        current_buffer_size += data_size;
        char* buffer_new = new char[current_buffer_size];
        memcpy(buffer_new, _buffer, buffer_upper_bound);
        delete [] _buffer;
        _buffer = buffer_new;
    }
    memcpy(_buffer + buffer_upper_bound, data.data(), data_size);
    buffer_upper_bound += data_size;
}

void PacketDispatcher::SetTransport( ITransport * transport )
{
    if ((QProcessEnvironment::systemEnvironment().
            contains(DebugPacketDispEnvVarName) &&
            str2bool(QProcessEnvironment::systemEnvironment().
                     value(DebugPacketDispEnvVarName)))
            || (transport != nullptr &&
                transport->debugParam("DISPATCHER") ) )
        _outputDebug = true;
    _transport = transport;
}

TransportRequestStatus PacketDispatcher::Dispatch(const uchar *buffer, uint buffSize, uint & bytesRead)
{
    auto PacketDispatcherTable = GetDispatcherTable();
    FieldValueCheckStatus prevStatus = FieldValueCheckStatus::MatchFail;//for debug
    uint rowIndex = 0, columnIndex = 0, relativeOffset = 0;//for debug
    for (; rowIndex < _tableRowCount; rowIndex++)
    {
        auto row = PacketDispatcherTable[rowIndex];
        for (columnIndex = 0, relativeOffset = 0; columnIndex < row->count; columnIndex++)
        {
            auto columnDef = row->rowDef[columnIndex];
            bytesRead = columnDef->Offset() + columnDef->Size() + relativeOffset;
            _status = columnDef->Match(buffer, buffSize, relativeOffset);

            if (_status == FieldValueCheckStatus::MatchFail)
                break;
            else if (_status == FieldValueCheckStatus::NeedMoreData)
            {
                LogInfo(prevStatus,relativeOffset, rowIndex, columnIndex, bytesRead);
                return TransportRequestStatus::REQUEST_NEED_MORE_DATA;
            }
            else if (_status == FieldValueCheckStatus::InvalidCRC)
            {
                LogInfo(prevStatus,relativeOffset, rowIndex, columnIndex, bytesRead);
                return TransportRequestStatus::REQUEST_CRC_ERROR;
            }
            else if (_status == FieldValueCheckStatus::RelativeOffsetMode)
                _status = UpdateRelativeOffsetFromSizeStorage(columnDef, buffer,
                                                              buffSize, &relativeOffset);
            //TODO: make part of the above?
            if (_status == FieldValueCheckStatus::NeedMoreData)
            {
                LogInfo(prevStatus,relativeOffset, rowIndex, columnIndex, bytesRead);
                return TransportRequestStatus::REQUEST_NEED_MORE_DATA;
            }

            prevStatus = _status;
        }
        if (_status == FieldValueCheckStatus::Match ||
                _status == FieldValueCheckStatus::Valid)
        {
            //bytesRead += relativeOffset;//apparently it's been updated twice&
            //see UpdateRelativeOffsetFromSizeStorage() method
            emit dispatchPacket(row, buffer, buffSize, bytesRead);
            return row->callMethod(buffer, buffSize, bytesRead);
        }
    }
    LogInfoBasedOnStatus(relativeOffset, prevStatus, columnIndex, rowIndex, bytesRead);
    return TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR;
}
FieldValueCheckStatus PacketDispatcher::UpdateRelativeOffsetFromSizeStorage
(
        const PacketFieldDescriptor *fielddesc,
        const uchar* buffer,
        uint buffSize,
        uint *const relativeOffset
)
{
    uint offsetToRefSize = fielddesc->GetOffsetToSizeStorage();
    if ( offsetToRefSize == 0)
        return FieldValueCheckStatus::MatchFail;
    uint headerSize = fielddesc->Size();
    uint sizeRefTypeSize = fielddesc->auxData();

    uint multiplier = 1;
    if (auto varSizeField = dynamic_cast<const VarSizeFieldDescriptor*>(fielddesc))
        multiplier = varSizeField->GetMultiplier();

    if (*relativeOffset > 0)
        offsetToRefSize += (*relativeOffset);// - (size == 0? 1 : size));
    if (buffSize > offsetToRefSize)
    {
        uint value = buffer[offsetToRefSize];
        if (sizeRefTypeSize > 0)
            memcpy(&value, &buffer[offsetToRefSize], sizeRefTypeSize);

        uint fieldSize = (value - headerSize) * multiplier;

        if (buffSize < fielddesc->Offset() + *relativeOffset + fieldSize) {
            return FieldValueCheckStatus::NeedMoreData;
        }

        *relativeOffset += fieldSize;
        return FieldValueCheckStatus::Valid;
    }
    else
        return FieldValueCheckStatus::NeedMoreData;
}

void PacketDispatcher::InitTable(const PacketDescriptor *PacketDispatcherTable)
{
    typedef PacketDescriptor *PacketDescPtr;

    _dispatcherTable = new PacketDescPtr[_tableRowCount];
    for(uint i = 0; i < _tableRowCount; i++ )
    {
        _dispatcherTable[i] = new PacketDescriptor
        {
            PacketDispatcherTable[i].rowDef,
            PacketDispatcherTable[i].count,
            PacketDispatcherTable[i].callMethod
        };
//        _dispatcherTable[i]->rowDef = PacketDispatcherTable[i].rowDef;
//        _dispatcherTable[i]->count = PacketDispatcherTable[i].count;
//        _dispatcherTable[i]->callMethod = PacketDispatcherTable[i].callMethod;
    }
}

PacketDispatcher::~PacketDispatcher()
{
    if ( _dispatcherTable )
    {
        for(uint i = 0; i < _tableRowCount; i++)
        {
            if (_dispatcherTable[i])
                delete _dispatcherTable[i];
        }
        delete[] _dispatcherTable;
    }
}

void PacketDispatcher::LogInfo(FieldValueCheckStatus prevStatus, uint relativeOffset, uint rowIndex, uint columnIndex, int bytesRead)
{
    if (_outputDebug)
    {
        QString name = rowIndex < _tableRowCount ?
                    GetDispatcherTable()[rowIndex]->rowDef[columnIndex]->Name() :
                    QString("rowIndex >= %1")
                        .arg( _tableRowCount );
        qDebug().noquote()<< getLogPrefix()
                << QString("Status after %1 (prev:%2) "
                            "[ %3 rowIndex: %4,columnIndex: %5,relativeOffset: %6 bytesRead: %7]")
                    .arg( MatchResultToStr( _status ) )
                    .arg( MatchResultToStr( prevStatus ) )
                    .arg( name )
                    .arg( rowIndex )
                    .arg( columnIndex )
                    .arg( relativeOffset )
                    .arg(bytesRead);
    }
}

void PacketDispatcher::LogInfoBasedOnStatus(uint relativeOffset, FieldValueCheckStatus prevStatus, uint columnIndex, uint rowIndex, int bytesRead)
{
    if (prevStatus != _status
             && prevStatus == FieldValueCheckStatus::Match
             && columnIndex > 1)
        LogInfo(prevStatus, relativeOffset, rowIndex, columnIndex, bytesRead);
}

TransportRequestStatus PacketDispatcher::SendResponse(const QByteArray &data)
{
    if( _transport )
        return _transport->SendRequest( data );
    else
        return REQUEST_TRANSPORT_UNDEFINED;
}

QString PacketDispatcher::MatchResultToStr(FieldValueCheckStatus status)
{
    static
    const QVector< QPair< FieldValueCheckStatus, QString > > readableStatuses =
    {
        { FieldValueCheckStatus::MatchFail,"MatchFail"},
        { FieldValueCheckStatus::Skip,"Skip"},
        { FieldValueCheckStatus::Match,"Match"},
        { FieldValueCheckStatus::RelativeOffsetMode,"RelativeOffsetMode"},
        { FieldValueCheckStatus::InvalidCRC,"InvalidCRC"},
        { FieldValueCheckStatus::Valid,"Valid"},
        { FieldValueCheckStatus::NeedMoreData,"NeedMoreData"}
    };
    for( auto statusDef : readableStatuses )
    {
        if( statusDef.first == status )
        {
            return statusDef.second;
        }
    }
    return QString("Error: %1").arg( (int) status );
}
QByteArray PacketDispatcher::buffer() const
{
    return _buffer;////_buffer;
}

FieldValueCheckStatus PacketDispatcher::status() const
{
    return _status;
}

uint PacketDispatcher::garbageLimit() const
{
    return _garbage_limit;
}

void PacketDispatcher::setGarbageLimit(const uint &garbage_limit)
{
    _garbage_limit = garbage_limit;
}
