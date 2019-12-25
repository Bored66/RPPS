// Description: Транспорт протокола HTTP
#include "HttpTransport.h"

CHttpTransport::CHttpTransport( QHash< QString, QString > params,
                                Transform*                 transform,
                                QObject * parent ) :
    CTcpClientTransport( params, transform, parent )
{
    Init();
}

CHttpTransport::CHttpTransport( QHash< QString, QString > params,
                                QList< Replacement >       replacements,
                                QObject * parent ) :
    CTcpClientTransport( params, replacements, parent )
{
    Init();
}

void CHttpTransport::Init()
{
    CTcpClientTransport::Init();
    Params.type = "HTTP";
    SetParam( "Port", "80" );
}

TransportRequestStatus                               CHttpTransport::SendRequest( const QByteArray data )
{
    QString authorisation;

    if( Params.params.contains( "Authorization" ) )
        authorisation = "Authorization: " + Params.params[ "Authorization" ];

    if( ! IsOpen() )
        OpenConnection();

    if( IsOpen() )
    {
        QString request = data;
        QString wrap = QString( "%1 HTTP/1.1\r\nConnection: close\r\n"
                                "Host: %2\r\n%3\r\n\r\n")
                .arg( QString( request ) )
                .arg( Params.params[ "Host" ] )
                .arg( authorisation );

        qDebug().noquote() << wrap.toLocal8Bit();
        auto result( CTcpClientTransport::SendRequest( wrap.toLocal8Bit() ) );
        return result;//CTcpClientTransport::SendRequest( wrap.toLocal8Bit() );
    }
    else
    {
        qDebug().noquote() << Q_FUNC_INFO << ErrorString();
        Log( "Open HTTP error: " + ErrorString() );
        return REQUEST_TRANSPORT_UNAVAILABLE;
    }
}
