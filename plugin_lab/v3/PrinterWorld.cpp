#include "PrinterWorld.h"
#include <iostream>

void PrintMessage() { std::cout << "Hello World" << std::endl; }

const char *GetID() { return "World"; }

const char *GetDescription() { return "Prints Hello World"; }
