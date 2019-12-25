// Description: Транспорт TCP-сервер
#ifndef TCPSERVERTRANSPORT_H
#define TCPSERVERTRANSPORT_H

#include <QAbstractSocket>
#include <QPointer>
#include <QTcpServer>
#include <QTcpSocket>

#include "Transport.h"
#include "TransportTransform.h"

class CTcpServerTransport :
        public                                  TransportTransform
{
    Q_OBJECT
public:
    CTcpServerTransport( QHash< QString, QString >  params,
                         Transform*                 transform = nullptr,
                         QObject*                   parent = nullptr );
    CTcpServerTransport( QHash< QString, QString >  params,
                         QList< Replacement >       replacements,
                         QObject*                   arent = nullptr );

    ~CTcpServerTransport();

    virtual bool                            IsOpen() const;
    void CloseSockets();
    bool hasError() const override;
    int errorCode() const override;
    QString errorString() const override;
    QString getErrorName(int errCode) const override;
    QList< QTcpSocket * > Listeners() const
    {
        return listeners;
    }

public slots:
    void                                    ProcessNewConnection();
    void                                    ConnectionStateChanged( QAbstractSocket::SocketState state );
    TransportRequestStatus                  SendRequest( const QByteArray data ) override;
    void                                    ReadFromSocket();
    void connectionEstablished();
    void reportHost(QAbstractSocket *socket);
    void onError(QAbstractSocket::SocketError);
protected:
    void                                    Init();
    virtual bool                            _OpenConnection( QHash< QString, QString > params ) override;
    virtual void                            _CloseConnection() override;

private:
    QScopedPointer< QTcpServer >            m_pServer;
    QList< QTcpSocket * >                   listeners;
};

#endif // TCPSERVERTRANSPORT_H
