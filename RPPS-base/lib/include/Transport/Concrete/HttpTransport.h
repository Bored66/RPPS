// Description: Транспорт протокола HTTP
#ifndef HTTPTRANSPORT_H
#define HTTPTRANSPORT_H

#include <QObject>

#include "TcpClientTransport.h"

class CHttpTransport :
    public                                  CTcpClientTransport
{
    Q_OBJECT
public:
        CHttpTransport( QHash< QString, QString > params,
                        Transform*                 transform = nullptr,
                        QObject * parent = nullptr );

        CHttpTransport( QHash< QString, QString > params,
                        QList< Replacement >       replacements,
                        QObject * parent = nullptr );

protected:
    void                                    Init();
    TransportRequestStatus                  SendRequest( const QByteArray data );

};

#endif // HTTPTRANSPORT_H
