// Copyright (c) 2015-2017 Hypha

#ifndef RPILED_H
#define RPILED_H

#include <hypha/plugin/hyphaactor.h>
#include <hypha/plugin/hyphabaseplugin.h>
#include <string>

namespace hypha {
namespace plugin {
namespace rpiled {
class RpiLed : public HyphaActor {
 public:
  void doWork();
  void setup();
  const std::string name() { return "rpiled"; }
  const std::string getTitle() { return "RpiLed"; }
  const std::string getVersion() { return "0.1"; }
  const std::string getDescription() {
    return "Plugin control red, yellow and green LED";
  }
  const std::string getConfigDescription() { return "{}"; }
  void loadConfig(std::string json);
  std::string getConfig();
  std::string getStatusMessage();

  HyphaBasePlugin *getInstance(std::string id);

  void receiveMessage(std::string message);
  std::string communicate(std::string message);

  void setRed(bool on);
  void setYellow(bool on);
  void setGreen(bool on);

 private:
  bool red = false;
  bool yellow = false;
  bool green = false;
};
}
}
}
#endif  // RPILED_H
