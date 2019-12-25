// Description: Тест взаимодействия с устройством
#ifndef CTEST_H
#define CTEST_H
#include <QObject>
#include <QString>
#include <QDebug>

#include "Transport.h"

class CTest:
	public QObject
{
    Q_OBJECT
public:
    CTest( const QString & name, QObject * parent = nullptr, bool useTestServer = true ) :
        QObject( parent ),
        m_sName( name ),
        m_bFailed( false ),
        serverDispatchStatus( REQUEST_RESPONSE_UNKNOWN_ERROR ),
        clientDispatchStatus( REQUEST_RESPONSE_UNKNOWN_ERROR ),
        passedTests( 0 ),
        failedTests( 0 ),
        useTestServer ( useTestServer ),
        testRequest( true ),
        testAnswer( true )
    { serverData.chop(0); clientData.chop(0); }
    QString                                 Name() const                { return m_sName; }
    bool                                    IsPassed() const              { return !m_bFailed; }

public slots:
    void                                    SetFail()                   { m_bFailed = true; }
    void                                    SetTestRequest( bool value ) { testRequest = value; }
    void                                    SetTestAnswer( bool value ) { testAnswer = value; }

    void                                    ClientDataSent( const QByteArray & data )
    {
        clientData.append(data);
    }
    void                                    ServerDataReceived( const QByteArray & data )
    {
        receivedByServer.append( data );
    }
    void                                    ServerDataSent( const QByteArray & data )
    {
        serverData.append(data);
    }
    void                                    ClientDataReceived( const QByteArray & data )
    {
        receivedByClient.append( data );
    }

    void                                    ServerResultReceived( TransportRequestStatus status )
    {
        if( serverDispatchStatus != REQUEST_SEND_SUCCESS )
        {
            serverDispatchStatus = status;
        }
    }

    void                                    ClientResultReceived( TransportRequestStatus status )
    {
        if( clientDispatchStatus != REQUEST_RESPONSE_SUCCESS )
        {
            clientDispatchStatus = status;
        }
    }

    void                                    CheckAnswer( const QString & answerName )
    {
        receivedAnswerName = answerName;
    }

    void                                    OnDataCount( int expected, int received, const QString & field )
    {
        expectedCount[ field ] = expected;
        receivedCount[ field ] = received;
        variadicFields << field;
    }

    void                                    OnWrongDataReceived( QVariant expected, QVariant received )
    {
        if( variadicFields.count() )
        {
            auto field = variadicFields.last();
            unexpectedData[ field ].append( { expected.toString(), received.toString() } );
        }
    }

    void                                    OnWrongDataReceived( const QString field, QVariant expected, QVariant received )
    {
        unexpectedFields << field;
        unexpectedData[ field ].append( { expected.toString(), received.toString() } );
    }

    void                                    FullTest()
    {
        if( testRequest )
        {
            TestClientData();
            if(useTestServer)
            {
                TestServerReceived();
                TestServerDispatch();
            }
        }
        if( testAnswer )
        {
            if(useTestServer)
            {
                TestServerData();
            }
            TestClientReceived();
            TestClientDispatch();
            TestAnswerName();
            for( auto field : variadicFields )
            {
                TestDataCount( field );
                TestData( field, "Variadic " );
            }
            for( auto field : unexpectedFields )
            {
                TestData( field );
            }
        }
    }

    QString                                 SetPassed( bool passed )
    {
        if( passed )
        {
            passedTests++;
        }
        else
        {
            failedTests++;
            SetFail();
        }
        return GetPassedText( passed );
    }
    QString                                 GetPassedText( bool passed )
    {
        return QString( "[%1]: %2\t" )
                .arg( m_sName )
                .arg( passed ? "PASSED" : "FAILED" );
    }

    void                                    TestClientData()
    {
        m_sResult += SetPassed( !clientData.isEmpty() ) + "Client sent: "
                     + clientData.toHex() + "\n";
    }
    void                                    TestServerData()
    {
        m_sResult += SetPassed( !serverData.isEmpty() ) + "Server sent: "
                     + serverData.toHex() + "\n";
    }
    QString                                 SetPassedByReceived
    (
        const QByteArray &                  sent,
        const QByteArray &                  received
    )
    {
        return SetPassed
                (
                    !received.isEmpty()
                    &&
                    ( received.toHex() == sent.toHex() )
                );
    }

    void                                    TestServerReceived()
    {
        m_sResult += SetPassedByReceived( clientData, receivedByServer )
                     + "Server received: " + receivedByServer.toHex() + "\n";
    }
    void                                    TestClientReceived()
    {
        if (useTestServer)
            m_sResult += SetPassedByReceived( serverData, receivedByClient )
                     + "Client received: " + receivedByClient.toHex() + "\n";
        else
            m_sResult +=
                             "Client received: " + receivedByClient.toHex() + "\n";
    }
    void                                    TestServerDispatch()
    {
        CheckDispatchResult( serverDispatchStatus, "Server" );
    }
    void                                    TestClientDispatch()
    {
        CheckDispatchResult( clientDispatchStatus, "Client" );
    }
    void                                    CheckDispatchResult
    (
        TransportRequestStatus                       status,
        const QString &                     type
    )
    {
        m_sResult += SetPassed( status == REQUEST_RESPONSE_SUCCESS );
        m_sResult += type + " dispatch result: ";
        m_sResult += ITransport::RequestResultToStr( status ) + "\n";
    }

    void                                    TestAnswerName()
    {
        QString answerName = receivedAnswerName;
        m_sResult += SetPassed( !answerName.isEmpty()
                                &&
                                ( answerName == Name() )
                                );

        if( answerName != Name() )
        {
            if( answerName.isEmpty() )
            {
                m_sResult += "Answer: Failed to receive response\n";
            }
            else
            {
                m_sResult += "Answer: Invalid, name = " + answerName + "\n";
            }
        }
        else
        {
            m_sResult += "Answer: Valid\n";
        }
    }    

    void                                    TestDataCount( const QString & field )
    {
        int expected = expectedCount[ field ];
        int received = receivedCount[ field ];
        bool passed = expected == received;

        m_sResult += SetPassed( passed )
                     + QString( "Data count [%1] : " ).arg( field );
        if( passed )
        {
            m_sResult += "Valid";
        }
        else
        {
            m_sResult += QString( "Expected: %1, received: %2" )
                         .arg( expected )
                         .arg( received );
        }
        m_sResult += "\n";
    }

    void                                    TestData( const QString & field, const QString & prefix = QString() )
    {
        auto unexpected = unexpectedData[ field ];

        m_sResult += SetPassed( !unexpected.count() )
                     + QString( "%1Field [%2]: " ).arg( prefix ).arg( field );

        for( int i = 0; i < qMin(unexpected.count(), 3 ); ++i )
        {
            auto diff = unexpected.at( i );
            m_sResult += QString( "\n\tFAILED\tExpected: %1, received: %2" )
                         .arg( diff.first )
                         .arg( diff.second );
        }

        if( unexpected.count() > 3 )
        {
            m_sResult += "\n\tFAILED\t...";
        }
        if( !unexpected.count() )
        {
            m_sResult += "Valid";
        }

        m_sResult += "\n";
    }

    QString                                 Result()
    {
        return m_sResult +
                QString( "[%1] PASSED: %2, FAILED: %3" )
                .arg( Name() )
                .arg( passedTests )
                .arg( failedTests );
    }

private:
    QString                                 m_sName;
    bool                                    m_bFailed;
    QString                                 m_sResult;
    QByteArray                              clientData;
    QByteArray                              serverData;
    QByteArray                              receivedByServer;
    QByteArray                              receivedByClient;
    TransportRequestStatus                  serverDispatchStatus;
    TransportRequestStatus                  clientDispatchStatus;
    QString                                 receivedAnswerName;
    int                                     passedTests;
    int                                     failedTests;
    bool                                    useTestServer;
    QHash< QString, int >                   expectedCount;
    QHash< QString, int >                   receivedCount;
    QHash< QString, QVector< QPair< QString, QString > > >
                                            unexpectedData;
    QStringList                             unexpectedFields;
    QStringList                             variadicFields;
    bool                                    testRequest;
    bool                                    testAnswer;
};
#endif //CTEST_H
