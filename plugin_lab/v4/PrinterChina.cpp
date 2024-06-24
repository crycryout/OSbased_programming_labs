#include "PrinterChina.h"
#include <iostream>

class PrinterChina : public Printer {
public:
  void Print() override { std::cout << "Hello China" << std::endl; }

  const char *GetID() override { return "China"; }

  const char *Help() override { return "Prints Hello China"; }
};

extern "C" Printer *CreatePrinter() { return new PrinterChina(); }
