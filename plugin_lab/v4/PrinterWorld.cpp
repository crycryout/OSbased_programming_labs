#include "PrinterWorld.h"
#include <iostream>

class PrinterWorld : public Printer {
public:
  void Print() override { std::cout << "Hello World" << std::endl; }

  const char *GetID() override { return "World"; }

  const char *Help() override { return "Prints Hello World"; }
};

extern "C" Printer *CreatePrinter() { return new PrinterWorld(); }
