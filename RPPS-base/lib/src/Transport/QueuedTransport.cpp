// Description: Транспорт с очередью команд
#include "QueuedTransport.h"

QueuedTransport::QueuedTransport( ITransport * coreTransport, uint timerStep ) :
    TransportTransform
    ( { coreTransport->Type(),
        coreTransport->GetParams()
      },
      nullptr,
      coreTransport->parent()
    ),
    coreTransport( coreTransport )
{
    SetTimerStep( timerStep );

    if( auto transformTransport = dynamic_cast< TransportTransform * >( coreTransport ) )
        setReplacements( transformTransport->Replacements() );

    connect( coreTransport, SIGNAL( dataSent( QByteArray ) ), SIGNAL( dataSent( QByteArray ) ) );
    connect( coreTransport, SIGNAL( dataReceived( QByteArray ) ), SIGNAL( dataReceived( QByteArray ) ) );

    connect( coreTransport, SIGNAL( connected() ), SIGNAL( connected() ));
    connect( coreTransport, SIGNAL( disconnected() ), SIGNAL( disconnected() ));
}

bool                                        QueuedTransport::IsOpen() const
{
    return coreTransport->IsOpen();
}

TransportRequestStatus                      QueuedTransport::SendRequest( const QByteArray data )
{
    queue.append( data );

    return lastStatus;
}

void                                        QueuedTransport::SetTimerStep( uint value )
{
    if( timerId )
    {
        killTimer( timerId );
        timerId = 0;
    }

    if( value )
    {
        timerId = startTimer( value );
    }
}

void                                        QueuedTransport::Pause()
{
    paused = true;
}

void                                        QueuedTransport::Resume()
{
    paused = false;
}

void                                        QueuedTransport::Clear()
{
    queue.clear();
}

void                                        QueuedTransport::timerEvent( QTimerEvent * )
{
    if( !paused && queue.count() )
    {
        lastStatus = coreTransport->SendRequest( queue.takeFirst() );
    }
}

bool                                        QueuedTransport::_OpenConnection( QHash< QString, QString > params )
{
    return coreTransport->OpenConnection( params );
}

void                                        QueuedTransport::_CloseConnection()
{
    coreTransport->CloseConnection();
}

bool                                        QueuedTransport::hasError() const
{
    return coreTransport->hasError();
}

int QueuedTransport::errorCode() const
{
   return coreTransport->errorCode();
}

QString QueuedTransport::errorString() const
{
    return coreTransport->errorString();
}

QString QueuedTransport::getErrorName(int errCode) const
{
    return coreTransport->getErrorName(errCode);
}

void                                        QueuedTransport::SetParams( const QHash< QString, QString > & params )
{
    coreTransport->SetParams( params );
}

void                                        QueuedTransport::SetParams( TransportParams & params )
{
    coreTransport->SetParams( params );
}

QString                                     QueuedTransport::ErrorString() const
{
    return coreTransport->ErrorString();
}

TransportRequestStatus                      QueuedTransport::LastStatus()
{
    return lastStatus;
}

void                                        QueuedTransport::setReplacements
(
    const QList< Replacement > &            replacements
)
{
    if( coreTransport )
        coreTransport->setReplacements( replacements );

    if( !mTransform )
        mTransform = new Transform( this );

    mTransform->SetReplacements( replacements );
}

void                                        QueuedTransport::PrependData( const QByteArray data )
{
    queue.prepend( data );
}
