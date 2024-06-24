#include "CPluginController.h"
#include <dlfcn.h>
#include <filesystem>
#include <iostream>

CPluginController::CPluginController() {}

CPluginController::~CPluginController() {
  for (auto handle : m_handles) {
    dlclose(handle);
  }
}

void CPluginController::LoadPlugins(const std::string &directory) {
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".so") {
      void *handle = dlopen(entry.path().c_str(), RTLD_LAZY);
      if (!handle) {
        std::cerr << "Could not load the shared library: " << dlerror()
                  << std::endl;
        continue;
      }
      CreatePluginFunc createPlugin =
          (CreatePluginFunc)dlsym(handle, "CreatePlugin");
      const char *dlsym_error = dlerror();
      if (dlsym_error) {
        std::cerr << "Could not locate CreatePlugin function: " << dlsym_error
                  << std::endl;
        dlclose(handle);
        continue;
      }
      IPrintPlugin *plugin = createPlugin();
      m_plugins.push_back(plugin);
      m_handles.push_back(handle);
    }
  }
}

void CPluginController::PrintHelp() {
  for (const auto &plugin : m_plugins) {
    std::cout << "ID: " << plugin->GetID() << " - " << plugin->GetDescription()
              << std::endl;
  }
}

void CPluginController::Print(const std::string &pluginID) {
  for (const auto &plugin : m_plugins) {
    if (plugin->GetID() == pluginID) {
      plugin->Print();
      return;
    }
  }
  std::cerr << "Unknown plugin ID: " << pluginID << std::endl;
}
