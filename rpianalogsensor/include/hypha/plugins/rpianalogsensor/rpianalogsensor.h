// Copyright (c) 2015-2017 Hypha

#ifndef RPIANALOGSENSOR_H
#define RPIANALOGSENSOR_H

#include <mutex>
#include <string>

#include <hypha/plugin/hyphabaseplugin.h>
#include <hypha/plugin/hyphasensor.h>

namespace hypha {
namespace plugin {
namespace rpianalogsensor {
class RpiAnalogSensor : public HyphaSensor {
 public:
  void doWork();
  void setup();
  const std::string name() { return "rpianalogsensor"; }
  const std::string getTitle() { return "RpiAnalogSensor"; }
  const std::string getVersion() { return "0.1"; }
  const std::string getDescription() { return "Plugin to read analog sensor"; }
  const std::string getConfigDescription() override {
    return "{"
           "\"confdesc\":["
           "{\"name\":\"alarm\", "
           "\"type\":\"boolean\",\"value\":\"false\",\"description\":\"Give "
           "alarm if measured value is under min or over max.\"},"
           "{\"name\":\"pin\", "
           "\"type\":\"integer\",\"value\":\"6\",\"description\":\"PIN of "
           "connected Sensor.\"},"
           "{\"name\":\"min\", "
           "\"type\":\"integer\",\"value\":\"1\",\"description\":\"Min value "
           "to give alarm if measurement is beneath.\"},"
           "{\"name\":\"max\", "
           "\"type\":\"integer\",\"value\":\"1023\",\"description\":\"Max "
           "value to give alarm if measurement is above.\"}"
           "]}";
  }
  void loadConfig(std::string json);
  std::string getConfig();
  std::string getStatusMessage();

  void measure();
  HyphaBasePlugin *getInstance(std::string id);

  void receiveMessage(std::string message);
  std::string communicate(std::string message);

 private:
  std::mutex measure_mutex;
  bool alarm = false;
  int analog = -1;
  int PIN = 6;
  int min = 1;
  int max = 1023;
  int counter = 0;
};
}
}
}
#endif  // RPIANALOGSENSOR_H
