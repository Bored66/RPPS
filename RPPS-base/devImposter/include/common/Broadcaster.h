// Description: Модуль оповещения
#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QObject>

class Broadcast :
    public                                  QObject
{
    Q_OBJECT
public:
    static void                             Log( const QString & msg );
    static void                             Warning( const QString & msg );
    static void                             Error( const QString & msg );

    static Broadcast *                      Instance();

    static bool                             ConnectToAny( QObject * receiver, const QString & slot );
    static bool                             ConnectToLogs( QObject * receiver, const QString & slot );
    static bool                             ConnectToWarnings( QObject * receiver, const QString & slot );
    static bool                             ConnectToErrors( QObject * receiver, const QString & slot );

    static bool                             BroadcastLogs( QObject * sender, const QString & signal );
    static bool                             BroadcastWarnings( QObject * sender, const QString & signal );
    static bool                             BroadcastErrors( QObject * sender, const QString & signal );
signals:
    void                                    anyReceive( const QString & msg );
    void                                    log( const QString & msg );
    void                                    warning( const QString & msg );
    void                                    error( const QString & msg );
private:
    static bool                             Connect( const QString & signal, QObject * receiver, const QString & slot );
    static bool                             Connect( QObject * sender, const QString & signal, const QString & to );
private:
    static Broadcast *                      pBroadcast;
};

#endif //BROADCASTER_H
