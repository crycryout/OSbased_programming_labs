#ifndef IPRINTPLUGIN_H
#define IPRINTPLUGIN_H

class IPrintPlugin {
public:
  virtual ~IPrintPlugin() {}
  virtual void Print() = 0;
  virtual const char *GetID() = 0;
  virtual const char *GetDescription() = 0;
};

typedef IPrintPlugin *(*CreatePluginFunc)();

#endif // IPRINTPLUGIN_H
