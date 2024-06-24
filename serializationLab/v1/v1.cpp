#include <fstream>
#include <iostream>

class MyClass {
public:
  int data;

  void serialize(const std::string &filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
      std::cerr << "Failed to write the file." << std::endl;
      return;
    }

    ofs.write(reinterpret_cast<const char *>(&data), sizeof(data));
    ofs.close();
  }

  void deserialize(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
      std::cerr << "Failed to read the file." << std::endl;
      return;
    }

    ifs.read(reinterpret_cast<char *>(&data), sizeof(data));
    ifs.close();
  }

  void print() const { std::cout << "Data: " << data << std::endl; }
};

int main() {
  MyClass obj1;
  obj1.data = 42;
  std::string filename = "data";
  obj1.serialize(filename);
  MyClass obj2;
  obj2.deserialize(filename);
  obj2.print();
  return 0;
}
