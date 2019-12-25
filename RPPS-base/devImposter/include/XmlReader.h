// Description: Основной класс чтения XML
#ifndef XMLREADER_H
#define XMLREADER_H

#include <QObject>
#include <QHash>
#include <QXmlStreamReader>
#include <QStringList>

class QDomDocument;
class QDomNode;
class QDomElement;

//------------------------------------------------------------------------------
class CToken
{
public:
    QString name;
    QList< QString > attributes;
    QList< CToken > children;
};
/*
    "..."                                   //любая ветка
    "Transform", { "name!" }                //конкретная ветка, обязательный атрибут
    "replace", { "from", "to" }             //возможные атрибуты
    "#"                                     //возможно текстовое значение
    "#!"                                     //текстовое значение обязательно
*/

class CXmlReader :
    public                                  QObject
{
    Q_OBJECT
public:
    explicit CXmlReader( QObject * parent = nullptr );
    virtual ~CXmlReader(){}

    static QString                          ToCamelCase( const QString & name, bool setFirstCapital = true );
    static QString                          FromCamelCase( const QString & name );
    static QString                          ReadFile(const QString & filename , QString &desc);
    static QStringList                      GetComments( QDomNode & node );
    QDomNode                                TakeComments( QDomNode & node );
    QDomNode                                SkipComments( QDomNode & node );
    void                                    SetMeta( QObject * object, QDomNode & node );
    uint                                    ErrorCount()
    { return errorCount; }

    QString                                 Filename() const;
    void                                    SetFilename( const QString & value );

    QDomDocument                            GetDomFromText( const QString & xml );
protected:
    void                                    CheckBranch( CToken tokenBranch, QDomNode domBranch );
    QStringList                             CollectAttributes( QDomNode & domBranch );
    QStringList                             Attributes( CToken tokenBranch );
    QStringList                             Mandatory( CToken tokenBranch );
    QStringList                             Children( CToken tokenBranch );
    CToken                                  ChildByName( CToken tokenBranch, const QString & name );
    QString                                 GetAttributeValue
    (
        const QDomNode &                    node,
        const QString &                     name,
        const QString &                     defaultValue                = QString()
    ) const;
    QString                                 GetAttributeValue
    (
        const QDomElement &                 elem,
        const QString &                     name,
        const QString &                     defaultValue                = QString()
    ) const;
    CToken                                  TakeToken( QStringList & tokens, const QString & prefix = QString() );
    void                                    SetTokenTree( const QString & strTree );
signals:
    void                                    log( const QString & msg );
    void                                    error( const QString & msg );
public slots:
    void                                    Log( const QString & msg );
    void                                    Error( const QString & msg );
protected:
    CToken                                  m_tokenTree;
    uint                                    errorCount;
    QString                                 filename;
    QStringList                             comments;
};

#endif // XMLREADER_H
