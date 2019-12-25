// Description: Транспорт TCP-клиент
#ifndef TCPCLIENTTRANSPORT_H
#define TCPCLIENTTRANSPORT_H

#include "Transport.h"
#include "TransportTransform.h"

#include <QAbstractSocket>

class QTcpSocket;
class CTcpClientTransport :
    public                                  TransportTransform
{
    Q_OBJECT
public:
    CTcpClientTransport( const QHash< QString, QString >& params,
                         Transform * transform = nullptr,
                         QObject * parent = nullptr );
    CTcpClientTransport( const QHash< QString, QString > & params,
                         const QList< Replacement >& replacements,
                         QObject * parent = nullptr );

    virtual bool                            IsOpen() const;
    bool hasError() const override
    { return hasError(false); }
    bool hasError(bool checkForTimeout) const;
    int errorCode() const override;
    QString errorString() const override;
    QString getErrorName(int errCode) const override;
    QString                         ErrorString() const;

signals:
    void                                    socketConnected();
    void                                    socketDisconnected();
    void                                    stateChanged( QAbstractSocket::SocketState );
protected:
    bool checkTimeout(QAbstractSocket::SocketState state) const;
    void                                    Init();
    bool                                    _OpenConnection( QHash< QString, QString > params ) override;
    void                                    _CloseConnection() override;

    void                                    timerEvent(QTimerEvent *);

public slots:
    void                                    ConnectionStateChanged( QAbstractSocket::SocketState state );
    TransportRequestStatus                  SendRequest( const QByteArray data ) override;
private slots:
    void                                    onError( QAbstractSocket::SocketError );
private:
    void                                    KillReconnectTimer();
    void                                    socketErrorOut( QTcpSocket * socket );
    void                                    connectTo( const QHash< QString, QString > &params, QTcpSocket * socket );
    bool                                    tryToOpen( QHash< QString, QString > params );
    void                                    connectSlots();
    QTcpSocket * TcpSocket() const;
private:
    bool                                    m_bReconnect;
    bool                                    isConnected                 = false;
    mutable QTime                           timeout;
    int                                     timeoutValue                = 0;
    bool                                    opening                     = false;

    int                                     reconnectTimer              = 0;
};

#endif // TCPCLIENTTRANSPORT_H
