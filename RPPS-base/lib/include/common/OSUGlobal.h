// Description: Глобальные определения
#ifndef OSU_GLOBAL_H
#define OSU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OSU_LIBRARY)
#  define OSU_EXPORT Q_DECL_EXPORT
#else
#  define OSU_EXPORT Q_DECL_IMPORT
#endif

#endif // OSU_GLOBAL_H
