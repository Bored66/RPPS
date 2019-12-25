// Description: Чтение настроек транспорта из файла
#include "TransportReader.h"

#include <QDomDocument>
CTransportReader::CTransportReader( QObject * parent ) :
    CXmlReader( parent )
{
    SetTokenTree( strTransportTokenTree );

}

TransportParams                             CTransportReader::ParseTransport( const QString & xml )
{
    TransportParams result;

    QDomDocument dom = GetDomFromText( xml );
    QString transportName = QString();
    if( !dom.isNull() )
    {
        QDomElement root = dom.documentElement();
        CheckBranch( m_tokenTree, root );

        transportName = root.attribute( "default" );

        QDomNode child = root.firstChild();
        while( !child.isNull() )
        {
            if( child.toElement().attribute( "name" ) == transportName )
            {
                result.type = child.toElement().attribute( "type" );
                result.name = transportName;

                QDomNode param = child.firstChild();
                while( !param.isNull() )
                {
                    QDomElement elem = param.toElement();
                    result.params[ elem.tagName() ] = elem.text().simplified();

                    param = param.nextSibling();
                }
                break;
            }
            child = child.nextSibling();
        }
    }

    if( !result.params.count() )
    {
        Error( "There are no settings for transport named " + transportName );
    }

    return result;
}
