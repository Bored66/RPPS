// Description: Основной класс чтения XML
#include "XmlReader.h"

#include <QDomDocument>
#include <QStringList>
#include <QFile>
#include <QTextStream>

CXmlReader::CXmlReader( QObject * parent ) :
    QObject( parent ),
    m_tokenTree( { "..." } ),
    errorCount( 0 )
{}

QString                                     CXmlReader::ToCamelCase( const QString & name, bool setFirstCapital )
{
    if( name.isEmpty() )
        return QString();

    QString result;
    bool isFirst = true;

    QStringList syllables = name.split( "_" );
    for( QString syllable : syllables )
    {
        if( syllable.length() )
        {
            QString first = syllable.left( 1 );
            if( isFirst )
            {
                isFirst = false;
                if( setFirstCapital )
                    first = first.toUpper();
                else
                    first = first.toLower();
            }
            else
                first = first.toUpper();

            result.append( first );
            result.append( syllable.mid( 1 ).toLower() );
        }
    }

    return result;
}

QString CXmlReader::ReadFile( const QString & filename, QString & desc )
{
    QString xmlData;
    QFile file( filename );
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream readStream( &file );

        xmlData = readStream.readAll();
        file.close();
    }
    else
    {
        desc = QString("Error opening file: %1")
              .arg(filename);
        return  QString{};
    }
    return xmlData;
}

QStringList                                 CXmlReader::GetComments( QDomNode & node )
{
    QStringList comments;

    while( !node.isNull() && node.isComment() )
    {
        comments << node.nodeValue();
        node = node.nextSibling();
    }

    return comments;
}

QDomNode                                    CXmlReader::TakeComments( QDomNode & node )
{
    comments = GetComments( node );

    return node;
}

QDomNode                                    CXmlReader::SkipComments(QDomNode & node)
{
    GetComments( node );

    return node;
}

void                                        CXmlReader::SetMeta( QObject * object , QDomNode & node )
{
    if( object )
    {
        object->setProperty( "FileName", filename );
        object->setProperty( "LineNumber", node.lineNumber() );
        object->setProperty( "Comments", comments );

        comments.clear();
    }
}

QString CXmlReader::Filename() const
{
    return filename;
}

void CXmlReader::SetFilename( const QString & value )
{
    filename = value;
}

void RemoveComments( QDomNode & node )
{
    QDomNode child = node.firstChild();
    while( !child.isNull() )
    {
        if( child.isComment() )
        {
            node.removeChild( child );
        }
        if( child.hasChildNodes() )
        {
            RemoveComments( child );
        }

        child = child.nextSibling();
    }
}

QDomDocument CXmlReader::GetDomFromText( const QString & xml )
{
    QDomDocument dom;

    QString errorMsg;
    int line;
    int column;

    if( !dom.setContent( xml, &errorMsg, &line, &column ) )
    {
        Error( "Error parsing xml" +
        errorMsg +
        QString( "At line %1, column %2: %3" )
               .arg( line )
               .arg( column )
               .arg( xml.split( "\n" ).at( line - 1 ) ) );
        errorCount++;
    }
//    else
//    {
//        RemoveComments( dom );
//    }

    return dom;
}

void CXmlReader::CheckBranch( CToken tokenBranch, QDomNode domBranch )
{
    QDomElement elem = domBranch.toElement();
    QStringList exists = CollectAttributes( domBranch );
    QStringList possible = Attributes( tokenBranch );
    QStringList mandatory = Mandatory( tokenBranch );

    for( QString attr : mandatory )
        if( !exists.contains( attr ) )
            Error( QString( "Attribute '%1' for tag '%2' not found; Line: %3" )
                   .arg( attr )
                   .arg( elem.tagName() )
                   .arg( elem.lineNumber() ) );

    for( QString attr : exists )
        if( !possible.contains( attr ) )
            Error( QString( "Invalid attribute name '%1'' for tag '%2'; Line: %3" )
                   .arg( attr )
                   .arg( elem.tagName() )
                   .arg( elem.lineNumber() ) );

    QStringList children = Children( tokenBranch );
    if( children.contains( "#!" ) )
    {
        if( elem.text().isEmpty() )
            Error( QString( "Value for tag '%1' is empty; Line: %2" )
                   .arg( elem.tagName() )
                   .arg( elem.lineNumber() ) );
    }
    else if( !children.contains( "#" ) )
        if( !elem.text().isEmpty() )
        {
            QDomNodeList nodeList = elem.childNodes();

            for( int i = 0; i < nodeList.size(); ++i )
            {
                QDomNode subElem = nodeList.at( i );
                if( subElem.isText() )
                {
                    Error( QString( "Unexpected value for tag '%1'; Value: '%2'; Line: %3" )
                           .arg( elem.tagName() )
                           .arg( elem.text().simplified() )
                           .arg( elem.lineNumber() ) );
                    break;
                }
            }
        }

    QDomNode child = domBranch.firstChild();

    while( !child.isNull() )
    {
        QString tagName = child.toElement().tagName();

        if( !tagName.isEmpty() )
        {
            if( children.contains( tagName ) || children.contains( "..." ) )
                CheckBranch( ChildByName( tokenBranch, tagName ), child );
            else
                Error( QString( "Unexpected branch name '%1' for tag '%2'; Line: %3" )
                       .arg( tagName )
                       .arg( elem.tagName() )
                       .arg( child.toElement().lineNumber() ) );
        }

        child = child.nextSibling();
    }
}

QStringList CXmlReader::CollectAttributes( QDomNode & domBranch )
{
    QStringList result;

    QDomNamedNodeMap attributes = domBranch.toElement().attributes();

    for( int i = 0; i < attributes.count(); ++i )
        result << attributes.item( i ).toAttr().name();

    return result;

}

QStringList CXmlReader::Attributes( CToken tokenBranch )
{
    QStringList result;

    for( QString attr : tokenBranch.attributes )
        if( attr.endsWith( "!" ) )
            result << attr.left( attr.size() - 1 );
        else
            result << attr;

    return result;
}

QStringList CXmlReader::Mandatory( CToken tokenBranch )
{
    QStringList result;

    for( QString attr : tokenBranch.attributes )
        if( attr.endsWith( "!" ) )
            result << attr.left( attr.size() - 1 );

    return result;
}

QStringList CXmlReader::Children( CToken tokenBranch )
{
    QStringList result;

    for( CToken child : tokenBranch.children )
        result << child.name;

    return result;
}

CToken CXmlReader::ChildByName( CToken tokenBranch, const QString & name )
{
    QString childName = name;

    QStringList children = Children( tokenBranch );
    if( !children.contains( childName ) )
        childName = "...";

    for( CToken child : tokenBranch.children )
        if( child.name == childName )
            return child;

    return CToken();
}

QString CXmlReader::GetAttributeValue
(
    const QDomNode &                        node,
    const QString &                         name,
    const QString &                         defaultValue
) const
{
    return node.toElement().attribute( name, defaultValue ).trimmed();
}

QString                                     CXmlReader::GetAttributeValue
(
    const QDomElement &                     elem,
    const QString &                         name,
    const QString &                         defaultValue
) const
{
    return elem.attribute( name, defaultValue ).trimmed();
}

CToken CXmlReader::TakeToken( QStringList & tokens, const QString & prefix )
{
    auto subPrefix = prefix + "    ";

    auto tokenInfo = tokens.takeFirst()
                    .replace( "(", " " )
                    .replace( ")", " " )
                    .simplified().split( " " );

    auto tokenName = tokenInfo.takeFirst();

    QList< CToken > subTokens;

    while( tokens.count() && tokens.first().startsWith( subPrefix ) )
    {
        subTokens << TakeToken( tokens, subPrefix );
    }

    CToken result = { tokenName, tokenInfo, subTokens };

    return result;
}

void CXmlReader::SetTokenTree( const QString & strTree )
{
    auto lines = strTree.split( "\n" );
    lines.removeAll( QString() );

    if( lines.count() )
    {
        m_tokenTree = TakeToken( lines );
    }
}


void CXmlReader::Log( const QString & msg )
{
    emit log( msg );
}

void CXmlReader::Error( const QString & msg )
{
    emit error( msg );
}
