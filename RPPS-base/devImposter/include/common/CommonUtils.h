// Description: Утилиты
#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <QtCore>

//------------------------------------------------------------------------------
///
/// \note Example:
/// \code
///     ...
///     SleepMethods::msleep( 500 );
///     ...
/// \endcode
///
class   SleepMethods:
        public                          QThread
{
    public:
        static void sleep(unsigned long s)
        {
            QThread::sleep( s );
        }
        static void msleep(int ms)
        {
            QThread::msleep( ms );
        }
        static void usleep(unsigned long us)
        {
            QThread::usleep( us );
        }
};
//------------------------------------------------------------------------------

uchar                                       hexStringToByte( const QByteArray & hex );
ushort                                      toWord( uchar low , uchar hi = 0x0 );
uchar                                       getHigh( ushort data );
uchar                                       getLow( ushort data );
bool                                        str2bool(const QString & str);
QString getLogPrefix();
QString insertDelim(const QString &data, const char *delim = " ");

template < class T >
T                         SearchStaticData( QVector< QPair< QString, T > > source, const QString& pattern, const T &defaultValue )
{
    static QHash< QString, T > data = FillDataHash( source );

    if( data.contains( pattern ) )
    {
        return data[ pattern ];
    }
    return defaultValue;
}
template < class T >
QHash< QString, T >                         FillDataHash( QVector< QPair< QString, T > > source )
{
    QHash< QString, T > data;

    for( auto pair : source )
    {
        data[ pair.first ] = pair.second;
    }

    return data;
}
bool IsDataPrintable(const QByteArray &data );
#ifndef IS_NULL
#define IS_NULL(POINTER) (nullptr == (POINTER))
#endif
#ifndef NOT_NULL
#define NOT_NULL(POINTER) (nullptr != (POINTER))
#endif

#endif // COMMON_UTILS_H
