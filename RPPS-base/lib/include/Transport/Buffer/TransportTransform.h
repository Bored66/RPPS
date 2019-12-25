// Description: Транспорт поддерживающий преобразование байт
#ifndef TransportTransform_H
#define TransportTransform_H

#include <QObject>
#include <QPointer>
#include "Transport.h"
#include "Transform.h"

class TransportTransform :
    public                                  ITransport
{
    Q_OBJECT
public:
    explicit TransportTransform( TransportParams params,
                                 Transform * transform = nullptr,
                                 QObject * parent = nullptr );
    explicit TransportTransform( TransportParams params,
                                 const QList< Replacement >& replacements,
                                 QObject * parent = nullptr );

    bool  TransformExists() const;

    QList< Replacement > Replacements() const;
public slots:
    virtual TransportRequestStatus  SendRequest( const QByteArray data );
    virtual void setReplacements( const QList< Replacement >& replacements );

protected:
    virtual void Init(){}
    virtual void DataReceived( const QByteArray & data );
private:
    virtual TransportRequestStatus SendTransformedRequest( const QByteArray data );

protected:
    QPointer<Transform> mTransform;
private:
    QByteArray          mData;
};

inline bool
TransportTransform::TransformExists() const
{ return mTransform; }


#endif //TransportTransform_H
