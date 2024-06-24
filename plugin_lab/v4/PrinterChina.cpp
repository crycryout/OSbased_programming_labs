#include "PrinterChina.h"
#include <iostream>

void PrinterChina::Print() { std::cout << "Hello China" << std::endl; }

const char *PrinterChina::GetID() { return "China"; }

const char *PrinterChina::GetDescription() { return "Prints Hello China"; }

extern "C" IPrintPlugin *CreatePlugin() { return new PrinterChina(); }
