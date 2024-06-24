#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <unordered_map>
#include <vector>

class ILSerializable {
public:
  virtual ~ILSerializable() = default;
  virtual bool Serialize(int fd) const = 0;
  virtual ILSerializable *Deserialize(int fd) = 0;
  virtual bool GetType(int &type) const = 0;
  virtual void Print() const = 0;
};

class CA_LL : public ILSerializable {
private:
  int i;

public:
  CA_LL(int value = 0) : i(value) {}

  bool Serialize(int fd) const override {
    if (::write(fd, &i, sizeof(int)) == -1)
      return false;
    return true;
  }

  ILSerializable *Deserialize(int fd) override {
    CA_LL *p = new CA_LL();
    if (::read(fd, &(p->i), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    return p;
  }

  bool GetType(int &type) const override {
    type = 0;
    return true;
  }

  void Print() const override {
    std::cout << "CA_LL Value of i: " << i << std::endl;
  }
};

class CB_LL : public ILSerializable {
private:
  int x, y;

public:
  CB_LL(int valueX = 0, int valueY = 0) : x(valueX), y(valueY) {}

  bool Serialize(int fd) const override {
    if (::write(fd, &x, sizeof(int)) == -1)
      return false;
    if (::write(fd, &y, sizeof(int)) == -1)
      return false;
    return true;
  }

  ILSerializable *Deserialize(int fd) override {
    CB_LL *p = new CB_LL();
    if (::read(fd, &(p->x), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    if (::read(fd, &(p->y), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    return p;
  }

  bool GetType(int &type) const override {
    type = 1;
    return true;
  }

  void Print() const override {
    std::cout << "CB_LL Value of x: " << x << ", y: " << y << std::endl;
  }
};

class CC_LL : public ILSerializable {
private:
  int a, b, c;

public:
  CC_LL(int valueA = 0, int valueB = 0, int valueC = 0)
      : a(valueA), b(valueB), c(valueC) {}

  bool Serialize(int fd) const override {
    if (::write(fd, &a, sizeof(int)) == -1)
      return false;
    if (::write(fd, &b, sizeof(int)) == -1)
      return false;
    if (::write(fd, &c, sizeof(int)) == -1)
      return false;
    return true;
  }

  ILSerializable *Deserialize(int fd) override {
    CC_LL *p = new CC_LL();
    if (::read(fd, &(p->a), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    if (::read(fd, &(p->b), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    if (::read(fd, &(p->c), sizeof(int)) == -1) {
      delete p;
      return nullptr;
    }
    return p;
  }

  bool GetType(int &type) const override {
    type = 2;
    return true;
  }

  void Print() const override {
    std::cout << "CC_LL Value of a: " << a << ", b: " << b << ", c: " << c
              << std::endl;
  }
};

class CLSerializer {
public:
  bool Serialize(const char *pFilePath,
                 const std::vector<std::shared_ptr<ILSerializable>> &v) {
    int fd = ::open(pFilePath, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
      return false;

    for (const auto &obj : v) {
      int type;
      obj->GetType(type);
      if (::write(fd, &type, sizeof(type)) == -1) {
        ::close(fd);
        return false;
      }

      if (!obj->Serialize(fd)) {
        ::close(fd);
        return false;
      }
    }

    ::close(fd);
    return true;
  }

  bool Deserialize(const char *pFilePath,
                   std::vector<std::shared_ptr<ILSerializable>> &v) {
    int fd = ::open(pFilePath, O_RDONLY);
    if (fd == -1)
      return false;

    while (true) {
      int nType;
      int r = ::read(fd, &nType, sizeof(nType));
      if (r == 0) // EOF
        break;
      if (r == -1) {
        ::close(fd);
        return false;
      }

      std::shared_ptr<ILSerializable> obj;
      if (m_registry.find(nType) != m_registry.end()) {
        obj.reset(m_registry[nType]->Deserialize(fd));
        if (obj) {
          v.push_back(obj);
        }
      }
    }

    ::close(fd);
    return true;
  }

  void Register(int type, ILSerializable *prototype) {
    m_registry[type] = prototype;
  }

private:
  std::unordered_map<int, ILSerializable *> m_registry;
};

int main() {
  {
    CA_LL a1(2);
    CB_LL b1(3);
    CB_LL b2(4);
    CA_LL a2(5);
    CC_LL c1(1, 2, 3);
    CC_LL c2(4, 5, 6);

    std::vector<std::shared_ptr<ILSerializable>> v;

    v.push_back(std::make_shared<CA_LL>(a1));
    v.push_back(std::make_shared<CB_LL>(b1));
    v.push_back(std::make_shared<CB_LL>(b2));
    v.push_back(std::make_shared<CA_LL>(a2));
    v.push_back(std::make_shared<CC_LL>(c1));
    v.push_back(std::make_shared<CC_LL>(c2));

    CLSerializer s;
    s.Serialize("data", v);
  }

  {
    CLSerializer s;
    s.Register(0, new CA_LL());
    s.Register(1, new CB_LL());
    s.Register(2, new CC_LL());

    std::vector<std::shared_ptr<ILSerializable>> v;
    s.Deserialize("data", v);

    for (const auto &obj : v) {
      obj->Print();
    }
  }

  return 0;
}
