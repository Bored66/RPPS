// Description: Транспорт протокола UDP
#ifndef UDPTRANSPORT_H
#define UDPTRANSPORT_H

#include "Transport.h"
#include "TransportTransform.h"

#include <QAbstractSocket>
#include <QHostAddress>

class QUdpSocket;

class CUdpTransport :
        public                                  TransportTransform
{
    Q_OBJECT
public:
        CUdpTransport( QHash< QString, QString > params,
                       Transform * transform = nullptr,
                       QObject * parent = nullptr );
        CUdpTransport(QHash< QString, QString > params,
                       const QList<Replacement> &replacements,
                       QObject * parent = nullptr );

        virtual bool                        IsOpen() const;
        bool hasError() const override;
        int errorCode() const override;
        QString errorString() const override;
        QString getErrorName(int errCode) const override;
signals:
        void readDataFrom(QHostAddress hostAddress, quint16 port);
//        QAbstractSocket::SocketState        State();
private:
    QUdpSocket * UdpSocket() const;
protected:
    void                                    Init();
    virtual bool                            _OpenConnection( QHash< QString, QString > params );
    virtual void                            ReadData();
    virtual int                             Write( QByteArray data );
};

#endif // UDPTRANSPORT_H
