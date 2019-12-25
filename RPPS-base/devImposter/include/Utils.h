// Description: Утилиты преобразований байт
#ifndef UTILS_H
#define UTILS_H

#include <QList>
#include <QPair>
#include <QTime>

#include <QtEndian>

template< typename VALUE_TYPE >
VALUE_TYPE                                  GetRandomValue( QList< QPair< double, VALUE_TYPE > > normalized )
{
    if( normalized.isEmpty() )
    {
        return 0;
    }

    int randomVal = qrand();
    double fractValue = ( ( double )randomVal )/( RAND_MAX );
    for( auto option : normalized )
    {
        if( option.first >= fractValue )
        {
            return option.second;
        }
    }
    return normalized.last().second;
}

template< class ENUM, typename T >
void                                        Merge( QList< ENUM > keys, T & value )
{
    for( auto key : keys )
    {
        value |= ( T ) key;
    }
}

template< class ENUM, typename T = long >
T                                           Merge( QList< ENUM > keys )
{
    T value = 0;
    for( auto key : keys )
    {
        value |= ( T ) key;
    }
    return value;
}

template< class ENUM, typename T >
bool                                        Has( ENUM key, T value )
{
    return ( ( T ) key ) & value ;
}

template< class ENUM, typename T >
bool                                        Equal( ENUM key, T value )
{
    return ( ( T ) key ) == value;
}

template< typename DATA_TYPE >
void                                        InitListFromBuffer
(
    QList< DATA_TYPE > **                   dataPtr,
    char **                                 offset,
    int                                     bytesToCopy
)
{
    *dataPtr = new QList< DATA_TYPE >();
    for( uint i = 0; i < bytesToCopy/sizeof( DATA_TYPE ); ++i )
    {
        DATA_TYPE subStruct;
        memcpy( &( subStruct ), *offset, sizeof( DATA_TYPE ) );
        ( * offset ) += sizeof( DATA_TYPE );
        ( * dataPtr )->append( subStruct );
    }
}

template< typename FIELD_TYPE >
void                                        InitFieldFromBuffer( FIELD_TYPE * field, char ** offset )
{
    memcpy( field, *offset, sizeof( FIELD_TYPE ) );
    ( * offset ) += sizeof( FIELD_TYPE );
}
template < class T >
T                                           fromByteArray( QByteArray data )
{
    T * result = static_cast< T * >( ( void * )data.data() );

    T answer = * result;
    return answer;
}
template < class T >
QByteArray                                  toByteArray( T data )
{
    return QByteArray( static_cast< char * >( ( void * ) &data ), sizeof( T ) );
}

template < typename SizeType = quint32, typename T >
T                                           FromBigEndian( T source )
{
    SizeType * toSwap = ( SizeType* )&source;
    SizeType swapped = qFromBigEndian( *toSwap );
    T * result = ( T* )&swapped;
    return *result;
}

template < typename SizeType = quint32, typename T >
T                                           ToBigEndian( T source )
{
    SizeType * toSwap = ( SizeType* )&source;
    SizeType swapped = qToBigEndian( *toSwap );
    T * result = ( T* )&swapped;
    return *result;
}

int                                         GetInt( const QString & value );
int                                         GetValue( const QString & value );


#ifndef arraySize
#if __cplusplus < 201103L
#define arraySize( ARRAY ) (sizeof( ARRAY )/sizeof( ARRAY[ 0 ] ))
#else
template<typename T, std::size_t N>
constexpr std::size_t arraySize( const T (&)[N]) noexcept
{ return N; }// Эффективный и современныи С++, Скотт Мейерс, стр.29
#define arraySize( ARRAY ) (sizeof( ARRAY )/sizeof( ARRAY[ 0 ] ))
#endif
#endif //arraySize

#endif //UTILS_H
