#ifndef CPLUGINENUMERATOR_H
#define CPLUGINENUMERATOR_H

#include <filesystem>
#include <string>
#include <vector>

class CPluginEnumerator {
public:
  std::vector<std::filesystem::path>
  EnumeratePlugins(const std::string &directory);
};

#endif // CPLUGINENUMERATOR_H
