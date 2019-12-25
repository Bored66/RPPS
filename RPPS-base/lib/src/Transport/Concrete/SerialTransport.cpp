// Description: Транспорт для RS-232
#include "SerialTransport.h"
#include "CommonUtils.h"
#include "OSULiterals.h"

CSerialTransport::CSerialTransport( QHash< QString, QString > params,
                                    Transform * transform,
                                    QObject * parent ) :
    TransportTransform( { OSULiterals::CSERIAL_TYPE, params }, transform, parent )
{
    Init();
}

CSerialTransport::CSerialTransport( QHash< QString, QString > params,
                                    const QList< Replacement >& replacements,
                                    QObject * parent ) :
    TransportTransform( { OSULiterals::CSERIAL_TYPE, params }, replacements, parent )
{
    Init();
}

bool                                        CSerialTransport::IsClearToSend()
{
    Q_ASSERT( SerialPort() );
    return SerialPort()->pinoutSignals() & QSerialPort::ClearToSendSignal;
}

void CSerialTransport::Init()
{
    TransportTransform::Init();
#ifdef USE_DYNAMIC_MEMORY
    if( ! m_pDevice.isNull() )
        m_pDevice->deleteLater();

    m_pDevice = new QSerialPort(nullptr);
#else
    m_pDevice = &_port;
#endif
    Q_ASSERT( !m_pDevice.isNull());

    if( !m_pDevice.isNull() )
    {
        QObject::connect( SerialPort(),
                          SIGNAL( error(QSerialPort::SerialPortError) ),
                          this,
                          SLOT  ( OnError(QSerialPort::SerialPortError) ),
                          Qt::DirectConnection );

        connect( m_pDevice.data(), SIGNAL( readyRead() ), SLOT( ReadData() ), Qt::QueuedConnection );
    }
}

bool                                        CSerialTransport::_OpenConnection( QHash< QString, QString > params )
{
    if(!m_pDevice.isNull())
    {
        auto port = SerialPort();
        QString portName = params.value( OSULiterals::CPORT_TYPE, QString::null );

        if( portName.isEmpty())
        {
            _CloseConnection();
        }
        else
        {
            //TODO: new params => close port descriptor ! (litvinenko)
            port->close();

            port->setPortName( portName );
//            if( ! port->isOpen() )
            {
                port->open( QIODevice::ReadWrite );
            }
            if( port->isOpen() )
            {
                port->setBaudRate( ToBaudRate( params.value( OSULiterals::CBAUDRATE_TYPE, QString::number( (int)QSerialPort::Baud115200 ) ) ) );
                port->setDataBits( ToDataBits( params.value( OSULiterals::CDATABITS_TYPE, QString::number( (int)QSerialPort::Data8 ) ) ) );
                port->setParity  ( ToParity( params.value( OSULiterals::CPARITY_TYPE, QString::number( (int)QSerialPort::NoParity ) ) ) );
                port->setStopBits( ToStopBits( params.value( OSULiterals::CSTOPBITS_TYPE, QString::number( (int)QSerialPort::OneStop ) ) ) );
                port->setFlowControl( ToFlowControl( params.value( OSULiterals::CFLOWCTRL_TYPE, "No" ) ) );

                if( params.contains(OSULiterals::CDTR_TYPE) )
                    port->setDataTerminalReady( ToDtr( params.value( OSULiterals::CDTR_TYPE, "false" ) ) );
                if( params.contains(OSULiterals::CRTS_TYPE) )
                    port->setRequestToSend( ToRts( params.value( OSULiterals::CRTS_TYPE, "false" ) ) );
            }
        }
    }

    return IsOpen();
}

void CSerialTransport::OnError(QSerialPort::SerialPortError serialPortError)
{
    if((serialPortError == QSerialPort::NoError) ||
            (serialPortError >= QSerialPort::UnknownError)  )
        return;

    Log( "Port Error: " + QString::number( serialPortError ) );
    if( ! (m_pDevice.isNull()) )
        SerialPort()->clearError();

    //CloseConnection();
}

QSerialPort *CSerialTransport::SerialPort() const
{
    if (!m_pDevice.isNull())
        return qobject_cast< QSerialPort * >( m_pDevice.data() );
    return nullptr;
}

QSerialPort::BaudRate                       CSerialTransport::ToBaudRate( const QString & rate )
{
    return SearchStaticData(rates, rate, QSerialPort::UnknownBaud);
}

QSerialPort::DataBits
CSerialTransport::ToDataBits(const QString& dataBit)
{
    return SearchStaticData(dataBits, dataBit, QSerialPort::UnknownDataBits);
}

QSerialPort::Parity
CSerialTransport::ToParity(const QString& parity)
{
    return SearchStaticData(parities, parity,  QSerialPort::UnknownParity);
}

QSerialPort::StopBits
CSerialTransport::ToStopBits(const QString& stopBit)
{
    return SearchStaticData(stopBits, stopBit, QSerialPort::UnknownStopBits);
}

QSerialPort::FlowControl
CSerialTransport::ToFlowControl(const QString& control)
{
    return SearchStaticData(controls, control, QSerialPort::UnknownFlowControl);
}

bool CSerialTransport::hasError() const
{
    if (m_pDevice.isNull())
        return true;
    auto port = SerialPort();
    auto error = port->error();
    if(port && (error == QSerialPort::NoError || error == QSerialPort::UnknownError) )
        return false;
    return true;
}

int CSerialTransport::errorCode() const
{
    if (!SerialPort())
        return -1;

    return SerialPort()->error();
}

QString CSerialTransport::errorString() const
{
    if (!SerialPort())
        return "Device not created";

    return SerialPort()->errorString();
}

QString CSerialTransport::getErrorName(int errCode) const
{
    return NameFromEnum(errCode, SerialPort(), "SerialPortError");
}

bool                                        CSerialTransport::ToDtr( const QString & dtr )
{
    return str2bool(dtr);
}

bool CSerialTransport::ToRts(const QString& rts )
{
    return str2bool(rts);
}

const QVector< QPair< QString, QSerialPort::BaudRate > > CSerialTransport::rates = {
    { "1200", QSerialPort::Baud1200 },
    { "2400", QSerialPort::Baud2400 },
    { "4800", QSerialPort::Baud4800 },
    { "9600", QSerialPort::Baud9600 },
    { "19200", QSerialPort::Baud19200 },
    { "38400", QSerialPort::Baud38400 },
    { "57600", QSerialPort::Baud57600 },
    { "115200", QSerialPort::Baud115200 },
    { "Unknown", QSerialPort::UnknownBaud }
};
const QVector< QPair< QString, QSerialPort::DataBits > > CSerialTransport::dataBits = {
    { "5", QSerialPort::Data5 },
    { "6", QSerialPort::Data6 },
    { "7", QSerialPort::Data7 },
    { "8", QSerialPort::Data8 },
    { "Unknown", QSerialPort::UnknownDataBits }
};
const QVector< QPair< QString, QSerialPort::Parity > > CSerialTransport::parities = {
    { "0", QSerialPort::NoParity },
    { "2", QSerialPort::EvenParity },
    { "3", QSerialPort::OddParity },
    { "4", QSerialPort::SpaceParity },
    { "5", QSerialPort::MarkParity },
    { "Unknown", QSerialPort::UnknownParity }
};
const QVector< QPair< QString, QSerialPort::StopBits > > CSerialTransport::stopBits = {
    { "1", QSerialPort::OneStop },
    { "2", QSerialPort::TwoStop },
    { "3", QSerialPort::OneAndHalfStop },
    { "Unknown", QSerialPort::UnknownStopBits }
};
const QVector< QPair< QString, QSerialPort::FlowControl > > CSerialTransport::controls = {
    { "No", QSerialPort::NoFlowControl },
    { "Hardware", QSerialPort::HardwareControl },
    { "Software", QSerialPort::SoftwareControl },
    { "Unknown", QSerialPort::UnknownFlowControl }
};
