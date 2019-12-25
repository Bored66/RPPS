// Description: Сохранение настроек транспорта
#ifndef TRANSPORTWRITER_H
#define TRANSPORTWRITER_H

#include <QString>

#include "Transport.h"

class TransportWriter
{
public:
    TransportWriter();

    static void                             Write
    (
        const QString &                     filename,
        const TransportParams &             params
    );
};

#endif // TRANSPORTWRITER_H