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

  void Print() const { std::cout << "CA_LL Value of i: " << i << std::endl; }
};

class CB_LL {
private:
  int x, y;

public:
  CB_LL(int valueX = 0, int valueY = 0) : x(valueX), y(valueY) {}

  bool Serialize(std::ofstream &outFile) const {
    outFile.write(reinterpret_cast<const char *>(&x), sizeof(x));
    outFile.write(reinterpret_cast<const char *>(&y), sizeof(y));
    return outFile.good();
  }

  bool Deserialize(std::ifstream &inFile) {
    inFile.read(reinterpret_cast<char *>(&x), sizeof(x));
    inFile.read(reinterpret_cast<char *>(&y), sizeof(y));
    return inFile.good();
  }

  void Print() const {
    std::cout << "CB_LL Value of x: " << x << ", y: " << y << std::endl;
  }
};

struct Serialized {
  int nType; // 0 for CA_LL; 1 for CB_LL
  void *pObj;
};

class Serializer {
public:
  bool Serialize(const char *pFilePath, std::vector<Serialized> &v) {
    std::ofstream outFile(pFilePath, std::ios::binary);
    if (!outFile) {
      std::cerr << "Failed to open file for writing: " << pFilePath
                << std::endl;
      return false;
    }

    size_t size = v.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size));

    for (const auto &serialized : v) {
      outFile.write(reinterpret_cast<const char *>(&serialized.nType),
                    sizeof(serialized.nType));
      if (serialized.nType == 0) {
        CA_LL *obj = static_cast<CA_LL *>(serialized.pObj);
        obj->Serialize(outFile);
      } else if (serialized.nType == 1) {
        CB_LL *obj = static_cast<CB_LL *>(serialized.pObj);
        obj->Serialize(outFile);
      }
    }

    outFile.close();
    return true;
  }

  bool Deserialize(const char *pFilePath, std::vector<Serialized> &v) {
    std::ifstream inFile(pFilePath, std::ios::binary);
    if (!inFile) {
      std::cerr << "Failed to open file for reading: " << pFilePath
                << std::endl;
      return false;
    }

    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size));
    v.resize(size);

    for (auto &serialized : v) {
      inFile.read(reinterpret_cast<char *>(&serialized.nType),
                  sizeof(serialized.nType));
      if (serialized.nType == 0) {
        CA_LL *obj = new CA_LL();
        obj->Deserialize(inFile);
        serialized.pObj = obj;
      } else if (serialized.nType == 1) {
        CB_LL *obj = new CB_LL();
        obj->Deserialize(inFile);
        serialized.pObj = obj;
      }
    }

    inFile.close();
    return true;
  }
};

int main() {
  std::vector<Serialized> objects;

  CA_LL objA1(42);
  CA_LL objA2(84);
  CB_LL objB1(7, 14);
  CB_LL objB2(21, 28);

  objects.push_back({0, &objA1});
  objects.push_back({0, &objA2});
  objects.push_back({1, &objB1});
  objects.push_back({1, &objB2});

  const char *filePath = "serialized_objects_v3.bin";

  Serializer serializer;

  if (serializer.Serialize(filePath, objects)) {
    std::cout << "Objects serialized successfully." << std::endl;
  }

  std::vector<Serialized> newObjects;
  if (serializer.Deserialize(filePath, newObjects)) {
    std::cout << "Objects deserialized successfully." << std::endl;
    for (const auto &serialized : newObjects) {
      if (serialized.nType == 0) {
        CA_LL *obj = static_cast<CA_LL *>(serialized.pObj);
        obj->Print();
        delete obj;
      } else if (serialized.nType == 1) {
        CB_LL *obj = static_cast<CB_LL *>(serialized.pObj);
        obj->Print();
        delete obj;
      }
    }
  }

  return 0;
}
