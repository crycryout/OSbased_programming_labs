#include <fstream>
#include <iostream>
#include <vector>

class MyClass1 {
public:
  int data1;

  void serialize(std::ofstream &ofs) const {
    ofs.write(reinterpret_cast<const char *>(&data1), sizeof(data1));
  }

  void deserialize(std::ifstream &ifs) {
    ifs.read(reinterpret_cast<char *>(&data1), sizeof(data1));
  }

  void print() const { std::cout << "MyClass1 Data: " << data1 << std::endl; }
};

class MyClass2 {
public:
  int data2;
  int data3;

  void serialize(std::ofstream &ofs) const {
    ofs.write(reinterpret_cast<const char *>(&data2), sizeof(data2));
    ofs.write(reinterpret_cast<const char *>(&data3), sizeof(data3));
  }

  void deserialize(std::ifstream &ifs) {
    ifs.read(reinterpret_cast<char *>(&data2), sizeof(data2));
    ifs.read(reinterpret_cast<char *>(&data3), sizeof(data3));
  }

  void print() const {
    std::cout << "MyClass2 Data: " << data2 << ", " << data3 << std::endl;
  }
};

void serializeObjects(const std::string &filename,
                      const std::vector<MyClass1> &objects1,
                      const std::vector<MyClass2> &objects2) {
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error opening file for writing." << std::endl;
    return;
  }

  size_t size1 = objects1.size();
  ofs.write(reinterpret_cast<const char *>(&size1), sizeof(size1));
  for (const auto &obj : objects1) {
    obj.serialize(ofs);
  }

  size_t size2 = objects2.size();
  ofs.write(reinterpret_cast<const char *>(&size2), sizeof(size2));
  for (const auto &obj : objects2) {
    obj.serialize(ofs);
  }

  ofs.close();
}

void deserializeObjects(const std::string &filename,
                        std::vector<MyClass1> &objects1,
                        std::vector<MyClass2> &objects2) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs) {
    std::cerr << "Error opening file for reading." << std::endl;
    return;
  }

  size_t size1;
  ifs.read(reinterpret_cast<char *>(&size1), sizeof(size1));
  objects1.resize(size1);
  for (auto &obj : objects1) {
    obj.deserialize(ifs);
  }

  size_t size2;
  ifs.read(reinterpret_cast<char *>(&size2),
           sizeof(size2)); // 读取MyClass2对象数量
  objects2.resize(size2);
  for (auto &obj : objects2) {
    obj.deserialize(ifs);
  }

  ifs.close();
}

int main() {
  std::vector<MyClass1> objects1 = {{1}, {2}, {3}, {4}};
  std::vector<MyClass2> objects2 = {{5, 6}, {7, 8}, {9, 10}, {11, 12}};

  std::string filename = "data";

  serializeObjects(filename, objects1, objects2);

  std::vector<MyClass1> deserializedObjects1;
  std::vector<MyClass2> deserializedObjects2;
  deserializeObjects(filename, deserializedObjects1, deserializedObjects2);

  for (const auto &obj : deserializedObjects1) {
    obj.print();
  }

  for (const auto &obj : deserializedObjects2) {
    obj.print();
  }

  return 0;
}
