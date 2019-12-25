// Description: Транспорт TCP-клиент
#include <QTcpSocket>
#include "TcpClientTransport.h"
#include "CommonUtils.h"

CTcpClientTransport::CTcpClientTransport(const QHash<QString, QString> &params,
                                          Transform * transform,
                                          QObject * parent ) :
    TransportTransform( { "ClientTCP", params }, transform, parent )
{
    Init();
}

CTcpClientTransport::CTcpClientTransport(const QHash<QString, QString> &params,
                                         const QList< Replacement >& replacements,
                                          QObject * parent ) :
    TransportTransform( { "ClientTCP", params }, replacements, parent )
{
    Init();
}

bool CTcpClientTransport::IsOpen() const
{
    bool retval = !m_pDevice.isNull();
    if( retval )
    {
        QTcpSocket *socket = TcpSocket();
        auto state = socket->state();
        auto error = socket->error();
        if( state == QAbstractSocket::SocketState::UnconnectedState ||
            error == QAbstractSocket::SocketError::RemoteHostClosedError )
        {
            if( socket->isOpen() )
                socket->close();
        }
        retval = socket->isValid()
                && socket->isOpen()
                && state == QAbstractSocket::SocketState::ConnectedState;
    }
    return retval;
}
bool CTcpClientTransport::hasError(bool checkForTimeout) const
{
    if (m_pDevice.isNull())
        return true;
    QTcpSocket *socket = TcpSocket();
    auto state = socket->state();
    auto error = socket->error();
    if (not socket->isValid())
        return true;
    if (checkForTimeout)
    {
        if (error == QAbstractSocket::SocketError::SocketTimeoutError &&
                state == QAbstractSocket::SocketState::ConnectedState &&
                  timeoutValue > 0 )
        {
            if (checkTimeout(state))
                return true;
        }
        else
            timeout = QTime();
    }
    if (state == QAbstractSocket::SocketState::ConnectedState)
        return false;
    if (error == QAbstractSocket::SocketError::UnknownSocketError &&
            (state == QAbstractSocket::SocketState::ConnectingState))
        return false;
    return true;
}

int CTcpClientTransport::errorCode() const
{
    if (!TcpSocket())
        return -1;

    return TcpSocket()->error();
}

QString CTcpClientTransport::errorString() const
{
    if (!TcpSocket())
        return "Device not created";

    return TcpSocket()->errorString();
}

QString CTcpClientTransport::getErrorName(int errCode) const
{
    return NameFromEnum(errCode, TcpSocket(), "SocketError");
}

QString CTcpClientTransport::ErrorString() const
{
    if (m_pDevice.isNull())
        return "QIODevice is null";
    QTcpSocket *socket = TcpSocket();
    auto error = socket->error();
    return QString("%1 - %2")
            .arg( (uint)error)
            .arg( socket->errorString() );
}
void CTcpClientTransport::Init()
{
    if (m_pDevice.isNull() == false)
        m_pDevice->deleteLater();
    m_pDevice = new QTcpSocket( this );
    connectSlots();
    TransportTransform::Init();
}
void CTcpClientTransport::connectSlots()
{
#ifdef ALL_SIGNALS
    connect(m_pDevice.data(), SIGNAL(aboutToClose()), this,
            SIGNAL(disconnected()));
    connect(m_pDevice.data(), SIGNAL(readChannelFinished()), this,
            SIGNAL(disconnected()));
#endif
    QTcpSocket* socket = TcpSocket();
    Qt::ConnectionType connType = Qt::AutoConnection;
    if (connType == Qt::QueuedConnection)
    {
        qRegisterMetaType< QAbstractSocket::SocketError >( "QAbstractSocket::SocketError" );
        qRegisterMetaType< QAbstractSocket::SocketState >( "QAbstractSocket::SocketState" );
    }
    connect( socket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),
             SLOT( ConnectionStateChanged( QAbstractSocket::SocketState ) ),
             connType ) ;
    connect( socket, SIGNAL( readyRead() ), SLOT( ReadData() ),
             connType );
    connect( socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)),
             connType );
    connect( socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()),
             connType );
}

QTcpSocket *CTcpClientTransport::TcpSocket() const
{
    if (!m_pDevice.isNull())
        return qobject_cast<QTcpSocket*>( m_pDevice.data() );
    return nullptr;
}
bool CTcpClientTransport::_OpenConnection( QHash< QString, QString > params )
{
    opening = true;
    m_bReconnect = str2bool( params[ "Reconnect" ] );
    if (params.contains("DisconnectAfter"))
        timeoutValue = params["DisconnectAfter"].toInt();
    return tryToOpen(params);
}

void                                        CTcpClientTransport::_CloseConnection()
{
    opening = false;
    ITransport::_CloseConnection();
}

void                                        CTcpClientTransport::KillReconnectTimer()
{
    if (reconnectTimer) {
        killTimer(reconnectTimer);
        reconnectTimer = 0;
    }
}

void CTcpClientTransport::timerEvent(QTimerEvent *)
{
    if (opening && reconnectTimer) {
        KillReconnectTimer();
        OpenConnection();
    }
}
bool CTcpClientTransport::tryToOpen(QHash< QString, QString > params)
{
    QTcpSocket* socket = TcpSocket();
    bool valid= socket->isValid(), haserrors = hasError();
    if (valid  == false && haserrors)
        Init();
    socket = TcpSocket();
    auto state = socket->state();
    if(state != QAbstractSocket::SocketState::ConnectingState &&
        state != QAbstractSocket::SocketState::HostLookupState)
        connectTo(params, socket);
    return IsOpen();
}
void CTcpClientTransport::connectTo
(const QHash< QString, QString >& params, QTcpSocket* socket)
{
    uint keepAlive = str2bool( params[ "KeepAliveOption" ] );
    QString ip = params[ "ServerIP" ];
    uint port = params[ "Port" ].toUInt();
    socket->connectToHost( ip, port );
    socket->setSocketOption(QAbstractSocket::SocketOption::
                            KeepAliveOption, keepAlive);
}

void CTcpClientTransport::ConnectionStateChanged( QAbstractSocket::SocketState state )
{
    emit stateChanged( state );
    uint MORE_DIAG = isVerbose();
    if (MORE_DIAG)
    {
        if (hasError(true))
            Log("Socket error: " + ErrorString() + " " +
                   Params.params[ "ServerIP" ] +
                    ":" + Params.params[ "Port" ]);
    }
    if( state == QAbstractSocket::UnconnectedState )
    {
        if( isConnected )
        {
            Log(Params.getParamStr(" ") +
                          "ConnectionStateChanged: Disconnected");
            isConnected = false;
            emit disconnected();
            emit socketDisconnected();
        }
        if( opening && m_bReconnect )
        {
            if (!reconnectTimer) {
                reconnectTimer = startTimer(500);
            }
        }
    }
    else if( state == QAbstractSocket::ConnectedState )
    {
        KillReconnectTimer();
        Log( Params.getParamStr(" ") +
                      "ConnectionStateChanged: Connected");
        isConnected = true;
        emit connected();
        emit socketConnected();
    }
    else if (MORE_DIAG)
    {
        if ( state == QAbstractSocket::ConnectingState )
        {
            Log(Params.getParamStr(" ") + "ConnectionStateChanged: Connecting...");
        }
        else if ( state == QAbstractSocket::HostLookupState )
        {
            Log(Params.getParamStr(" ") + "ConnectionStateChanged: Host lookup");
        }
        else if ( state == QAbstractSocket::ClosingState)
        {
            Log(Params.getParamStr(" ") +
                          "ConnectionStateChanged: Closing...");
        }
    }
}

TransportRequestStatus CTcpClientTransport::SendRequest( const QByteArray data )
{
    QTcpSocket* socket = TcpSocket();

    if( socket->state() == QAbstractSocket::ConnectedState )
    {
        if (socket->isValid())
            return TransportTransform::SendRequest( data );
        socketErrorOut(socket);
    }
    return REQUEST_TRANSPORT_UNAVAILABLE;
}

void CTcpClientTransport::socketErrorOut(QTcpSocket* socket)
{
    qWarning().noquote() << getLogPrefix()
           << "Socket error: " << ErrorString() << " "
           << Params.params["ServerIP" ]
           << ":" << Params.params["Port" ]
           << " socket is valid: " << socket->isValid();
}

bool CTcpClientTransport::checkTimeout(QAbstractSocket::SocketState state) const
{
    bool moreErrorDiag = isVerbose("ERROR");
    if (timeout.isNull())
    {
        if (moreErrorDiag)
            qDebug().noquote() << "1st SocketTimeoutError...";
        timeout.start();
    }
    else if (timeout.isValid() &&
             state == QAbstractSocket::SocketState::ConnectedState)
    {
        if (moreErrorDiag)
            qDebug().noquote() <<
              QString("SocketTimeoutError elapsed: %1ms")
                        .arg(timeout.elapsed());
        if (timeoutValue > 0 && timeout.elapsed() > timeoutValue)
        {
            qDebug().noquote() <<
              QString("Socket error timed out expired after %1ms")
                        .arg(timeout.elapsed());
            timeout = QTime();
            //CloseConnection();
            return true;
        }
    }
    return false;
}

void CTcpClientTransport::onError(QAbstractSocket::SocketError error)
{    
    if (isVerbose("ERROR"))
    {
        QTcpSocket* socket = TcpSocket();
#ifdef CHECK_TIMEOUT
        if (error == QAbstractSocket::SocketError::SocketTimeoutError)
        {
            auto state = socket->state();
            checkTimeout(state);
        }
#else
        Q_UNUSED(error);
#endif
        if (socket->isValid())
            return;
        socketErrorOut(socket);
    }
}
