// Description: Файл описания констант
#ifndef DI_CONSTANTS_H
#define DI_CONSTANTS_H

#include <QStringList>
#include <QHash>
#include <QVector>
#include "DILiterals.h"

/**
 * \brief Константа simpleTypes содержит список
 * простых типов данных.
 */
static const QStringList simpleTypes =
{
    "uchar", "ushort", "short", "uint", "int", "float", "double", "char"
};

static const QStringList floatTypes =
{
    "float", "long float", "double", "qreal"
};

/**
 * \brief Константа simpleTypesSize определяет массив размеров в байтах типов
 * данных списка @sa simpleTypes.
 * @warning Последовательность значений массива соответствует последовательности
 * элементов константы @sa simpleTypes.
 */
static const uint simpleTypesSize[] =
{
    sizeof(uchar), sizeof(ushort), sizeof(short), sizeof(uint),
    sizeof(int), sizeof(float), sizeof(double), sizeof( char )
};
//template <typename KEY=uint, typename VAL=QString>
//AssocCollection QHash<KEY,VAL>;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
static QHash<QString, uint> SimpleTypeSizes =
{
    {"uchar",sizeof(uchar)}, {"ushort",sizeof(ushort)},
    {"short",sizeof(short)}, {"uint",sizeof(uint)},
    {"int",sizeof(int)}, {"float",sizeof(float)},
    {"double",sizeof(double)}, {"char",sizeof(char)}
};
#else
    //TODO: решить проблему инициализации для SEMANTIC_TREE_SPLITTERS SEMANTIC_TREE_SPLITTERS_WRAP
#endif

inline void                                         InitTreeSplitters ()
{}


#endif // DI_CONSTANTS_H

