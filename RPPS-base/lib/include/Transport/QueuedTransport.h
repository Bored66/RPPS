// Description: Транспорт с очередью команд
#ifndef QUEUEDTRANSPORT_H
#define QUEUEDTRANSPORT_H

#include "TransportTransform.h"

class QueuedTransport :
    public                                  TransportTransform
{
    Q_OBJECT
public:
    QueuedTransport
    (
        ITransport *                        coreTransport,
        uint                                timerStep
    );
    bool                                    IsOpen() const override;
    bool                                    hasError() const override;
    int errorCode() const override;
    QString errorString() const override;
    QString getErrorName(int errCode) const override;

    void                                    SetParams( const QHash< QString, QString >& params ) override;
    void                                    SetParams( TransportParams& params ) override;
    QString                                 ErrorString() const override;
    TransportRequestStatus                  LastStatus();
    void                                    setReplacements
    (
        const QList< Replacement > &        replacements
    ) override;
public slots:
    void                                    PrependData( const QByteArray data );
    TransportRequestStatus                  SendRequest( const QByteArray data ) override;
    void                                    SetTimerStep( uint value );

    void                                    Pause();
    void                                    Resume();
    void                                    Clear();

protected:
    void                                    timerEvent( QTimerEvent * ) override;
    bool                                    _OpenConnection( QHash< QString, QString > params ) override;
    void                                    _CloseConnection() override;

private:
    QQueue< QByteArray >                    queue;
    ITransport *                            coreTransport               = nullptr;
    int                                     timerId                     = 0;
    TransportRequestStatus                  lastStatus                  = REQUEST_SEND_SUCCESS;
    bool                                    paused                      = false;
};

#endif // QUEUEDTRANSPORT_H
