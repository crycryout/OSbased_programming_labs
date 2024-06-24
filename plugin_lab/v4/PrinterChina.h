#ifndef PRINTERCHINA_H
#define PRINTERCHINA_H

#include "IPrintPlugin.h"

class PrinterChina : public IPrintPlugin {
public:
  void Print() override;
  const char *GetID() override;
  const char *GetDescription() override;
};

#endif // PRINTERCHINA_H
