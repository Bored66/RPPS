// Description: Описание XML структур
#ifndef XMLTAGS_H
#define XMLTAGS_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>

#include "OSULiterals.h"

namespace Generate
{

//------------------------------------------------------------------------------
CSTR(XML_HEADER,            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
CSTR(XML_COMMENT,           "<!-- %1 -->")

CSTR(XML_NAME_BEGIN,        "<%1>")                                             // for example, "<OES_SORT_Port>"
CSTR(XML_NAME_END,          "</%1>")                                            // for example, "</OES_SORT_Port>"
CSTR(XML_TAG_TYPE,          " type=\"%1\"")                                     // for example,  type="bit"
CSTR(XML_TAG_SIZE ,         " size=\"%1\"")                                     // for example,  size="2"
CSTR(XML_TAG_TRANSFORM,     " transform=\"%1\"")                                // for example, " transform="OES_ByteStuffing"" => "<Format name="OES_SORT_Port" transform="OES_ByteStuffing" defaultByteValue="0xA5">"
CSTR(XML_TAG_DEFVAL,        " defaultByteValue=\"%1\"")                         // for example, " defaultByteValue="0xA5"" => "<Format name="OES_SORT_Port" defaultByteValue="0xA5">"
CSTR(XML_TAG_REF,           "#%1")                                              // for example, <DATA1>#DATA<DATA1/>
CSTR(XML_TAG_TEMPL,         " template=\"%1\"")                                 //for example, " template="CMD"" => <MSG name="PANEL_FRONT_COM_SET_LIGHT" template="CMD">
CSTR(XML_TAG_REACT,         " reaction=\"%1\"")                                 // for example, " reaction="REACT_FLASH""
CSTR(XML_TAG_FROM,          " from=\"%1\"")                                     // for example, " from="objectAddress""
CSTR(XML_TAG_RANGE,         " range=\"%1\"")                                    // for example, " range="0:3,8, 30""
CSTR(XML_TAG_VALUE,         " value=\"%1\"")                                    // for example, "<option value="0x80" prob=".8">ERROR</option>
CSTR(XML_TAG_PROB,          " prob=\"%s\"")
CSTR(XML_TAG_OPTION,        "<option value=\"%1\" %2>%3</option>")// %1 == XML_TAG_VALUE, %2 == XML_TAG_PROB %3 == body

CSTR(XML_TRANSFORM_BEGIN,   "<Transform name=\"%1\">")                          // for example, "<Transform name="OES_ByteStuffing">"
CSTR(XML_TRANSFORM_REPL,    "\t<replace from=\"%1\" to=\"%2\"/>")               // for example, "<replace from="0xAA" to="0xBB00"/>"
CSTR(XML_TRANSFORM_END,     "</Transform>")

CSTR(XML_FORMAT_BEGIN,      "<Format name=\"%1\"%2%3>")                         // for example, "<Format name="OES_SORT_Port"%2%3>"
CSTR(XML_FORMAT_END,        "</Format>")

CSTR(XML_CONST_SIMPLE,      "<ConstDef name=\"%1\">%2</ConstDef>")              // for example, <ConstDef name="PACKET_START_MARKER">0xAA</ConstDef>
CSTR(XML_CONST_BEGIN,       "<ConstDef name=\"%1\">")                           // for example, <ConstDef name="DATA_SIZE">
CSTR(XML_CONST_SIZEOF,      "\t<SizeOf field=\"%1\"/>")                         // for example, <SizeOf field="DATA"/>
CSTR(XML_CONST_END,         "</ConstDef>")

CSTR(XML_TYPE_BEGIN,        "<TypeDef name=\"%1\">")                            // for example, <TypeDef name="DATA_MEASURE">
CSTR(XML_TYPE_FIELD,        "\t<%1%2%3/>")                                      // for example,  <LIGHT_SOUND type="bit" size="2"/>
CSTR(XML_TYPE_END,          "</TypeDef>")

CSTR(XML_MSG_BEGIN,         "<MSG name=\"%1\"%2>")                              //for example, <MSG name="PANEL_FRONT_COM_SET_LIGHT"%2> => <MSG name="PANEL_FRONT_COM_SET_LIGHT" template="CMD">
CSTR(XML_MSG_END,           "</MSG>")

//TODO: Разобраться со всеми типами CMsgFields
CSTR(XML_MSGFLD_SIMPLE,      "\t<%1/>")                                         //for example, <Status/>
CSTR(XML_MSGFLD_VAL,         "\t<%1>%2</%1>")                                   //for example, <SensorNum>#SENSOR_NUM_MDT</SensorNum> <SensorNum>7</SensorNum>
//CSTR(XML_MSGFLD,            "" },

CSTR(XML_CMD,               "<%1 format=\"%2\">")                    //for example, <SortPort format="OES_SORT_Port">
CSTR(XML_REACT_BEGIN,       "<REACTION name=\"%1\">")
CSTR(XML_REACT_END,         "</REACTION>")
CSTR(XML_RET_BEGIN,         "<RET%1%2>%3</RET>") // %1 == XML_TAG_TYPE %2 == XML_TAG_REACT %3 == body
CSTR(XML_RET_END,           "</RET") // %3 == XML_TAG_TEMPL %4 == body
CSTR(XML_REQ_BEGIN,         "<REQ name =\"%1\" type=\"%2\"%3>%4") // %3 == XML_TAG_TEMPL %4 == body
CSTR(XML_REQ_END,           "</REQ") // %3 == XML_TAG_TEMPL %4 == body


//------------------------------------------------------------------------------
inline QStringList
PrepareXML( const QStringList& PBody )
{
    return
            QStringList()
            << XML_HEADER
            << PBody ; }

//------------------------------------------------------------------------------
inline QString
PrepareXMLComment( const QString& PComment )
{
    return XML_COMMENT.arg(PComment);
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLName( const QString& PName, const QStringList& PBody )
{
    return
            QStringList()
            << XML_NAME_BEGIN.arg(PName)
            << PBody
            << XML_NAME_END.arg(PName) ;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLTransform( const QString& PName,
                     const QList<QPair<String, QString> >& PPairs )
{
    QStringList _retval;
    for( auto _p : PPairs )
        _retval.append(XML_TRANSFORM_REPL.arg( _p.first).arg(_p.second));
    return
            QStringList()
            << XML_TRANSFORM_BEGIN.arg(PName)
            << _retval
            << XML_TRANSFORM_END.arg(PName) ;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLFormat( const QString& PName, const QStringList& PBody,
                  const QString& PTransformName = QString::null,
                  const QString& PDefVal = QString::null)
{
    QString _tr = PTransformName.isEmpty() ?
                      QString() :
                      XML_TAG_TRANSFORM.arg(PTransformName);
    QString _val = PDefVal.isEmpty() ?
                       QString() :
                       XML_TAG_DEFVAL.arg(PDefVal);
    return
            QStringList()
            << XML_FORMAT_BEGIN.arg(PName).arg(_tr).arg(_val)
            << PBody
            << XML_FORMAT_END;
}


//------------------------------------------------------------------------------
template < typename VALUE_TYPE >
Q_INLINE_TEMPLATE QString
PrepareXMLConstSimple( const QString& PName, const VALUE_TYPE& PValue )
{
    return XML_CONST_SIMPLE.arg(PName).arg(PValue);
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLConst( const QString& PName, const QStringList& PBody )
{
    return QStringList()
            << XML_CONST_BEGIN.arg(PName)
            << PBody
            << XML_CONST_END;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLTypedef( const QString& PName, const QStringList& PBody )
{
    return QStringList()
            << XML_TYPE_BEGIN.arg(PName)
            << PBody
            << XML_TYPE_END;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLMsg( const QString& PName, const QString& PTemplate,
               const QStringList& PBody )
{
    QString _tpl = PTemplate.isEmpty() ?
                       QString() :
                       XML_TAG_TEMPL.arg(PTemplate);
    return QStringList()
            << XML_MSG_BEGIN.arg(PName).arg(_tpl)
            << PBody
            << XML_MSG_END;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline QString
PrepareXMLOption( const QString& PValue, const double PProbability, bool PWithProb = false)
{
    return PValue.isEmpty() ?
                "" :
                XML_TAG_OPTION.arg(PValue).arg( PWithProb ? QString::number(PProbability,3,0) : "");
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLCommand( const QString& PName, const QString& PFormat,
               const QStringList& PBody )
{
    return QStringList()
            << XML_CMD.arg(PName).arg(PFormat)
            << PBody
            << XML_NAME_END.arg(PName);
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLReaction( const QString& PName, const QStringList& PBody )
{
    return QStringList()
            << XML_REACT_BEGIN.arg(PName)
            << PBody
            << XML_REACT_END;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLRequest( const QString& PName,
                   const QString& PType,
                   const QString& PTemplate,
                   const QStringList& PBody )
{
    QString _tpl = PTemplate.isEmpty() ?
                       QString() :
                       XML_TAG_TEMPL.arg(PTemplate);
    return QStringList()
            << XML_REQ_BEGIN.arg(PName).arg(PType).arg(_tpl)
            << PBody
            << XML_REQ_END;
}

//------------------------------------------------------------------------------
inline QStringList
PrepareXMLReturn( const QStringList& PBody,
                  const QString& PType = QString::null,
                  const QString& PReact = QString::null )
{
    QString _type = PType.isEmpty() ?
                        QString() :
                        XML_TAG_TYPE.arg(PType);
    QString _react = PReact.isEmpty() ?
                        QString() :
                        XML_TAG_REACT.arg(PReact);
    return QStringList()
            << XML_RET_BEGIN.arg(_type).arg(_react)
            << PBody
            << XML_RET_END;
}

}// namespace Generate

#endif // XMLTAGS_H

