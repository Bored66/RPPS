// Description: Транспорт для RS-232
#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H

#include <QtSerialPort/QSerialPort>
#include <QtGlobal>
#include <QVector>

#include "Transport.h"
#include "TransportTransform.h"


class CSerialTransport :
    public                                  TransportTransform
{
    Q_OBJECT
public:
        CSerialTransport( QHash< QString, QString > params,
                          Transform* transform = nullptr,
                          QObject * parent = nullptr );

        CSerialTransport( QHash< QString, QString > params,
                          const QList< Replacement >& replacements,
                          QObject * parent = nullptr );

    bool                                    IsClearToSend();

    static
    QSerialPort::BaudRate                   ToBaudRate( const QString & rate );
    static
    QSerialPort::DataBits                   ToDataBits( const QString & dataBit );
    static
    QSerialPort::Parity                     ToParity( const QString & parity );
    static
    QSerialPort::StopBits                   ToStopBits( const QString & stopBit );
    static
    QSerialPort::FlowControl                ToFlowControl( const QString & control );
    virtual bool hasError() const override;
    int errorCode() const override;
    QString errorString() const override;
    QString getErrorName(int errCode) const override;
protected:
    void                                    Init();
    virtual bool                            _OpenConnection( QHash< QString, QString > params );
protected slots:
    void                                    OnError(QSerialPort::SerialPortError serialPortError);
private:
    QSerialPort *                           SerialPort() const;
    QSerialPort _port;
    bool                                    ToDtr( const QString & dtr );
    bool                                    ToRts( const QString & rts );
    static
    const QVector< QPair< QString, QSerialPort::BaudRate > > rates;
    static
    const QVector< QPair< QString, QSerialPort::DataBits > > dataBits;
    static
    const QVector< QPair< QString, QSerialPort::Parity > > parities;
    static
    const QVector< QPair< QString, QSerialPort::StopBits > > stopBits;
    static
    const QVector< QPair< QString, QSerialPort::FlowControl > > controls;
};

#endif // SERIALTRANSPORT_H
