#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef void (*PrintMessageFunc)();
typedef const char *(*GetIDFunc)();
typedef const char *(*GetDescriptionFunc)();

void loadPlugins(
    const std::string &directory,
    std::map<std::string, std::pair<std::string, std::string>> &plugins) {
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".so") {
      void *handle = dlopen(entry.path().c_str(), RTLD_LAZY);
      if (!handle) {
        std::cerr << "Could not load the shared library: " << dlerror()
                  << std::endl;
        continue;
      }

      GetIDFunc GetID = (GetIDFunc)dlsym(handle, "GetID");
      const char *dlsym_error = dlerror();
      if (dlsym_error) {
        std::cerr << "Could not locate GetID function: " << dlsym_error
                  << std::endl;
        dlclose(handle);
        continue;
      }

      GetDescriptionFunc GetDescription =
          (GetDescriptionFunc)dlsym(handle, "GetDescription");
      dlsym_error = dlerror();
      if (dlsym_error) {
        std::cerr << "Could not locate GetDescription function: " << dlsym_error
                  << std::endl;
        dlclose(handle);
        continue;
      }

      std::string id = GetID();
      std::string description = GetDescription();
      plugins[id] = {entry.path().string(), description};

      dlclose(handle);
    }
  }
}

void printHelp(
    const std::map<std::string, std::pair<std::string, std::string>> &plugins) {
  for (const auto &plugin : plugins) {
    std::cout << "ID: " << plugin.first << " - " << plugin.second.second
              << std::endl;
  }
}

void executePlugin(const std::string &libraryPath) {
  void *handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
  if (!handle) {
    std::cerr << "Could not load the shared library: " << dlerror()
              << std::endl;
    return;
  }

  PrintMessageFunc PrintMessage =
      (PrintMessageFunc)dlsym(handle, "PrintMessage");
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "Could not locate PrintMessage function: " << dlsym_error
              << std::endl;
    dlclose(handle);
    return;
  }

  PrintMessage();
  dlclose(handle);
}

int main(int argc, char *argv[]) {
  const std::string pluginDir = "./plugin";
  std::map<std::string, std::pair<std::string, std::string>> plugins;
  loadPlugins(pluginDir, plugins);

  if (argc == 2) {
    std::string command = argv[1];
    if (command == "help") {
      printHelp(plugins);
    } else if (plugins.find(command) != plugins.end()) {
      executePlugin(plugins[command].first);
    } else {
      std::cerr << "Unknown command or plugin ID. Use 'help' to see available "
                   "commands and plugins."
                << std::endl;
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <command|FuncID>" << std::endl;
  }

  return 0;
}
