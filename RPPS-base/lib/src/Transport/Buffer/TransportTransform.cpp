// Description: Транспорт поддерживающий преобразование байт
#include "TransportTransform.h"

TransportTransform::TransportTransform
(
    TransportParams params,
    Transform* transform,
    QObject* parent
)
    : ITransport( params, parent),
      mTransform( transform )
{
    Init();
}

TransportTransform::TransportTransform
(
    TransportParams params,
    const QList<Replacement>& replacements,
    QObject* parent
)
    : ITransport( params, parent )
{
    setReplacements(replacements);
    Params.type = params.GetType().trimmed();
    Init();
}

QList< Replacement > TransportTransform::Replacements() const
{
    if( mTransform )
        return mTransform->Replacements();
    return {};
}

TransportRequestStatus TransportTransform::SendRequest( const QByteArray data )
{
    if( Replacements().isEmpty() == false )
    {
        auto stuffed = mTransform->Stuff( data );
        return SendTransformedRequest(stuffed);
    }
    else
        return SendTransformedRequest( data );
}
TransportRequestStatus TransportTransform::SendTransformedRequest(const QByteArray data)
{
    return ITransport::SendRequest( data );
}

void TransportTransform::setReplacements(const QList<Replacement>& replacements)
{
  if( ! mTransform)
    mTransform = new Transform( this );

  mTransform->SetReplacements( replacements );
  mTransform->setDebugResult(debugParam("STUFFUNSTUFF"));
}

void TransportTransform::DataReceived( const QByteArray & data )
{
    debugOut(data, "RCVD");

    mData.append( data );

    if( Replacements().isEmpty() == false )
    {
        int untrustedBytes = 0;
        QByteArray unstuffed = mTransform->Unstuff( mData, untrustedBytes );

        int receivedCount = unstuffed.length() - untrustedBytes;
        if (receivedCount > 0) {
            emit dataReceived( unstuffed.left( receivedCount ) );
        }
        if (untrustedBytes) {
            mData = mData.right( untrustedBytes );
        } else {
            mData.clear();
        }
    }
    else
    {
        emit dataReceived( data );
        mData.clear();
    }
}

