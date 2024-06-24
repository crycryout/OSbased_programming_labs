#include "CPluginEnumerator.h"

std::vector<std::filesystem::path>
CPluginEnumerator::EnumeratePlugins(const std::string &directory) {
  std::vector<std::filesystem::path> plugins;
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".so") {
      plugins.push_back(entry.path());
    }
  }
  return plugins;
}
