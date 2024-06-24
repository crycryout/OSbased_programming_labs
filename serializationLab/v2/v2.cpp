#include <fstream>
#include <iostream>
#include <vector>

class MyClass {
public:
  int data;

  void serialize(std::ofstream &ofs) const {
    ofs.write(reinterpret_cast<const char *>(&data), sizeof(data));
  }

  void deserialize(std::ifstream &ifs) {
    ifs.read(reinterpret_cast<char *>(&data), sizeof(data));
  }

  void print() const { std::cout << "Data: " << data << std::endl; }
};

void serializeObjects(const std::string &filename,
                      const std::vector<MyClass> &objects) {
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Failed to write file." << std::endl;
    return;
  }

  size_t size = objects.size();
  ofs.write(reinterpret_cast<const char *>(&size),
            sizeof(size)); // 写入对象数量

  for (const auto &obj : objects) {
    obj.serialize(ofs);
  }

  ofs.close();
}

std::vector<MyClass> deserializeObjects(const std::string &filename) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs) {
    std::cerr << "Failed to read file." << std::endl;
    return {};
  }

  size_t size;
  ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

  std::vector<MyClass> objects(size);
  for (auto &obj : objects) {
    obj.deserialize(ifs);
  }

  ifs.close();
  return objects;
}

int main() {
  std::vector<MyClass> objects = {{42}, {13}, {7}, {99}};

  std::string filename = "data";

  serializeObjects(filename, objects);

  std::vector<MyClass> deserializedObjects = deserializeObjects(filename);

  for (const auto &obj : deserializedObjects) {
    obj.print();
  }

  return 0;
}
