// Description: Интерфейс транспорта
#include "Transport.h"
#include "CommonUtils.h"

#include <QFile>
#include <QDataStream>

ITransport::ITransport(TransportParams params, QObject* parent )
    : QObject( parent )
    , m_pDevice( 0 )
{
    Params.type = params.type.trimmed();
    Params.params = params.params;
    SetDumpFileName(Params.params);
}

bool                                        ITransport::IsOpen() const
{
    bool retval = !m_pDevice.isNull() && m_pDevice->isOpen();

    if( !retval )
        emit disconnected();
    return retval;
}

bool                                        ITransport::OpenConnection( QHash< QString, QString > params )
{
    CloseConnection();

    _SetParams( params );

    if( _OpenConnection( GetParams() ) )
    {
        emit connected();
    }

    return IsOpen();
}
void                                        ITransport::CloseConnection()
{
    if( IsOpen() )
    {
        if (isVerbose("CONNECTION"))
            Log("Requested to close connection which is already opened");
        _CloseConnection();
    }
}

void                                        ITransport::SetParams( const QHash< QString, QString > & params )
{
    if( !IsOpen() )
        _SetParams( params );
    else
        if( !OpenConnection( params ) )
            emit notifyStatus( TransportRequestStatus::REQUEST_TRANSPORT_UNAVAILABLE );
}

void                                        ITransport::SetParams( TransportParams & params )
{
    if( params.type == Params.GetType() )
        SetParams( params.params );
    else
    {
        if( params.type.isEmpty() )
            emit notifyStatus( TransportRequestStatus::REQUEST_TRANSPORT_UNAVAILABLE );
        else
        {
            bool active = IsOpen();

            CloseConnection();

            Params.type = params.GetType().trimmed();
            _SetParams( params );

            if( active )
                if( !OpenConnection( params.params ) )
                    emit notifyStatus( TransportRequestStatus::REQUEST_TRANSPORT_UNAVAILABLE );
        }
    }
}

QString ITransport::ErrorString() const
{
    if (m_pDevice.isNull())
        return QString(RequestResultToStr(REQUEST_TRANSPORT_UNDEFINED));
    if (hasError())
        return m_pDevice->errorString();
    return QString();
}

QString ITransport::RequestResultToStr(TransportRequestStatus status)
{
    static const
            QVector< QPair< int, QString > >
            statuses =
    {
        { REQUEST_SEND_SUCCESS, "Success" },
        { REQUEST_TRANSPORT_UNAVAILABLE, "Transport unavailable" },
        { REQUEST_TRANSPORT_UNDEFINED, "Transport undefined" },
        { REQUEST_TRANSPORT_FAILED, "Transport failed" },
        { REQUEST_SEND_UNKNOWN_ERROR, "Error: Failed to send" },
        { REQUEST_CRC_ERROR, "Error: Invalid CRC" },
        { REQUEST_NEED_MORE_DATA, "Incomplete data" },
        { REQUEST_RESPONSE_UNKNOWN_ERROR, "Error: Unknown datagram" }
    };
    for( auto statusDef : statuses )
    {
        if( statusDef.first == status )
        {
            return statusDef.second;
        }
    }
    return QString("Error: %1").arg( status );
}

void ITransport::SetDumpFileName(const QHash< QString, QString >& params)
{
    if (params.contains(DumpDataToFile) && params[DumpDataToFile].isEmpty() == false)
        dataReceivedFileName = params[DumpDataToFile];
}

void                                        ITransport::_SetParams( const QHash< QString, QString > & params )
{
    if( !params.isEmpty() )
        Params.params = params;
    SetDumpFileName(params);
}

void                                        ITransport::_SetParams( TransportParams & params )
{
    if( ( !params.params.isEmpty() ) && ( !( Params.GetType().isEmpty() ) ) )
    {
        Params = params;
        SetDumpFileName(params.params);
    }
}


int                                         ITransport::Write( QByteArray data )
{
    if (IsOpen())
    {
        int written = m_pDevice->write( data );
        if (written > -1)
        {
            if (written < data.size())
            {
                debugInfoOut("SENT",
                     QString("written < data.size() - %1 %2 error: %3")
                     .arg(written)
                     .arg(data.size())
                     .arg(hasError()? ErrorString() : "No"));
            }
            else
                debugOut(data, "SENT");
            return written;
        }
    }
    debugInfoOut("SENT","failed");
    return -1;
}

QByteArray                                  ITransport::ReadAll()
{
    if( ! (m_pDevice.isNull()) )
        if( m_pDevice->isOpen() )
            return m_pDevice->readAll();

    return QByteArray();
}

void                                        ITransport::_CloseConnection()
{
    if( !m_pDevice.isNull() )
        if( m_pDevice->isOpen() )
            m_pDevice->close();
    emit disconnected();
}
void ITransport::SetParam( const QString & key, const QString & value )
{
    Params.params[ key ] = value;
}
void ITransport::Log( const QString & msg )
{
    QString logMsg = QString( "%1 %2" )
            .arg( getLogPrefix() )
            .arg( msg );
    emit log( logMsg );
    qWarning().noquote() << logMsg;
}
void ITransport::debugOut(const QByteArray& data, const QString &paramName)
{
    static const int max_size = 125;
    if (debugParam(paramName))
    {
       auto size = data.size();
       auto truncate = data.left(max_size);
#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR > 8

        QString info =
            size > max_size?
                   QString(" [%1] %2 ...").arg(data.size())
                    .arg(QString(truncate.toHex(' '))):
                    data.toHex(' ');
#else
        QString info = insertDelim(data.size() > 250?
                   QString(" [%1] %2...").arg(data.size())
                    .arg(QString(truncate.toHex())):
                   data.toHex());
#endif
        debugInfoOut(paramName, info);
    }
}
void ITransport::debugInfoOut(const QString & msgOut)
{
    Log(msgOut);
}

void ITransport::moveToThread(QThread *thread)
{
    qDebug().noquote() << "m_pDdevice->thread()" << m_pDevice->thread();
    qDebug().noquote() << "transport's thread()" << this->thread();
    qDebug().noquote() << "thread's thread()" << thread->thread();
    QObject::moveToThread(thread);
#ifdef USE_DYNAMIC_MEMORY
    m_pDevice->moveToThread(thread);
#else
#endif
    qDebug().noquote() << "device->thread()" << m_pDevice->thread();
    qDebug().noquote() << "transport thread()" << this->thread();
    qDebug().noquote() << "m_pDevice->thread()" << m_pDevice->thread();
    qDebug().noquote() << "transport's thread()" << this->thread();
}
void ITransport::debugInfoOut(const QString &paramName, const QString & info)
{
    if (debugParam(paramName))
    {
        QString name = Params.params.contains("Section")?
                Params.params["Section"] : "";
        QString msgOut = name + " " + paramName  + " : " + info;
        debugInfoOut(msgOut);
    }
}
void                                        ITransport::DataReceived( const QByteArray & data )
{
    debugOut(data, QString("RCVD [%1]").arg(data.size()));
    emit dataReceived( data );
}

int                                         ITransport::DeviceWaitDelay()
{
    return GetParams().value( deviceWaitDelay, "10" ).toInt();
}

int                                         ITransport::BufferThreshold()
{
    return GetParams().value( bufferThreshold, "512" ).toInt();
}

void ITransport::DumpDataIntoFile(const QByteArray &data)
{
    if (dataReceivedFileName.isEmpty())
        return;
    try
    {
        QFile dataReceivedFile(dataReceivedFileName);
        dataReceivedFile.open(QIODevice::Append);
        QDataStream dataStream(&dataReceivedFile);
        dataStream.writeBytes( data.data(), data.size() );
        dataReceivedFile.close();
    }
    catch(...)
    {
        qDebug().noquote() << "Received an exception while trying to dump data into a file " + dataReceivedFileName;
    }
}

TransportRequestStatus ITransport::SendRequest( const QByteArray data )
{
    TransportRequestStatus retval( IsOpen() ?
                                   REQUEST_SEND_SUCCESS :
                                   REQUEST_TRANSPORT_UNAVAILABLE );

    if( IsOpen() )
    {
        if (debugParam("TEST_SEND1"))
        {
            int delta = 1;
            for (int i = 0; i < data.size(); i+=delta)
            {
                if( Write( data.mid(i, delta) ) == -1 )
                    retval = REQUEST_TRANSPORT_FAILED;
            }
            char prefix[]{0x40,03,0x0a};
            QByteArray garbage(prefix, 3);
            garbage += QString("garbage").toLocal8Bit();
            Write( garbage );
        }
        else if( Write( data ) == -1 )
            retval = REQUEST_TRANSPORT_FAILED;
    }

    if( retval == REQUEST_SEND_SUCCESS )
        emit dataSent( data );
    else
        emit notifyStatus( retval );
    return retval;
}

void ITransport::ReadData()
{
    int delay = DeviceWaitDelay();
    int threshold = BufferThreshold();

    QByteArray data = QByteArray();

    while( IsOpen() )
    {
        if( m_pDevice->bytesAvailable() && (data.size() < threshold) )
        {
            data.append( ReadAll() );
            m_pDevice->waitForReadyRead( delay );
        }
        else
            break;
    }
    if( data.isEmpty() )
        return;
    DataReceived( data );
    DumpDataIntoFile( data );
}
const QString nameParVerbose = "VERBOSE";

uint ITransport::debugParam(const QString &param) const
{
    QString namesValue = Params.params.contains(nameParVerbose)?
             Params.params[nameParVerbose] : "";
    if (namesValue.isEmpty())
        return 0;
    QStringList names = namesValue.split(":");
    if (names.contains(param))
        return 1;
    return 0;
}
void TransportParams::DumpParams(const char *sep) const
{
    qWarning().noquote() << getParamStr(sep);
}
QString TransportParams::getParamStr(const char *sep) const
{
    QString paramStr;
    paramStr += "Type: ";
    paramStr += type.toStdString().c_str();
    paramStr += sep;
    paramStr += "Name: ";
    paramStr += name.toStdString().c_str();
    paramStr += sep;
    for (auto param : params.keys())
    {
        paramStr += param.toStdString().c_str();
        if (param != nameParVerbose)//skipping value for DEBUG
        {
            paramStr += ":";
            paramStr += params.value(param).toStdString().c_str();
        }
        paramStr += sep;
    }
    return paramStr;
}

const QString IDS_NONE_TRANSPOT_TYPE = "Undefined";
const QString IDS_COM_TRANSPOT_TYPE = "Serial";
const QString IDS_UDP_TRANSPOT_TYPE ="UDP";
const QString IDS_HTTP_TRANSPOT_TYPE = "HTTP";
const QString IDS_CLIENT_TCP_TRANSPOT_TYPE = "ClientTCP";
const QString IDS_SERVER_TCP_TRANSPOT_TYPE ="ServerTCP";
const QString IDS_SETTING_TYPE = "type";

const QStringList transport_types = { IDS_COM_TRANSPOT_TYPE,
                      IDS_UDP_TRANSPOT_TYPE,
                      IDS_HTTP_TRANSPOT_TYPE,
                      IDS_CLIENT_TCP_TRANSPOT_TYPE/*,
                                                            IDS_SERVER_TCP_TRANSPOT_TYPE*/};
const QString transportTypeName = IDS_SETTING_TYPE;

QString Transport::GetTypeSettingName()
{
    return transportTypeName;
}

namespace Transport
{

QStringList GetTypeNames()
{
    return transport_types;
}

}
