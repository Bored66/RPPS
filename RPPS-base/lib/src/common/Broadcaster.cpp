// Description: Модуль оповещения
#include "Broadcaster.h"

Broadcast *                                 Broadcast::pBroadcast = nullptr;

void                                        Broadcast::Log( const QString & msg )
{
    emit Instance()->log( msg );
}

void                                        Broadcast::Warning( const QString & msg )
{
    emit Instance()->warning( msg );
}

void                                        Broadcast::Error( const QString & msg )
{
    emit Instance()->error( msg );
}

Broadcast *                                 Broadcast::Instance()
{
    if( !pBroadcast )
    {
        pBroadcast = new Broadcast();
        connect( pBroadcast, SIGNAL( log(QString) ), pBroadcast, SIGNAL( anyReceive(QString) ) );
        connect( pBroadcast, SIGNAL( warning(QString) ), pBroadcast, SIGNAL( anyReceive(QString) ) );
        connect( pBroadcast, SIGNAL( error(QString) ), pBroadcast, SIGNAL( anyReceive(QString) ) );
    }

    return pBroadcast;
}

bool                                        Broadcast::ConnectToAny( QObject * receiver, const QString & slot )
{
    return Connect( SIGNAL( anyReceive(QString) ), receiver, slot );
}

bool                                        Broadcast::ConnectToLogs( QObject * receiver, const QString & slot )
{
    return Connect( SIGNAL( log(QString) ), receiver, slot );
}

bool                                        Broadcast::ConnectToWarnings( QObject * receiver, const QString & slot )
{
    return Connect( SIGNAL( warning(QString) ), receiver, slot );
}

bool                                        Broadcast::ConnectToErrors(QObject * receiver, const QString & slot)
{
    return Connect( SIGNAL( error(QString) ), receiver, slot );
}

bool                                        Broadcast::BroadcastLogs( QObject * sender, const QString & signal )
{
    return Connect( sender, signal, SIGNAL( log(QString) ) );
}

bool                                        Broadcast::BroadcastWarnings( QObject * sender, const QString & signal )
{
    return Connect( sender, signal, SIGNAL( warning(QString) ) );
}

bool                                        Broadcast::BroadcastErrors( QObject * sender, const QString & signal )
{
    return Connect( sender, signal, SIGNAL( error(QString) ) );
}

bool                                        Broadcast::Connect( const QString & signal, QObject * receiver, const QString & slot )
{
    return connect
            (
                Instance(),
                QMetaObject::normalizedSignature( signal.toLatin1() ),
                receiver,
                QMetaObject::normalizedSignature( slot.toLatin1() ), Qt::QueuedConnection
                );
}

bool                                        Broadcast::Connect( QObject * sender, const QString & signal, const QString & to )
{
    return connect
            (
                sender,
                QMetaObject::normalizedSignature( signal.toLatin1() ),
                Instance(),
                QMetaObject::normalizedSignature( to.toLatin1() ), Qt::QueuedConnection
                );
}
