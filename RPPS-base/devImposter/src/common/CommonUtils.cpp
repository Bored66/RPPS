// Description: Утилиты
#include <QByteArray>
#include "CommonUtils.h"
#include <QSet>

uchar                                       hexStringToByte( const QByteArray & hex )
{
    return QByteArray::fromHex( hex ).at( 0 );
}
ushort                                      toWord( uchar low, uchar hi )
{
    return ( hi << 8 ) + low;
}
uchar                                       getHigh( ushort data )
{
    return data >> 8;
}
uchar                                       getLow( ushort data )
{
    return data & 0xFF;
}
bool str2bool(const QString & str)
{
    return ( ( str.trimmed().toLower() == "true" ) || (str.toInt() != 0) );
}
const QString logDateTimeFormat = "dd.M.yyyy HH:mm:ss.zzz";

QString getLogPrefix()
{
    return QString("[%1] ")
            .arg( QDateTime::currentDateTime().
                  toString(logDateTimeFormat) ).toStdString().c_str();
}
QString insertDelim(const QString &data, const char *delim)
{
    QString tmp;
    for(int i = 0; i < data.size();i+=2)
        tmp += data.mid(i, 2) + delim;
    return tmp;
}
bool IsDataPrintable(const QByteArray &data )
{
    QList<QChar> symbols { QChar('\r'), QChar('\n'), QChar('\0') };

    for( auto i = 0; i < data.size(); i++ )
    {
        QChar qCh = QLatin1Char( data.at( i ) ).toLatin1();
        if( ! ( qCh.isPrint() || symbols.contains(qCh) ) )
            return false;
    }
    return true;
}
