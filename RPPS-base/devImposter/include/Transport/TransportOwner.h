// Description: Возможность смены владельца транспорта
#ifndef TRANSPORTOWNER
#define TRANSPORTOWNER

#include "Transport.h"

class TransportOwner
{
public:
  virtual void SetTransport(ITransport *) = 0;
  virtual ITransport *GetTransport() const = 0;
};

#endif // TRANSPORTOWNER
