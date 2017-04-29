// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpianalogsensor/rpianalogsensor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <Poco/ClassLibrary.h>
#include <QtCore/QProcess>

#include <hypha/utils/logger.h>

using namespace hypha::utils;
using namespace hypha::plugin;
using namespace hypha::plugin::rpianalogsensor;

void RpiAnalogSensor::doWork() {
  if (alarm) {
    measure();
    if (analog < min || analog > max) {
      boost::property_tree::ptree sendobject;
      sendobject.put("alarm", true);
      sendobject.put("id", this->id);
      sendobject.put("type", name());
      sendobject.put("analog", analog);
      sendobject.put("value", analog);
      std::stringstream sstream;
      boost::property_tree::write_json(sstream, sendobject);
      sendMessage(sstream.str());
      std::this_thread::sleep_for(std::chrono::seconds(20));
    } else {
      if (counter > 100) {
        boost::property_tree::ptree sendobject;
        sendobject.put("alarm", false);
        sendobject.put("id", this->id);
        sendobject.put("type", name());
        sendobject.put("analog", analog);
        sendobject.put("value", analog);
        std::stringstream sstream;
        boost::property_tree::write_json(sstream, sendobject);
        sendMessage(sstream.str());
        counter = 0;
      }
    }
    counter++;
  }
}

void RpiAnalogSensor::setup() {}

std::string RpiAnalogSensor::communicate(std::string UNUSED(message)) {
  return getStatusMessage();
}

std::string RpiAnalogSensor::getStatusMessage() {
  measure();
  return "Analog value: " + std::to_string(analog);
}

void RpiAnalogSensor::loadConfig(std::string json) {
    boost::property_tree::ptree ptjson;
    std::stringstream ssjson(json);
    boost::property_tree::read_json(ssjson, ptjson);

    if (ptjson.get_optional<bool>("alarm")) {
      alarm = ptjson.get<bool>("alarm");
    }
    if (ptjson.get_optional<int>("pin")) {
      PIN = ptjson.get<int>("pin");
    }
    if (ptjson.get_optional<int>("min")) {
      min = ptjson.get<int>("min");
    }
    if (ptjson.get_optional<int>("max")) {
      max = ptjson.get<int>("max");
    }
}

std::string RpiAnalogSensor::getConfig() { return "{}"; }

HyphaBasePlugin *RpiAnalogSensor::getInstance(std::string id) {
  RpiAnalogSensor *instance = new RpiAnalogSensor();
  instance->setId(id);
  return instance;
}

void RpiAnalogSensor::receiveMessage(std::string UNUSED(message)) {}

void RpiAnalogSensor::measure() {
  QProcess process;
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start("python", QStringList()
                              << "../plugins/rpianalogsensor.py"
                              << QString::fromStdString(std::to_string(PIN)));
  process.waitForFinished();
  QString output(process.readAllStandardOutput());
  measure_mutex.lock();
  this->analog = std::stoi(output.toStdString());
  measure_mutex.unlock();
}

PLUGIN_API POCO_BEGIN_MANIFEST(HyphaBasePlugin)
    POCO_EXPORT_CLASS(RpiAnalogSensor) POCO_END_MANIFEST
