// Description: Базовый класс тестирования протокола
#include <QCoreApplication>

#include "TransportFactory.h"

#include "BaseTest.h"

CBaseTest::CBaseTest( QObject * parent ) :
    QObject( parent ),
    m_pCurrentTest( nullptr ),
    m_iTimer( 0 ),
    m_pClientTransport( nullptr ),
    m_pServerTransport( nullptr )
{}
void                                        CBaseTest::CheckAnswer( const QString & slotName )
{
    m_pCurrentTest->CheckAnswer( slotName );
}

void                                        CBaseTest::OnDataCount(int expected, int received , const QString & field)
{
    m_pCurrentTest->OnDataCount( expected, received, field );
}

void                                        CBaseTest::OnWrongDataReceived( QVariant expected, QVariant received )
{
    m_pCurrentTest->OnWrongDataReceived( expected, received );
}

void                                        CBaseTest::OnWrongDataReceived
(
    const QString &                         field,
    QVariant                                expected,
    QVariant                                received
)
{
    m_pCurrentTest->OnWrongDataReceived( field, expected, received );
}

void                                        CBaseTest::StartWaiting()
{
    m_iTimer = startTimer( 200 );
}

void                                        CBaseTest::StopWaiting()
{
    if( !m_iTimer )
        return;

    killTimer( m_iTimer );
    m_iTimer = 0;
}

void                                        CBaseTest::ClientSendData( const QByteArray & data )
{
    m_pCurrentTest->ClientDataSent( data );
}

void                                        CBaseTest::ReceiveServerData( const QByteArray & data )
{
    m_pCurrentTest->ServerDataReceived( data );
}

void                                        CBaseTest::ServerSendData( const QByteArray & data )
{
    m_pCurrentTest->ServerDataSent( data );
}

void                                        CBaseTest::ReceiveClientData( const QByteArray & data )
{
    m_pCurrentTest->ClientDataReceived( data );
}

void                                        CBaseTest::ClientResultReceived( TransportRequestStatus status )
{
    m_pCurrentTest->ClientResultReceived( status );
}

void                                        CBaseTest::ServerResultReceived( TransportRequestStatus status )
{
    m_pCurrentTest->ServerResultReceived( status );
}

void                                        CBaseTest::timerEvent( QTimerEvent * )
{
    StopWaiting();
    m_pCurrentTest->FullTest();
}


void CBaseTest::OpenTransport(QList<Replacement> replacements, TransportParams clientParams, TransportParams serverParams )
{
    OpenClientConnection(clientParams, replacements);
    OpenServerConnection(serverParams, replacements);
}
void CBaseTest::OpenServerConnection(TransportParams serverParams, QList<Replacement> replacements)
{
    if(serverParams.type == "UDP")
    {
        if (serverParams.params.contains( IPAddress )==false)
            serverParams.params[ IPAddress ] = LocalHostIP;
        if (serverParams.params.contains( UDPSendPort )==false)
            serverParams.params[ UDPSendPort ] = TestUDPClientPort;
        if (serverParams.params.contains( UDPReceivePort )==false)
            serverParams.params[ UDPReceivePort ] = TestUDPServerPort;
    }
    if (serverParams.params.count())
    {
        TransportFactory factory;
        m_pServerTransport = factory.Create( serverParams, replacements, this );
        m_pServerTransport->OpenConnection();
        if ( m_pServerTransport->IsOpen() )
            qDebug().noquote() << " Server is opened";
        for (auto param : serverParams.params.keys() )
            qDebug().noquote() << param << " = " << serverParams.params[param];
        connect( m_pServerTransport, SIGNAL( dataSent(QByteArray)), SLOT( ServerSendData( QByteArray ) ), Qt::QueuedConnection );
    }
}

void CBaseTest::OpenClientConnection(TransportParams clientParams, QList<Replacement> replacements)
{
    if (clientParams.type == "UDP")
    {
        if (clientParams.params.contains( IPAddress )==false)
            clientParams.params[ IPAddress ] = LocalHostIP;
        if (clientParams.params.contains( UDPSendPort )==false)
            clientParams.params[ UDPSendPort ] = TestUDPServerPort;
        if (clientParams.params.contains( UDPReceivePort )==false)
            clientParams.params[ UDPReceivePort ] = TestUDPClientPort;
    }

    TransportFactory factory;
    m_pClientTransport = factory.Create( clientParams, replacements, this );
    for (auto param : clientParams.params.keys() )
        qDebug().noquote() << param << " = " << clientParams.params[param];
    m_pClientTransport->OpenConnection();
    if(m_pClientTransport->IsOpen())
        qDebug().noquote() << " Client is opened";

    connect( m_pClientTransport, SIGNAL( dataSent(QByteArray)), SLOT( ClientSendData( QByteArray ) ), Qt::QueuedConnection );
}

void                                        CBaseTest::ConnectDispatchers( QObject * server, QObject * client )
{
    connect( server, SIGNAL( onDispatch( QByteArray ) ), SLOT( ReceiveServerData( QByteArray ) ) );
    connect( server, SIGNAL( dispatchResult( TransportRequestStatus ) ), SLOT( ServerResultReceived( TransportRequestStatus ) ) );

    connect( client, SIGNAL( onDispatch( QByteArray ) ), SLOT( ReceiveClientData( QByteArray ) ) );
    connect( client, SIGNAL( dispatchResult( TransportRequestStatus ) ), SLOT( ClientResultReceived( TransportRequestStatus ) ) );
}

void                                        CBaseTest::RunTests( const QStringList & tests, bool useTestServer )
{
    for( auto test : tests )
    {
        StartWaiting();
        m_pCurrentTest = new CTest( test, this, useTestServer );
        m_colTests << m_pCurrentTest;
        QMetaObject::invokeMethod( this, "Test" + QMetaObject::normalizedSignature( test.toLocal8Bit() ) );
        while( m_iTimer )
        {
            QCoreApplication::processEvents();
        }
    }

    QList< CTest * > failed;
    for( auto test : m_colTests )
    {
        if( test->IsPassed() )
            qDebug().noquote() << "\n" << test->Result();
        else
            failed << test;
    }

    for( auto fail : failed )
        qDebug().noquote() << "\n" << fail->Result();

    qDebug().noquote() << QString( "Total: PASS: %1, FAIL: %2" )
                .arg( tests.count() - failed.count() )
                .arg( failed.count() );

    emit finished();
    QCoreApplication::processEvents();
}
