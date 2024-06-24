#include <dlfcn.h>
#include <iostream>

typedef void (*PrintMessageFunc)();

int main() {
  void *handle = dlopen("./libPrinter.so", RTLD_LAZY);
  if (!handle) {
    std::cerr << "Could not load the shared library: " << dlerror()
              << std::endl;
    return 1;
  }

  dlerror();

  PrintMessageFunc PrintMessage =
      (PrintMessageFunc)dlsym(handle, "PrintMessage");
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "Could not locate the function: " << dlsym_error << std::endl;
    dlclose(handle);
    return 1;
  }

  PrintMessage();

  dlclose(handle);
  return 0;
}
