// Description: Фабрика транспорта
#ifndef TRANSPORTFACTORY_H
#define TRANSPORTFACTORY_H

#include "Transport.h"
#include "QueuedTransport.h"
#include "Transform.h"

class TransportFactory
{
public:
    static
    ITransport *                            Create( const QString & filename,
                                                    QObject * parent = nullptr );
    static
    ITransport *                            Create
    (
            const QString & filename,
            const QList<Replacement> & replacements,
            QObject* parent = nullptr
    );
    static
    ITransport *                            Create
    (
            const TransportParams & params,
            QObject * parent = nullptr
    );
    static
    ITransport *                            Create
    (
            const TransportParams & params,
            const QList< Replacement > & replacements,
            QObject * parent = nullptr
    );

    static QueuedTransport *                MakeQueued
    (
        ITransport *                        coreTransport,
        uint                                timerStep                   = 100
    );
};

#endif // TRANSPORTFACTORY_H
