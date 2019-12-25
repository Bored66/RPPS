// Description: Чтение настроек транспорта из файла
#ifndef TRANSPORTREADER_H
#define TRANSPORTREADER_H

#include "XmlReader.h"
#include "Transport.h"

struct TransportParams;

const QString strTransportTokenTree = R"(
...( default! )
    Transport( name! type! )
        DeviceWaitDelay
            #!
        BufferThreshold
            #!
        Port
            #!
        BaudRate
            #!
        DataBits
            #!
        Parity
            #!
        StopBits
            #!
        DTR
            #!
        RTS
            #!
        FlowControl
            #!
        Reconnect
            #!
        ServerIP
            #!
        Host
            #!
        Authorization
            #!
        IP
            #!
        SendPort
            #!
        ReceivePort
            #!
        VERBOSE
            #!
)";

class CTransportReader :
    public                                  CXmlReader
{
    Q_OBJECT
public:
    explicit CTransportReader( QObject * parent );

    TransportParams                         ParseTransport( const QString & xml );
};

#endif // TRANSPORTREADER_H
