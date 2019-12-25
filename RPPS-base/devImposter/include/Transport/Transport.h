// Description: Интерфейс транспорта
#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include <QObject>
#include <QIODevice>
#include <QMutex>
#include <QPointer>

#include <QHash>
#include <QDebug>

#include "CommonUtils.h"
#include "Transform.h"

enum TransportRequestStatus
{
    REQUEST_SEND_SUCCESS = 0,
    REQUEST_RESPONSE_SUCCESS = REQUEST_SEND_SUCCESS,
    REQUEST_TRANSPORT_UNAVAILABLE,
    REQUEST_TRANSPORT_UNDEFINED,
    REQUEST_TRANSPORT_FAILED,
    REQUEST_SEND_UNKNOWN_ERROR,
    REQUEST_CRC_ERROR,
    REQUEST_NEED_MORE_DATA,
    REQUEST_RESPONSE_UNKNOWN_ERROR
};

struct TransportParams
{
    QString                                 type;
    QHash< QString, QString >               params;
    QString                                 name;

    QString                                 GetType() const
    {
        return type;
    }
    QString                                 GetName() const
    {
        return name;
    }
    QString                                 GetParam
    (
        const QString &                     key,
        const QString &                     defaultValue                = QString()
    ) const
    {
        return params.value( key, defaultValue );
    }
    void                                    DumpParams(const char * sep="\n") const;
    QString getParamStr(const char * sep) const;
};

class ITransport :
    public                                  QObject
{
    Q_OBJECT
protected:
    explicit                                ITransport( TransportParams params, QObject * parent = nullptr );
public:
    virtual                                 ~ITransport()
    {
        CloseConnection();
//        if( ! m_pDevice.isNull() )
//            m_pDevice->deleteLater();
    }

    virtual bool                            IsOpen() const;

    bool                                    OpenConnection( QHash< QString, QString > params = QHash< QString, QString >() );
    void                                    CloseConnection();
    QString                                 Type() const
    {
        return Params.type;
    }
    QHash< QString, QString >               GetParams() const
    {
        return Params.params;
    }
    void                                    DumpParams() const
    {
        Params.DumpParams();
    }
    virtual void                            SetParams( const QHash< QString, QString >& params);
    virtual void                            SetParams( TransportParams& params);
    template<typename MSG>
    TransportRequestStatus                  SendRequest( MSG & msg )
    {
        return SendRequest( msg.PrepareToSend() );
    }
    virtual QString                         ErrorString() const;
    virtual bool hasError() const = 0;
    virtual int errorCode() const = 0;
    virtual QString errorString() const = 0;
    virtual QString getErrorName(int errCode) const = 0;
    static
    QString                                 RequestResultToStr( TransportRequestStatus status );
    void SetDumpFileName(const QHash< QString, QString >& params);
    uint isVerbose(const char *parameter = "CONNECTION") const
    {
        return debugParam(parameter);
    }
    void debugInfoOut(const QString &msgOut);
    virtual void moveToThread(QThread *thread);

protected:
    template <class T>
    QString NameFromEnum(int errCode, T * obj, const QString & enumName) const
    {
        if (!obj)
            return "Device not created";

        QMetaObject metaObject = obj->staticMetaObject;
        auto enumIndex = metaObject.indexOfEnumerator(enumName.toLatin1());
        auto metaEnum = metaObject.enumerator(enumIndex);

        return metaEnum.valueToKey(errCode);
    }

    virtual void                            _SetParams( const QHash< QString, QString >& params);
    virtual void                            _SetParams( TransportParams& params );
    virtual bool                            _OpenConnection( QHash< QString, QString > params ) = 0;

    virtual int                             Write( QByteArray data );
    virtual QByteArray                      ReadAll();
    virtual void                            _CloseConnection();
    void                                    SetParam( const QString & key, const QString & value );

    void                                    Log( const QString & msg );

    virtual void                            DataReceived( const QByteArray & data );
    int                                     DeviceWaitDelay();
    int                                     BufferThreshold();
    void                                    DumpDataIntoFile(const QByteArray &data);

signals:
    void                                    dataSent( const QByteArray & data );
    void                                    dataReceived( const QByteArray & data );
    void                                    connected();
    void                                    disconnected() const;
    void                                    log( const QString & msg );
    void                                    notifyStatus( TransportRequestStatus ) const;

public slots:
    virtual void setReplacements( const QList< Replacement >& ){}
    virtual TransportRequestStatus          SendRequest( const QByteArray data );
    virtual void                            ReadData();
protected:
    QPointer< QIODevice >                   m_pDevice;
    TransportParams                         Params;
    uint debugParam(const QString & param) const;
    void debugOut(const QByteArray& data, const QString &paramName);
    void debugInfoOut(const QString &paramName, const QString &info);

private:
    QString                                 dataReceivedFileName;
    friend class PacketDispatcher;
};

const QString TestUDPServerPort = "13950";
const QString TestUDPClientPort = "13960";

const QString IPAddress         = "IP";
const QString LocalHostIP       = "127.0.0.1";

const QString UDPHostAddressBroadcast = "UDPHostAddressBroadcast";
const QString UDPSendPort       = "SendPort";
const QString UDPReceivePort    = "ReceivePort";
const QString PortIP            = "Port";

const QString deviceWaitDelay   = "DeviceWaitDelay";
const QString bufferThreshold   = "BufferThreshold";
const QString TCPTestPort       = "32011";
const QString DumpDataToFile    = "DumpDataToFile";

namespace Transport
{
    QString GetTypeSettingName();
    QStringList GetTypeNames();

    template<typename CONT, typename StringList >
    TransportParams GetTransportParams(const CONT &settings, const StringList & keys)
    {
        TransportParams params;
        QStringList transport_types = Transport::GetTypeNames();
        for( auto key :  keys)
        {
            QVariant variant = settings[ key ];
            const QString value( variant.toString() );
            if( key == Transport::GetTypeSettingName())
            {
                if( transport_types.contains( value.trimmed() ) )
                    params.type = value;
            }
            else
                params.params[ key ] = value;
        }
        return params;
    }

    template<typename CONT>
    TransportParams GetTransportParams(const CONT & cont)
    {
        return GetTransportParams(cont, cont.keys());
    }
}
#endif // ITRANSPORT_H
