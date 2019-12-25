// Description: Фабрика транспорта
#include "TransportFactory.h"
//#include "DroidTransport.h"
#ifdef EXCLUDE_SERIAL
#include "Concrete/SerialTransport.h"
#endif
//#include "Concrete/TerminalTransport.h"
#include "Concrete/TcpClientTransport.h"
#include "Concrete/TcpServerTransport.h"
#include "Concrete/HttpTransport.h"
#include "Concrete/UdpTransport.h"
#include "TransportReader.h"

ITransport *                                TransportFactory::Create( const QString & filename, QObject * parent )
{
    QString desc,
            xmlData = CXmlReader::ReadFile( filename, desc );
    CTransportReader * reader = new CTransportReader( parent );
    if (xmlData.isEmpty())
    {
        qDebug() << desc;
        return nullptr;
    }
    auto transportParameters = reader->ParseTransport( xmlData );

    return Create( transportParameters, QList<Replacement>(), parent );
}
ITransport *                                TransportFactory::Create(const QString & filename,
                                                                      const QList<Replacement> &replacements,
                                                                      QObject* parent)
{
    QString desc,
            xmlData = CXmlReader::ReadFile( filename, desc );
    CTransportReader * reader = new CTransportReader( parent );
    if (xmlData.isEmpty())
    {
        qDebug() << desc;
        return nullptr;
    }
    auto transportParameters = reader->ParseTransport( xmlData );

    return Create( transportParameters, replacements, parent );
}

ITransport *TransportFactory::Create(const TransportParams& params,
                                      QObject* parent)

{
    return Create(params, QList<Replacement>(), parent );
}

ITransport*
TransportFactory::Create(const TransportParams& params,
                           const QList<Replacement> &replacements,
                           QObject* parent)
{
    ITransport * result = nullptr;
#ifdef ANDROID
    //if( params.type == "Serial" )
      //  result = new DroidTransport( params.params, replacements, parent );

#else
#ifndef EXCLUDE_SERIAL
    if( params.type == "Serial" )
        result = new CSerialTransport( params.params, replacements, parent );
    else
#endif
#endif
    if( params.type == "ClientTCP" )
        result = new CTcpClientTransport( params.params, replacements, parent );
    else if( params.type == "ServerTCP" )
        result = new CTcpServerTransport( params.params, replacements, parent );
    else if( params.type == "HTTP" )
        result = new CHttpTransport( params.params, replacements, parent );
    else if( params.type == "UDP" )
        result = new CUdpTransport( params.params, replacements, parent );

    uint queueDelay = params.params.value( "QueueDelay", 0 ).toUInt();

    if( queueDelay )
        result = MakeQueued( result, queueDelay );

    return result;
}

QueuedTransport *                           TransportFactory::MakeQueued
(
    ITransport *                            coreTransport,
    uint                                    timerStep
)
{
    if( coreTransport )
    {
        return new QueuedTransport( coreTransport, timerStep );
    }

    return nullptr;
}
