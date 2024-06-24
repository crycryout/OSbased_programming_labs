#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

typedef void (*PrintMessageFunc)();

void loadAndExecutePlugins(const std::string &directory) {
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".so") {
      void *handle = dlopen(entry.path().c_str(), RTLD_LAZY);
      if (!handle) {
        std::cerr << "Could not load the shared library: " << dlerror()
                  << std::endl;
        continue;
      }

      dlerror(); // Clear any existing error

      PrintMessageFunc PrintMessage =
          (PrintMessageFunc)dlsym(handle, "PrintMessage");
      const char *dlsym_error = dlerror();
      if (dlsym_error) {
        std::cerr << "Could not locate the function: " << dlsym_error
                  << std::endl;
        dlclose(handle);
        continue;
      }

      PrintMessage();

      dlclose(handle);
    }
  }
}

int main() {
  const std::string pluginDir = "./plugin";
  loadAndExecutePlugins(pluginDir);

  return 0;
}
