#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>

class ILSerializable {
public:
  virtual bool Serialize(int fd) = 0;
  virtual ILSerializable *Deserialize(int fd) = 0;
  virtual bool GetType(int &type) = 0;
  virtual void f() = 0;

public:
  ILSerializable() {}
  virtual ~ILSerializable() {}
};

class A : public ILSerializable {
public:
  A() { i = 0; }
  explicit A(int j) { i = j; }
  virtual ~A() {}

  void f() override { std::cout << "in f(): " << i << std::endl; }

  bool Serialize(int fd) override {
    if (fd == -1)
      return false;
    return ::write(fd, &i, sizeof(int)) != -1;
  }

  ILSerializable *Deserialize(int fd) override {
    if (fd == -1)
      return nullptr;
    if (::read(fd, &i, sizeof(int)) <= 0)
      return nullptr;
    return this;
  }

  bool GetType(int &type) override {
    type = 0;
    return true;
  }

private:
  int i;
};

class B : public ILSerializable {
public:
  B() {
    i = 0;
    j = 0;
  }
  explicit B(int k) {
    i = k;
    j = k + 1;
  }
  virtual ~B() {}

  void f() override { std::cout << "in f(): " << i << " " << j << std::endl; }

  bool Serialize(int fd) override {
    if (fd == -1)
      return false;
    if (::write(fd, &i, sizeof(int)) == -1)
      return false;
    if (::write(fd, &j, sizeof(int)) == -1)
      return false;
    return true;
  }

  ILSerializable *Deserialize(int fd) override {
    if (fd == -1)
      return nullptr;
    if (::read(fd, &i, sizeof(int)) <= 0)
      return nullptr;
    if (::read(fd, &j, sizeof(int)) <= 0)
      return nullptr;
    return this;
  }

  bool GetType(int &type) override {
    type = 1;
    return true;
  }

private:
  int i, j;
};

class C : public ILSerializable {
public:
  C() {
    i = 0;
    j = 0;
    k = 0;
  }
  explicit C(int x) {
    i = x;
    j = x + 1;
    k = x + 2;
  }
  virtual ~C() {}

  void f() override {
    std::cout << "in f(): " << i << " " << j << " " << k << std::endl;
  }

  bool Serialize(int fd) override {
    if (fd == -1)
      return false;
    if (::write(fd, &i, sizeof(int)) == -1)
      return false;
    if (::write(fd, &j, sizeof(int)) == -1)
      return false;
    if (::write(fd, &k, sizeof(int)) == -1)
      return false;
    return true;
  }

  ILSerializable *Deserialize(int fd) override {
    if (fd == -1)
      return nullptr;
    if (::read(fd, &i, sizeof(int)) <= 0)
      return nullptr;
    if (::read(fd, &j, sizeof(int)) <= 0)
      return nullptr;
    if (::read(fd, &k, sizeof(int)) <= 0)
      return nullptr;
    return this;
  }

  bool GetType(int &type) override {
    type = 2;
    return true;
  }

private:
  int i, j, k;
};

class Serializer {
public:
  bool Serialize(const char *pFilePath, std::vector<ILSerializable *> &v) {
    int fd = ::open(pFilePath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
      return false;

    for (auto obj : v) {
      int type;
      obj->GetType(type);
      if (::write(fd, &type, sizeof(int)) == -1) {
        ::close(fd);
        return false;
      }
      if (!obj->Serialize(fd)) {
        ::close(fd);
        return false;
      }
    }

    return ::close(fd) != -1;
  }

  bool Deserialize(const char *pFilePath, std::vector<ILSerializable *> &v) {
    int fd = ::open(pFilePath, O_RDWR, 0644);
    if (fd == -1)
      return false;

    while (true) {
      int type;
      int r = ::read(fd, &type, sizeof(int));
      if (r <= 0)
        break;

      ILSerializable *obj = nullptr;
      if (type == 0)
        obj = new A();
      else if (type == 1)
        obj = new B();
      else if (type == 2)
        obj = new C();

      if (obj == nullptr || !obj->Deserialize(fd)) {
        delete obj;
        ::close(fd);
        return false;
      }

      v.push_back(obj);
    }

    return ::close(fd) != -1;
  }
};

int main() {
  {
    A a1(2);
    B b1(3);
    C c1(4);
    C c2(5);
    B b2(6);
    A a2(7);

    std::vector<ILSerializable *> v = {&a1, &b1, &c1, &c2, &b2, &a2};
    std::cout << v.size() << std::endl;

    Serializer s;
    s.Serialize("data", v);
  }

  {
    Serializer s;
    std::vector<ILSerializable *> v;
    s.Deserialize("data", v);

    for (auto obj : v) {
      obj->f();
      delete obj;
    }
  }

  return 0;
}
