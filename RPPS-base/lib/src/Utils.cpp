// Description: Утилиты преобразований байт
#include "Utils.h"

#include <QStringList>

int                                         GetInt( const QString & value )

{
    if( value.contains( "x" ) )
    {
        return value.toInt( 0, 16 );
    }
    else
    {
        return value.toInt();
    }
}

int                                         GetValue( const QString & value )
{
    if( value.contains( ":" ) )
    {
        QStringList range = value.split( ":" );
        int from = GetInt( range.at( 0 ) );
        int to = GetInt( range.at( 1 ) );
        int dist = qAbs( to - from );
        int index = qrand() % dist;
        return from + index;
    }
    else
    {
        return GetInt( value );
    }
}
