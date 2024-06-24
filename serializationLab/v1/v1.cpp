#include <fstream>
#include <iostream>

class CA_LL {
private:
  int i;

public:
  CA_LL(int value = 0) : i(value) {}

  bool Serialize(const char *pFilePath) {
    std::ofstream outFile(pFilePath, std::ios::binary);
    if (!outFile) {
      std::cerr << "Failed to open file for writing: " << pFilePath
                << std::endl;
      return false;
    }

    outFile.write(reinterpret_cast<char *>(&i), sizeof(i));
    outFile.close();
    return true;
  }

  bool Deserialize(const char *pFilePath) {
    std::ifstream inFile(pFilePath, std::ios::binary);
    if (!inFile) {
      std::cerr << "Failed to open file for reading: " << pFilePath
                << std::endl;
      return false;
    }

    inFile.read(reinterpret_cast<char *>(&i), sizeof(i));
    inFile.close();
    return true;
  }

  void Print() const { std::cout << "Value of i: " << i << std::endl; }
};

int main() {
  CA_LL obj(42);
  const char *filePath = "serialized_object.bin";

  if (obj.Serialize(filePath)) {
    std::cout << "Object serialized successfully." << std::endl;
  }

  CA_LL newObj;
  if (newObj.Deserialize(filePath)) {
    std::cout << "Object deserialized successfully." << std::endl;
    newObj.Print();
  }

  return 0;
}
