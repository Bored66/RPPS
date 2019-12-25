// Description: Сохранение настроек транспорта
#include "TransportWriter.h"

#include <QFile>
#include <QTextStream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomText>

TransportWriter::TransportWriter()
{}

void                                        TransportWriter::Write
(
    const QString &                         filename,
    const TransportParams &                 params
)
{
    QDomDocument result( "Transport" );
    auto root = result.createElement( "OESTransport" );
    root.setAttribute( "default", params.GetType() );
    auto transport = result.createElement( "Transport" );
    transport.setAttribute( "name", params.GetType() );
    transport.setAttribute( "type", params.GetType() );

    for( auto key : params.params.keys() )
    {
        auto value = params.params[ key ];
        auto node = result.createElement( key );
        auto text = result.createTextNode( value );
        node.appendChild( text );

        transport.appendChild( node );
    }

    root.appendChild( transport );
    result.appendChild( root );

    QFile out( filename );
    if( out.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QTextStream stream( &out );
        stream << result.toString();

        out.close();
    }

}
