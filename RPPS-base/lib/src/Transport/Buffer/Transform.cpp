// Description: Преобразование байт
#include "Transform.h"

#include <QDebug>
#include <QProcessEnvironment>

#include "CommonUtils.h"

Transform::Transform( QObject * parent ) :
    QObject( parent )
{}

void                                        Transform::SetReplacements( const QList< Replacement >& replacements )
{
    mReplacements  = replacements;
}

QByteArray                                  Transform::Stuff( const QByteArray & data )
{
    if( mReplacements.isEmpty() )
        return data;
    bool append = true;
    QByteArray result;
    for( int i = 0; i < data.size(); ++i )
    {
        append = true;

        for( Replacement replace : mReplacements )
        {
            if( (i >= replace.offsetLeft) && (i < data.size() - replace.offsetRight) )
            {
               if( data.at(i) == replace.bytesFrom.at(0) )
               {
                    result.append(replace.bytesTo);
                    append = false;
                    #ifdef QT_DEBUG
                    DebugOut( "Stuff: %1 => %2", { replace.bytesFrom, replace.bytesTo },
                              data, result, i );
                    #endif
                    break;
               }
            }
        }
        if (append)
            result.append(data.at(i));
    }
    return result;
}
void Transform::DebugOut
(
        const QString & type,
        QPair< QByteArray, QByteArray > replacement,
        const QByteArray data,
        QByteArray result,
        int minIndex
)
{
    if ((QProcessEnvironment::systemEnvironment()
            .contains("STUFFUNSTUFF_DEBUG") &&
            str2bool(QProcessEnvironment::systemEnvironment()
                     .value("STUFFUNSTUFF_DEBUG"))) ||
         ( debugResult ))
    {
        qWarning().noquote() << QString(type)
                    .arg( (QString) replacement.first.toHex() )
                    .arg( (QString) replacement.second.toHex() );
        qDebug().noquote() << data.left( minIndex  + replacement.second.length() ).toHex()
                 <<" transformed to";
        qDebug().noquote() << result.toHex();
    }
}
QByteArray                                  Transform::Unstuff( const QByteArray & data , int & untrusted )
{
    if( mReplacements.isEmpty() )
        return data;

    QByteArray result;
    QByteArray subData  ( data );
    result.clear();

    while( subData.length() )
    {
        int minIndex = subData.length();
        QPair< QByteArray, QByteArray > replacement = ReplacementLookup(subData, minIndex);
        if( minIndex < subData.length() )
        {
            AddReplacementToResult(result, minIndex, replacement, subData);
            #ifdef QT_DEBUG
            DebugOut( "Unstuff: %2 => %1", replacement, data, result, minIndex);
            #endif
        }
        else
        {
            result.append( subData );
            subData.clear();
        }
    }
    untrusted = UntrustedBytesCount( data );

    return result;
}

QPair<QByteArray, QByteArray> Transform::ReplacementLookup(QByteArray subData, int & minIndex)
{
    QPair< QByteArray, QByteArray > replacement;
    for( Replacement replace : mReplacements )
    {
        int index = subData.indexOf( replace.bytesTo );
        if( index > -1 )
        {
            if( index < minIndex )
            {
                minIndex = index;
                replacement = { replace.bytesFrom, replace.bytesTo };
            }
        }
    }
    return replacement;
}
void Transform::AddReplacementToResult(QByteArray & result, int minIndex, QPair< QByteArray, QByteArray > replacement, QByteArray &subData)
{
    result.append( subData.data(), minIndex );
    result.append( replacement.first );
    subData.remove( 0, minIndex + replacement.second.length() );
}

int Transform::UntrustedBytesCount( const QByteArray & data )
{
    for( auto replace : mReplacements )
    {
        auto bytes = replace.bytesFrom;
        if( bytes == data.right( bytes.length() ) )
            return bytes.length();
    }
    return 0;
}

void Transform::setDebugResult(bool value)
{
    debugResult = value;
}

QList< Replacement >                        Transform::Replacements() const
{
    return mReplacements;
}
