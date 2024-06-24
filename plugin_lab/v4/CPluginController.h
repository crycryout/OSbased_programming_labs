#ifndef CPLUGINCONTROLLER_H
#define CPLUGINCONTROLLER_H

#include "IPrintPlugin.h"
#include <string>
#include <vector>

class CPluginController {
public:
  CPluginController();
  ~CPluginController();

  void LoadPlugins(const std::string &directory);
  void PrintHelp();
  void Print(const std::string &pluginID);

private:
  std::vector<void *> m_handles;
  std::vector<IPrintPlugin *> m_plugins;
};

#endif // CPLUGINCONTROLLER_H
