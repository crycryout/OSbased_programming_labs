#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>

class ILSerializable {
public:
  virtual bool Serialize(int fd) = 0;
  virtual ILSerializable *Deserialize(int fd) = 0;
  virtual bool GetType(int &type) = 0;

public:
  ILSerializable() {}

  virtual ~ILSerializable() {}
};

class A : public ILSerializable {
public:
  A() { i = 0; }

  explicit A(int j) { i = j; }

  virtual ~A() {}

public:
  void f() { std::cout << "in f(): " << i << std::endl; }

public:
  virtual bool GetType(int &type) {
    type = 0;
    return true;
  }

  virtual bool Serialize(int fd) {
    if (-1 == fd)
      return false;

    if (::write(fd, &i, sizeof(int)) == -1)
      return false;

    return true;
  }

  virtual ILSerializable *Deserialize(int fd) {
    if (-1 == fd)
      return NULL;

    A *p = new A();

    int r = ::read(fd, &(p->i), sizeof(int));
    if ((0 == r) || (-1 == r)) {
      delete p;
      return NULL;
    }

    return p;
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

public:
  void f() { std::cout << "in f(): " << i << " " << j << std::endl; }

public:
  virtual bool GetType(int &type) {
    type = 1;
    return true;
  }

  virtual bool Serialize(int fd) {
    if (-1 == fd)
      return false;

    if (::write(fd, &i, sizeof(int)) == -1)
      return false;

    if (::write(fd, &j, sizeof(int)) == -1)
      return false;

    return true;
  }

  virtual ILSerializable *Deserialize(int fd) {
    if (-1 == fd)
      return NULL;

    B *p = new B();

    int r = ::read(fd, &(p->i), sizeof(int));
    if ((0 == r) || (-1 == r)) {
      delete p;
      return NULL;
    }

    r = ::read(fd, &(p->j), sizeof(int));
    if ((0 == r) || (-1 == r)) {
      delete p;
      return NULL;
    }

    return p;
  }

private:
  int i;
  int j;
};

class CLSerializer {
public:
  bool Serialize(const char *pFilePath, std::vector<ILSerializable *> &v) {
    int fd = ::open(pFilePath, O_RDWR | O_CREAT | O_TRUNC, 0);
    if (-1 == fd)
      return false;

    for (int i = 0; i < v.size(); i++) {
      int type;
      v[i]->GetType(type);

      if (::write(fd, &type, 4) == -1) {
        ::close(fd);
        return false;
      }

      v[i]->Serialize(fd);
    }

    if (::close(fd) == -1)
      return false;

    return true;
  }

  bool Deserialize(const char *pFilePath, std::vector<ILSerializable *> &v) {
    int fd = ::open(pFilePath, O_RDWR);
    if (-1 == fd)
      return false;

    for (;;) {
      int nType;
      int r = ::read(fd, &nType, 4);
      if ((-1 == r) || (0 == r))
        break;

      int type;
      for (int i = 0; i < m_vSerialized.size(); i++) {
        m_vSerialized[i]->GetType(type);
        if (type == nType) {
          ILSerializable *p = m_vSerialized[i]->Deserialize(fd);
          if (p != NULL)
            v.push_back(p);
        }
      }
    }

    if (::close(fd) == -1)
      return false;

    return true;
  }

  void Register(ILSerializable *pSerialized) {
    m_vSerialized.push_back(pSerialized);
  }

private:
  std::vector<ILSerializable *> m_vSerialized;
};

int main() {
  {
    A a1(2);
    B b1(3);
    B b2(4);
    A a2(5);

    std::vector<ILSerializable *> v;

    v.push_back(&a1);
    v.push_back(&b1);
    v.push_back(&b2);
    v.push_back(&a2);

    CLSerializer s;
    s.Serialize("data", v);
  }

  {
    CLSerializer s;
    A a;
    B b;
    s.Register(&a);
    s.Register(&b);

    std::vector<ILSerializable *> v;
    s.Deserialize("data", v);

    for (int i = 0; i < v.size(); i++) {
      A *p = dynamic_cast<A *>(v[i]);
      if (p != NULL)
        p->f();

      B *q = dynamic_cast<B *>(v[i]);
      if (q != NULL)
        q->f();
    }
  }

  return 0;
}
