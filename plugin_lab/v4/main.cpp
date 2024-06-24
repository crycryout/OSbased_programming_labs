#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Printer {
public:
  virtual void Print() = 0;
  virtual const char *GetID() = 0;
  virtual const char *Help() = 0;
  virtual ~Printer() {}
};

typedef Printer *(*CreatePrinterFunc)();

void loadPlugins(const std::string &directory,
                 std::map<std::string, std::unique_ptr<Printer>> &printers) {
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".so") {
      std::cout << "Loading " << entry.path().string() << std::endl;
      void *handle = dlopen(entry.path().c_str(), RTLD_LAZY);
      if (!handle) {
        std::cerr << "Could not load the shared library: " << dlerror()
                  << std::endl;
        continue;
      }

      dlerror(); // Clear any existing error

      CreatePrinterFunc CreatePrinter =
          (CreatePrinterFunc)dlsym(handle, "CreatePrinter");
      const char *dlsym_error = dlerror();
      if (dlsym_error) {
        std::cerr << "Could not locate CreatePrinter function: " << dlsym_error
                  << std::endl;
        dlclose(handle);
        continue;
      }

      std::unique_ptr<Printer> printer(CreatePrinter());
      if (!printer) {
        std::cerr << "Failed to create Printer instance from " << entry.path()
                  << std::endl;
        dlclose(handle);
        continue;
      }

      std::string id = printer->GetID();
      std::cout << "Loaded plugin ID: " << id << std::endl;
      printers[id] = std::move(printer);

      // Do not close handle here, keep it open while using the plugin
      // dlclose(handle);
    }
  }
}

void printHelp(
    const std::map<std::string, std::unique_ptr<Printer>> &printers) {
  for (const auto &plugin : printers) {
    std::cout << "ID: " << plugin.first << " - " << plugin.second->Help()
              << std::endl;
  }
}

void executePlugin(
    const std::string &id,
    const std::map<std::string, std::unique_ptr<Printer>> &printers) {
  auto it = printers.find(id);
  if (it != printers.end()) {
    it->second->Print();
  } else {
    std::cerr << "Unknown plugin ID. Use 'help' to see available plugins."
              << std::endl;
  }
}

int main(int argc, char *argv[]) {
  const std::string pluginDir = "./plugin";
  std::map<std::string, std::unique_ptr<Printer>> printers;
  loadPlugins(pluginDir, printers);

  if (argc == 2) {
    std::string command = argv[1];
    if (command == "help") {
      printHelp(printers);
    } else {
      executePlugin(command, printers);
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <command|FuncID>" << std::endl;
  }

  return 0;
}
