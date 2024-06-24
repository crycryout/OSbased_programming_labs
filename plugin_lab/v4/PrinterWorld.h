#ifndef PRINTERWORLD_H
#define PRINTERWORLD_H

#ifdef __cplusplus
extern "C" {
#endif

class Printer {
public:
  virtual void Print() = 0;
  virtual const char *GetID() = 0;
  virtual const char *Help() = 0;
  virtual ~Printer() {}
};

extern "C" Printer *CreatePrinter();

#ifdef __cplusplus
}
#endif

#endif // PRINTERWORLD_H
