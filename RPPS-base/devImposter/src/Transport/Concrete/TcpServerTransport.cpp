// Description: Транспорт TCP-сервер
#include "TcpServerTransport.h"

CTcpServerTransport::CTcpServerTransport( QHash< QString, QString > params,
                                          Transform * transform,
                                          QObject * parent ) :
    TransportTransform( { "ServerTCP", params }, transform, parent )
{
    Init();
}

CTcpServerTransport::CTcpServerTransport( QHash< QString, QString > params,
                                          QList< Replacement > replacements,
                                          QObject * parent ) :
    TransportTransform( { "ServerTCP", params }, replacements, parent )
{
    Init();
}

CTcpServerTransport::~CTcpServerTransport()
{
    CloseSockets();
}

void                                        CTcpServerTransport::CloseSockets()
{
    while( listeners.count() )
    {
        auto socket = listeners.takeFirst();
        socket->close();
        socket->deleteLater();
    }
}

bool CTcpServerTransport::hasError() const
{
    if (m_pDevice.isNull())
        return true;
    QTcpSocket *socket = dynamic_cast< QTcpSocket * >( m_pDevice.data() );
    auto state = socket->state();
    if (state == QAbstractSocket::SocketState::ConnectedState)
        return false;
    auto error = socket->error();
    if (error == QAbstractSocket::SocketError::UnknownSocketError &&
            (state == QAbstractSocket::SocketState::ConnectingState))
        return false;
    return true;
}

int CTcpServerTransport::errorCode() const
{
    if (m_pServer.isNull())
        return -1;

    return m_pServer->serverError();
}

QString CTcpServerTransport::errorString() const
{
    if (m_pServer.isNull())
        return "Device not created";

    return m_pServer->errorString();
}

QString CTcpServerTransport::getErrorName(int errCode) const
{
    if (m_pServer.isNull())
        return "Device not created";

    QMetaObject metaObject = m_pServer->staticMetaObject;
    auto enumIndex = metaObject.indexOfEnumerator("SocketError");
    auto metaEnum = metaObject.enumerator(enumIndex);

    return metaEnum.valueToKey(errCode);
}
bool                                        CTcpServerTransport::IsOpen() const
{
    bool retval = !m_pServer.isNull() && m_pServer->isListening();

    if( !retval )
    {
        emit disconnected();
    }
    return  retval;
}

void CTcpServerTransport::ProcessNewConnection()
{
    if( m_pServer.isNull())
    {
        emit disconnected();
        return;
    }

    QTcpSocket * socket = m_pServer->nextPendingConnection();

    reportHost(socket);
    qRegisterMetaType< QAbstractSocket::SocketState >( "QAbstractSocket::SocketState" );
    qRegisterMetaType< QAbstractSocket::SocketError >( "QAbstractSocket::SocketError" );
    connect( socket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),
             SLOT( ConnectionStateChanged( QAbstractSocket::SocketState ) ), Qt::QueuedConnection );
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(onError(QAbstractSocket::SocketError)));

    connect( socket, SIGNAL( disconnected() ),this,
             SLOT( connectionEstablished() ), Qt::QueuedConnection );

    connect( socket, SIGNAL( readyRead() ), SLOT( ReadFromSocket() ), Qt::QueuedConnection );
    listeners.append( socket );
}

void CTcpServerTransport::reportHost(QAbstractSocket *socket)
{
    QHostAddress address = socket->peerAddress();
    uint port = socket->localPort();
    if (address.toIPv4Address())
    {
        if (socket->state() !=
                QAbstractSocket::ConnectedState)
            emit disconnected();
        qDebug().noquote() << address.toString() << ":" << port
             << ( socket->state() ==
                  QAbstractSocket::ConnectedState?
                      " - connected" :
                      " - disconnecting");
    }
    else
    {
        qDebug().noquote() << "Disconnected";
        emit disconnected();
    }
}

void CTcpServerTransport::onError(QAbstractSocket::SocketError error)
{
    //qDebug().noquote() << "SocketError: " << error;
    if (error == QAbstractSocket::SocketError::RemoteHostClosedError)
        connectionEstablished();
}
void CTcpServerTransport::ConnectionStateChanged( QAbstractSocket::SocketState state )
{
    if( state == QAbstractSocket::UnconnectedState )
    {
        if( auto socket = qobject_cast< QTcpSocket * >( sender() )  )
        {
            disconnect( socket );
            if( listeners.contains( socket ) )
            {
                listeners.removeOne( socket );
            }
            socket->close();
            socket->deleteLater();
        }

        m_pDevice = nullptr;

        if( listeners.count() )
        {
            m_pDevice = listeners.first();
        }
    }
    else if( state == QAbstractSocket::ConnectedState )
        connectionEstablished();
}

void CTcpServerTransport::connectionEstablished()
{
    if( auto socket = qobject_cast< QTcpSocket * >( sender() )  )
    {
        reportHost( socket );
    }
}
TransportRequestStatus                      CTcpServerTransport::SendRequest( const QByteArray data )
{
    TransportRequestStatus result = REQUEST_SEND_SUCCESS;
    for( auto socket : listeners )
    {
        m_pDevice = socket;
        auto sendResult = TransportTransform::SendRequest( data );
        if( sendResult != REQUEST_SEND_SUCCESS )
        {
            result = sendResult;
        }
    }

    return result;
}

void                                        CTcpServerTransport::Init()
{
    m_pServer.reset( new QTcpServer( this ) );
    connect( m_pServer.data(), SIGNAL( newConnection() ), SLOT( ProcessNewConnection() ), Qt::QueuedConnection );
    TransportTransform::Init();
}

bool                                        CTcpServerTransport::_OpenConnection( QHash< QString, QString > params )
{
    if( m_pServer )
    {
        m_pServer->listen( QHostAddress::Any, params[ PortIP ].toUInt() );
    }
    return IsOpen();
}

void                                        CTcpServerTransport::_CloseConnection()
{
    if( m_pServer )
    {
        CloseSockets();
        m_pServer->close();
    }
}

void                                        CTcpServerTransport::ReadFromSocket()
{
    if( auto socket = qobject_cast< QTcpSocket * >( sender() )  )
    {
        m_pDevice = socket;
        ReadData();
    }
}


