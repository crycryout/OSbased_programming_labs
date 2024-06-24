#include <fstream>
#include <iostream>
#include <vector>

class CA_LL {
private:
  int i;

public:
  CA_LL(int value = 0) : i(value) {}

  bool Serialize(std::ofstream &outFile) const {
    outFile.write(reinterpret_cast<const char *>(&i), sizeof(i));
    return outFile.good();
  }

  bool Deserialize(std::ifstream &inFile) {
    inFile.read(reinterpret_cast<char *>(&i), sizeof(i));
    return inFile.good();
  }

  void Print() const { std::cout << "Value of i: " << i << std::endl; }
};

class SerializerForCA_LLs {
public:
  bool Serialize(const char *pFilePath, const std::vector<CA_LL> &v) {
    std::ofstream outFile(pFilePath, std::ios::binary);
    if (!outFile) {
      std::cerr << "Failed to open file for writing: " << pFilePath
                << std::endl;
      return false;
    }

    size_t size = v.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));

    for (const auto &obj : v) {
      if (!obj.Serialize(outFile)) {
        std::cerr << "Failed to serialize object." << std::endl;
        return false;
      }
    }

    outFile.close();
    return true;
  }

  bool Deserialize(const char *pFilePath, std::vector<CA_LL> &v) {
    std::ifstream inFile(pFilePath, std::ios::binary);
    if (!inFile) {
      std::cerr << "Failed to open file for reading: " << pFilePath
                << std::endl;
      return false;
    }

    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
    v.resize(size);

    for (auto &obj : v) {
      if (!obj.Deserialize(inFile)) {
        std::cerr << "Failed to deserialize object." << std::endl;
        return false;
      }
    }

    inFile.close();
    return true;
  }
};

int main() {
  std::vector<CA_LL> objects = {CA_LL(1), CA_LL(2), CA_LL(3)};
  const char *filePath = "serialized_objects.bin";

  SerializerForCA_LLs serializer;

  if (serializer.Serialize(filePath, objects)) {
    std::cout << "Objects serialized successfully." << std::endl;
  }

  std::vector<CA_LL> newObjects;
  if (serializer.Deserialize(filePath, newObjects)) {
    std::cout << "Objects deserialized successfully." << std::endl;
    for (const auto &obj : newObjects) {
      obj.Print();
    }
  }

  return 0;
}
