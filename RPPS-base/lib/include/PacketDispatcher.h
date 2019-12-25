/*
Copyright (c) 2017-2019 Burka, Borovitski, Golyapa

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// Description: Разбор структуры пакета
#pragma once

#include <QObject>
#include "ImposterInterfaces.h"
#include "Transport.h"
#include "CommonUtils.h"

#include "FieldDescriptor.h"

class PacketDispatcher :
    public                                  QObject
{
    Q_OBJECT
    uint _garbage_limit = 0;
protected:
    uint                                    _tableRowCount;
    ITransport *                            _transport;
    TransportRequestStatus                  _result;
    FieldValueCheckStatus                   _status;
    ////QByteArray                              _buffer;
    char* _buffer=nullptr;
    uint buffer_upper_bound=0, current_buffer_size = 0;
    uint offset_in_buffer=0;
    PacketDescriptor **                     _dispatcherTable;
    bool                                    _outputDebug;
public:
    explicit                                PacketDispatcher
    (
        ITransport *                        dataProvider,
        QObject *                           parent                      = nullptr
    );
    TransportRequestStatus                  Dispatch( const uchar * buffer, uint buffSize, uint & bytesRead );
    static
    QString                                 MatchResultToStr( FieldValueCheckStatus status );
    void                                    LogInfo(FieldValueCheckStatus prevStatus, uint relativeOffset, uint rowIndex, uint columnIndex, int bytesRead);
    void                                    LogInfoBasedOnStatus(uint relativeOffset, FieldValueCheckStatus prevStatus, uint columnIndex, uint rowIndex, int bytesRead);
    void                                    DebugOutput(const QByteArray &data, int bytesRead);
    virtual void                            SetTransport( ITransport * transport );
    ITransport *                            GetTransport() const
    { return _transport; }
    QByteArray buffer() const;
    void discard()
    {
        ////_buffer.clear();
        buffer_upper_bound=0;
    }
    FieldValueCheckStatus status() const;

    uint garbageLimit() const;
    void setGarbageLimit(const uint &garbageLimit);

signals:
    void                                    dispatchResult( TransportRequestStatus );
    void                                    onDispatch( const QByteArray & data );
    void                                    dispatchPacket(PacketDescriptor *desc, const uchar * buffer, uint buffSize, uint bytesRead );
public slots:
    virtual void                            Dispatch( const QByteArray & data );
protected:
    virtual
    PacketDescriptor **                     GetDispatcherTable() = 0;

    FieldValueCheckStatus                   UpdateRelativeOffsetFromSizeStorage
    (
            const PacketFieldDescriptor *desc,
            const uchar *buffer, uint buffSize,
            uint *const relativeOffset
    );

    void InitTable(const PacketDescriptor *PacketDispatcherTable);
    ~PacketDispatcher();
    TransportRequestStatus                           SendResponse( const QByteArray & data );
    template<typename PACKET, typename CLASS>
    PacketDispatcherMethodCall  GetMethodCall
    (
            void(CLASS::*signalMethod)(const PACKET &)
    )
    {
        return
        [this, signalMethod]( const uchar * buffer, uint index, uint & bytesRead )
        {
            return PacketDispatcher::DispatchPacket< PACKET, CLASS >
                (buffer, index, bytesRead, static_cast<CLASS*>(this), signalMethod);
        };
    }

    template<typename PACKET, typename CLASS>
    PacketDispatcherMethodCall  GetMethodCallVarSize
    (
            void(CLASS::*signalMethod)(const PACKET &)
    )
    {
        return
        [this, signalMethod]( const uchar * buffer, uint buffSize, uint & bytesRead )
        {
            Q_UNUSED(bytesRead)

            QByteArray qbyteArrBuff( ( char * ) buffer, buffSize );

            PACKET pack;
            if (!pack.Assign( qbyteArrBuff )) {
                return TransportRequestStatus::REQUEST_NEED_MORE_DATA;
            }

            emit (static_cast<CLASS*>(this)->*signalMethod)( pack );
            return TransportRequestStatus::REQUEST_SEND_SUCCESS;
        };
    }
    template<typename PACKET, typename CLASS >
    TransportRequestStatus DispatchPacket
    (
        const uchar *                       buffer,
        uint                                buffSize,
        uint &                              bytesRead,
        CLASS *                             sender,
        void(CLASS::*signal)(const PACKET &) = nullptr
    )
    {
        PACKET msg;
        auto expectedSize = msg.expectedSize();

        if( buffSize >=expectedSize )
        {
            bytesRead = expectedSize;

            QByteArray qbyteArrBuff( ( char * ) buffer, buffSize );
            msg.FromByteArray( qbyteArrBuff );

            if( msg.IsValidMessage() )
            {
                emit ( sender->*signal)( msg );
                return TransportRequestStatus::REQUEST_SEND_SUCCESS;
            }
            return TransportRequestStatus::REQUEST_CRC_ERROR;
        }
        return TransportRequestStatus::REQUEST_NEED_MORE_DATA;
    }
};
#if __GNUC__ == 4 && __GNUC_MINOR__ > 7
#define DISPATCHER_ENTRY(CMD, PACKET_CLASS, SIGNAL_METHOD) \
    CMD, arraySize( CMD ), \
    GetMethodCall<PACKET_CLASS>(&SIGNAL_METHOD)//,QString(#SIGNAL_METHOD"::"#PACKET_CLASS"::"#CMD)

#define DISPATCHER_VARSIZE_ENTRY(CMD, PACKET_CLASS, SIGNAL_TO_EMIT) \
    CMD, arraySize( CMD ), \
    GetMethodCallVarSize<PACKET_CLASS>(&SIGNAL_TO_EMIT)//, #SIGNAL_TO_EMIT"::"#CMD
#else
#define DISPATCHER_ENTRY(CMD, PACKET_CLASS, SIGNAL_METHOD) \
    CMD, arraySize( CMD ), \
    [this]( const uchar * buffer, uint index, uint & bytesRead )\
    {\
        if (_outputDebug)\
            qWarning().noquote() << QString("Calling method: %1").arg( #SIGNAL_METHOD"::"#PACKET_CLASS"::"#CMD ) ;\
        return DispatchPacket< PACKET_CLASS >\
         (buffer, index, bytesRead, this, &SIGNAL_METHOD);\
    }

#define DISPATCHER_VARSIZE_ENTRY(CMD, PACKET_CLASS, SIGNAL_TO_EMIT) \
    CMD, arraySize( CMD ), \
    [this]( const uchar * buffer, uint buffSize, uint & bytesRead ) \
    {\
        Q_UNUSED(bytesRead)\
        QByteArray qbyteArrBuff( ( char * ) buffer, buffSize );\
        PACKET_CLASS pack;\
        auto read = pack.Assign( qbyteArrBuff );\
        if (read == 0 || (read != 1 && bytesRead > read))\
            return TransportRequestStatus::REQUEST_NEED_MORE_DATA;\
        if (read > 1)\
            bytesRead = read;\
        emit SIGNAL_TO_EMIT( pack );\
        if (_outputDebug)\
            qDebug().noquote() << QString("Calling method: %1 %2").arg( #SIGNAL_TO_EMIT"<<"#CMD ).arg(read) ;\
        return TransportRequestStatus::REQUEST_SEND_SUCCESS;\
    }
#endif
