#include "CPluginController.h"
#include <iostream>

int main(int argc, char *argv[]) {
  CPluginController controller;
  controller.LoadPlugins("./plugin");

  if (argc == 2) {
    std::string command = argv[1];
    if (command == "help") {
      controller.PrintHelp();
    } else {
      controller.Print(command);
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <command|FuncID>" << std::endl;
  }

  return 0;
}
