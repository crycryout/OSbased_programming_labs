#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

class Serializable {
public:
  virtual ~Serializable() = default;
  virtual void serialize(std::ofstream &ofs) const = 0;
  virtual void deserialize(std::ifstream &ifs) = 0;
  virtual void print() const = 0;
  virtual std::string getType() const = 0;
};

class MyClass1 : public Serializable {
public:
  int data1;

  MyClass1(int d1 = 0) : data1(d1) {}

  void serialize(std::ofstream &ofs) const override {
    ofs.write(reinterpret_cast<const char *>(&data1), sizeof(data1));
  }

  void deserialize(std::ifstream &ifs) override {
    ifs.read(reinterpret_cast<char *>(&data1), sizeof(data1));
  }

  void print() const override {
    std::cout << "MyClass1 Data: " << data1 << std::endl;
  }

  std::string getType() const override { return "MyClass1"; }
};

class MyClass2 : public Serializable {
public:
  int data2;
  int data3;

  MyClass2(int d2 = 0, int d3 = 0) : data2(d2), data3(d3) {}

  void serialize(std::ofstream &ofs) const override {
    ofs.write(reinterpret_cast<const char *>(&data2), sizeof(data2));
    ofs.write(reinterpret_cast<const char *>(&data3), sizeof(data3));
  }

  void deserialize(std::ifstream &ifs) override {
    ifs.read(reinterpret_cast<char *>(&data2), sizeof(data2));
    ifs.read(reinterpret_cast<char *>(&data3), sizeof(data3));
  }

  void print() const override {
    std::cout << "MyClass2 Data: " << data2 << ", " << data3 << std::endl;
  }

  std::string getType() const override { return "MyClass2"; }
};

class MyClass3 : public Serializable {
public:
  int data4;
  int data5;
  int data6;

  MyClass3(int d4 = 0, int d5 = 0, int d6 = 0)
      : data4(d4), data5(d5), data6(d6) {}

  void serialize(std::ofstream &ofs) const override {
    ofs.write(reinterpret_cast<const char *>(&data4), sizeof(data4));
    ofs.write(reinterpret_cast<const char *>(&data5), sizeof(data5));
    ofs.write(reinterpret_cast<const char *>(&data6), sizeof(data6));
  }

  void deserialize(std::ifstream &ifs) override {
    ifs.read(reinterpret_cast<char *>(&data4), sizeof(data4));
    ifs.read(reinterpret_cast<char *>(&data5), sizeof(data5));
    ifs.read(reinterpret_cast<char *>(&data6), sizeof(data6));
  }

  void print() const override {
    std::cout << "MyClass3 Data: " << data4 << ", " << data5 << ", " << data6
              << std::endl;
  }

  std::string getType() const override { return "MyClass3"; }
};

void serializeObjects(
    const std::string &filename,
    const std::vector<std::shared_ptr<Serializable>> &objects) {
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Failed to write the file." << std::endl;
    return;
  }

  size_t size = objects.size();
  ofs.write(reinterpret_cast<const char *>(&size), sizeof(size));
  for (const auto &obj : objects) {
    std::string type = obj->getType();
    size_t typeSize = type.size();
    ofs.write(reinterpret_cast<const char *>(&typeSize), sizeof(typeSize));
    ofs.write(type.c_str(), typeSize);
    obj->serialize(ofs);
  }

  ofs.close();
}

void deserializeObjects(const std::string &filename,
                        std::vector<std::shared_ptr<Serializable>> &objects) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs) {
    std::cerr << "Error opening file for reading." << std::endl;
    return;
  }

  size_t size;
  ifs.read(reinterpret_cast<char *>(&size), sizeof(size));
  objects.resize(size);

  for (size_t i = 0; i < size; ++i) {
    size_t typeSize;
    ifs.read(reinterpret_cast<char *>(&typeSize), sizeof(typeSize));
    std::string type(typeSize, ' ');
    ifs.read(&type[0], typeSize);

    std::shared_ptr<Serializable> obj;
    if (type == "MyClass1") {
      obj = std::make_shared<MyClass1>();
    } else if (type == "MyClass2") {
      obj = std::make_shared<MyClass2>();
    } else if (type == "MyClass3") {
      obj = std::make_shared<MyClass3>();
    }
    obj->deserialize(ifs);
    objects[i] = obj;
  }

  ifs.close();
}

int main() {
  std::vector<std::shared_ptr<Serializable>> objects = {
      std::make_shared<MyClass1>(MyClass1{1}),
      std::make_shared<MyClass2>(MyClass2{2, 3}),
      std::make_shared<MyClass3>(MyClass3{4, 5, 6}),
  };

  std::string filename = "data";

  serializeObjects(filename, objects);

  std::vector<std::shared_ptr<Serializable>> deserializedObjects;
  deserializeObjects(filename, deserializedObjects);

  for (const auto &obj : deserializedObjects) {
    obj->print();
  }

  return 0;
}
