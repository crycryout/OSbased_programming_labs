#include "PrinterWorld.h"
#include <iostream>

void PrinterWorld::Print() { std::cout << "Hello World" << std::endl; }

const char *PrinterWorld::GetID() { return "World"; }

const char *PrinterWorld::GetDescription() { return "Prints Hello World"; }

extern "C" IPrintPlugin *CreatePlugin() { return new PrinterWorld(); }
