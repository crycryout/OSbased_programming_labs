#ifndef PRINTERWORLD_H
#define PRINTERWORLD_H

#include "IPrintPlugin.h"

class PrinterWorld : public IPrintPlugin {
public:
  void Print() override;
  const char *GetID() override;
  const char *GetDescription() override;
};

#endif // PRINTERWORLD_H
