#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>

class A {
public:
  A() { i = 0; }

  explicit A(int j) { i = j; }

  virtual ~A() {}

public:
  void f() { std::cout << "in f(): " << i << std::endl; }

public:
  bool Serialize(int fd) {
    if (-1 == fd)
      return false;

    if (::write(fd, &i, sizeof(int)) == -1)
      return false;

    return true;
  }

  bool Deserialize(int fd) {
    if (-1 == fd)
      return false;

    int r = ::read(fd, &i, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    return true;
  }

private:
  int i;
};

class B {
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
  bool Serialize(int fd) {
    if (-1 == fd)
      return false;

    if (::write(fd, &i, sizeof(int)) == -1)
      return false;

    if (::write(fd, &j, sizeof(int)) == -1)
      return false;

    return true;
  }

  bool Deserialize(int fd) {
    if (-1 == fd)
      return false;

    int r = ::read(fd, &i, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    r = ::read(fd, &j, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    return true;
  }

private:
  int i;
  int j;
};

class C {
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

public:
  void f() {
    std::cout << "in f(): " << i << " " << j << " " << k << std::endl;
  }

public:
  bool Serialize(int fd) {
    if (-1 == fd)
      return false;

    if (::write(fd, &i, sizeof(int)) == -1)
      return false;

    if (::write(fd, &j, sizeof(int)) == -1)
      return false;

    if (::write(fd, &k, sizeof(int)) == -1)
      return false;

    return true;
  }

  bool Deserialize(int fd) {
    if (-1 == fd)
      return false;

    int r = ::read(fd, &i, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    r = ::read(fd, &j, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    r = ::read(fd, &k, sizeof(int));
    if ((0 == r) || (-1 == r))
      return false;

    return true;
  }

private:
  int i;
  int j;
  int k;
};
struct Serialized {
  int nType; // 0 for A; 1 for B; 2 for C
  void *pObj;
};

class Serializer {
public:
  bool Serialize(const char *pFilePath, std::vector<Serialized> &v) {
    int fd = ::open(pFilePath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (-1 == fd)
      return false;

    for (int i = 0; i < v.size(); i++) {
      if (::write(fd, &(v[i].nType), 4) == -1) {
        ::close(fd);
        return false;
      }

      if (0 == v[i].nType) {
        A *p = (A *)(v[i].pObj);

        if (p->Serialize(fd) == false)
          return false;
      } else if (1 == v[i].nType) {
        B *p = (B *)(v[i].pObj);

        if (p->Serialize(fd) == false)
          return false;
      } else if (2 == v[i].nType) {
        C *p = (C *)(v[i].pObj);

        if (p->Serialize(fd) == false)
          return false;
      }
    }

    if (::close(fd) == -1)
      return false;

    return true;
  }

  bool Deserialize(const char *pFilePath, std::vector<Serialized> &v) {
    int fd = ::open(pFilePath, O_RDWR, 0644);
    if (-1 == fd)
      return false;

    for (;;) {
      int nType;
      int r = ::read(fd, &nType, 4);
      if ((-1 == r) || (0 == r))
        break;

      if (0 == nType) {
        A *p;
        p = new A();
        p->Deserialize(fd);

        Serialized s;
        s.nType = nType;
        s.pObj = p;

        v.push_back(s);
      } else if (1 == nType) {
        B *p;
        p = new B();
        p->Deserialize(fd);

        Serialized s;
        s.nType = nType;
        s.pObj = p;

        v.push_back(s);
      } else if (2 == nType) {
        C *p;
        p = new C();
        p->Deserialize(fd);

        Serialized s;
        s.nType = nType;
        s.pObj = p;

        v.push_back(s);
      }
    }

    if (::close(fd) == -1)
      return false;

    return true;
  }
};

int main() {
  {
    A a1(2);
    Serialized s1;
    s1.nType = 0;
    s1.pObj = &a1;

    B b1(3);
    Serialized s2;
    s2.nType = 1;
    s2.pObj = &b1;

    C c1(4);
    Serialized s3;
    s3.nType = 2;
    s3.pObj = &c1;

    C c2(5);
    Serialized s4;
    s4.nType = 2;
    s4.pObj = &c2;

    B b2(6);
    Serialized s5;
    s5.nType = 1;
    s5.pObj = &b2;

    A a2(7);
    Serialized s6;
    s6.nType = 0;
    s6.pObj = &a2;

    std::vector<Serialized> v;
    v.push_back(s1);
    v.push_back(s2);
    v.push_back(s3);
    v.push_back(s4);
    v.push_back(s5);
    v.push_back(s6);
    std::cout << v.size() << std::endl;

    Serializer s;
    s.Serialize("data", v);
  }

  {
    Serializer s;
    std::vector<Serialized> v;
    s.Deserialize("data", v);

    for (int i = 0; i < v.size(); i++) {
      std::cout << v.size() << std::endl;
      if (v[i].nType == 0) {
        A *p = (A *)(v[i].pObj);
        p->f();
      } else if (v[i].nType == 1) {
        B *p = (B *)(v[i].pObj);
        p->f();
      } else if (v[i].nType == 2) {
        C *p = (C *)(v[i].pObj);
        p->f();
      }
    }
  }

  return 0;
}
