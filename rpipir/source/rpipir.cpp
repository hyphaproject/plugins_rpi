// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpipir/rpipir.h"

#include <Poco/ClassLibrary.h>
#include <wiringPi.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <hypha/utils/logger.h>

using namespace hypha::utils;
using namespace hypha::plugin;
using namespace hypha::plugin::rpipir;

void RpiPir::doWork() {
  try {
    measure();
    sendMovement();
  } catch (...) {
    Logger::error("Error in RpiPir");
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void RpiPir::setup() { wiringPiSetup(); }

std::string RpiPir::communicate(std::string UNUSED(message)) {
  return getStatusMessage();
}

void RpiPir::measure() {
  movement_mutex.lock();
  this->movement = (digitalRead(PIN) == HIGH);
  movement_mutex.unlock();
}

void RpiPir::sendMovement() {
  bool movement = getMovement();

  boost::property_tree::ptree sendobject;
  std::stringstream ssso;

  sendobject.put("id", this->id);
  sendobject.put("type", name());
  sendobject.put("movement", movement);
  sendobject.put("value", movement);

  if (movement) {
    if (log) Logger::info("Movement detected: " + this->getId());

    sendobject.put("alarm", true);
    boost::property_tree::write_json(ssso, sendobject);
    sendMessage(ssso.str());
    std::this_thread::sleep_for(std::chrono::seconds(2));
  } else {
    sendobject.put("alarm", false);
    boost::property_tree::write_json(ssso, sendobject);
    sendMessage(ssso.str());
  }
}

bool RpiPir::getMovement() {
  movement_mutex.lock();
  bool ret = movement;
  movement_mutex.unlock();
  return ret;
}

void RpiPir::loadConfig(std::string json) {
  boost::property_tree::ptree ptjson;
  std::stringstream ssjson(json);
  boost::property_tree::read_json(ssjson, ptjson);

  if (ptjson.get_optional<bool>("log")) {
    log = ptjson.get<bool>("log");
  }
  if (ptjson.get_optional<int>("pin")) {
    PIN = ptjson.get<int>("pin");
  }
}

std::string RpiPir::getConfig() { return "{}"; }

hypha::plugin::HyphaBasePlugin *RpiPir::getInstance(std::string id) {
  RpiPir *instance = new RpiPir();
  instance->setId(id);
  return instance;
}

void RpiPir::receiveMessage(std::string UNUSED(message)) {}

std::string RpiPir::getStatusMessage() {
  return getMovement() ? "Movement" : "No Movement";
}

PLUGIN_API POCO_BEGIN_MANIFEST(HyphaBasePlugin)
    POCO_EXPORT_CLASS(RpiPir) POCO_END_MANIFEST
