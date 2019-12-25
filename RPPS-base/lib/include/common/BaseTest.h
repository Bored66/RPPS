// Description: Базовый класс тестирования протокола
#ifndef BASETEST_H
#define BASETEST_H

#include <QStringList>

#include "Transport.h"
#include "TransportTransform.h"

#include "Test.h"

class CBaseTest :
    public                                  QObject
{
    Q_OBJECT
public:
    explicit                                CBaseTest( QObject * parent = nullptr );
    void                                    OpenServerConnection(TransportParams serverParams, QList<Replacement> replacements);
    void                                    OpenClientConnection(TransportParams clientParams, QList<Replacement> replacements);
signals:
    void                                    finished();
public slots:
    void                                    CheckAnswer( const QString & slotName );
    void                                    OnDataCount( int expected, int received, const QString & field );
    void                                    OnWrongDataReceived( QVariant expected, QVariant received );
    void                                    OnWrongDataReceived( const QString & field, QVariant expected, QVariant received );
    void                                    StartWaiting();
    void                                    StopWaiting();
    void                                    ClientSendData( const QByteArray & data );
    void                                    ReceiveServerData( const QByteArray & data );
    void                                    ServerSendData( const QByteArray & data );
    void                                    ReceiveClientData( const QByteArray & data );
    void                                    ClientResultReceived( TransportRequestStatus status );
    void                                    ServerResultReceived( TransportRequestStatus status );

protected:
    void                                    timerEvent( QTimerEvent * );

    void                                    OpenTransport(QList< Replacement > replacements, TransportParams clientParams = {"UDP",{}}, TransportParams serverParams = {"UDP",{}});

    void                                    ConnectDispatchers( QObject * server, QObject * client );
    void                                    RunTests( const QStringList & tests, bool useTestServer = true );
protected:
    CTest *                                 m_pCurrentTest;
    int                                     m_iTimer;
    ITransport *                            m_pClientTransport;
    ITransport *                            m_pServerTransport;
    QList< CTest * >                        m_colTests;
};

#endif //BASETEST_H
