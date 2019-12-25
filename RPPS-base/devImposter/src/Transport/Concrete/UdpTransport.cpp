// Description: Транспорт протокола UDP
#include "UdpTransport.h"

#include <QUdpSocket>

CUdpTransport::CUdpTransport( QHash< QString, QString > params,
                              Transform * transform,
                              QObject * parent ) :
    TransportTransform( { "UDP", params }, transform, parent )
{
    Init();
}

CUdpTransport::CUdpTransport(QHash< QString, QString > params,
                              const QList<Replacement>& replacements,
                              QObject * parent ) :
    TransportTransform( { "UDP", params }, replacements, parent )
{
    Init();
}

bool CUdpTransport::IsOpen() const
{
    auto state = UdpSocket()->state();
    bool retval = !m_pDevice.isNull()
        && UdpSocket()->isValid()
        && state == QAbstractSocket::SocketState::BoundState
            ;
    if( !retval )
        emit disconnected();
    return retval;
}

bool CUdpTransport::hasError() const
{
    if (m_pDevice.isNull())
        return true;
    QUdpSocket *socket = UdpSocket();
    auto state = socket->state();
    auto error = socket->error();
    if (error == QAbstractSocket::SocketError::UnknownSocketError &&
            (state == QAbstractSocket::SocketState::ConnectingState))
        return false;
    return true;
}

int CUdpTransport::errorCode() const
{
    if (!UdpSocket())
        return -1;

    return UdpSocket()->error();
}

QString CUdpTransport::errorString() const
{
    if (!UdpSocket())
        return "Device not created";

    return UdpSocket()->errorString();
}

QString CUdpTransport::getErrorName(int errCode) const
{
    return NameFromEnum(errCode, UdpSocket(), "SocketError");
}

QUdpSocket *CUdpTransport::UdpSocket() const
{
    if (!m_pDevice.isNull())
        return dynamic_cast< QUdpSocket * >( m_pDevice.data() );
    return nullptr;
}

void
CUdpTransport::Init()
{
    Params.type = "UDP";
    m_pDevice = new QUdpSocket( this );
    connect( m_pDevice.data(), SIGNAL( readyRead() ), SLOT( ReadData() ) );
    TransportTransform::Init();
}

bool CUdpTransport::_OpenConnection( QHash< QString, QString > params )
{
    if( auto udpSocket = UdpSocket() )
    {
        auto port = params[ UDPReceivePort ].toUShort();
        bool hostAddressBroadcast =
                params[ UDPHostAddressBroadcast ].toInt();
        if (hostAddressBroadcast)
            return IsOpen();
        return udpSocket->bind(
                    QHostAddress::Any,
                    port,
                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint );
    }
    return IsOpen();
}

void                                        CUdpTransport::ReadData()
{
    if( IsOpen() )
    {
        if( auto udpSocket = UdpSocket() )
        {
            if( udpSocket->hasPendingDatagrams() )
            {
                QByteArray datagram;
                datagram.resize( udpSocket->pendingDatagramSize() );

                QHostAddress hostAddress;
                quint16 port;
                udpSocket->readDatagram( datagram.data(),
                                         datagram.size(),
                                         &hostAddress, &port);
                emit readDataFrom(hostAddress, port);
                if( !datagram.isEmpty() )
                {
                    DataReceived( datagram );
                }
            }
        }
    }
}

int  CUdpTransport::Write( QByteArray data )
{
    if( IsOpen() )
    {
        if( auto udpSocket = UdpSocket() )
        {
            auto params = GetParams();
            bool hostAddressBroadcast =
                    params[ UDPHostAddressBroadcast ].toInt();

            auto written = udpSocket->writeDatagram
            (
                data,
                hostAddressBroadcast ? QHostAddress::Broadcast :
                QHostAddress( Params.GetParam( IPAddress ) ),
                Params.GetParam( UDPSendPort ).toUShort()
            );
            if (written > -1)
            {
                debugOut(data, "SENT");
                return written;
            }
        }
    }
    debugInfoOut("SENT","failed");
    return 0;
}
