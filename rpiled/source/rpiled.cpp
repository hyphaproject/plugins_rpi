// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpiled/rpiled.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <Poco/ClassLibrary.h>
#include <wiringPi.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <hypha/plugin/hyphabaseplugin.h>
#include <hypha/utils/logger.h>

#define RED 23
#define YELLOW 24
#define GREEN 25

using namespace hypha::utils;
using namespace hypha::plugin;
using namespace hypha::plugin::rpiled;

void RpiLed::doWork() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

void RpiLed::setup() {
  wiringPiSetup();
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

std::string RpiLed::communicate(std::string UNUSED(message)) {
  return getStatusMessage();
}

std::string RpiLed::getStatusMessage() {
  return std::string((red ? "Red on" : "Red off") + std::string(", ") +
                     (yellow ? "Yellow on" : "Yellow off") + std::string(", ") +
                     (green ? "Green on" : "Green off"));
}

void RpiLed::loadConfig(std::string UNUSED(json)) {}

std::string RpiLed::getConfig() { return "{}"; }

HyphaBasePlugin *RpiLed::getInstance(std::string id) {
  RpiLed *instance = new RpiLed();
  instance->setId(id);
  return instance;
}

void RpiLed::receiveMessage(std::string message) {
  boost::property_tree::ptree ptjson;
  std::stringstream ssjson(message);
  boost::property_tree::read_json(ssjson, ptjson);

  if (ptjson.get_optional<bool>("red")) {
    setRed(ptjson.get<bool>("red"));
  }
  if (ptjson.get_optional<bool>("green")) {
    setRed(ptjson.get<bool>("green"));
  }
  if (ptjson.get_optional<bool>("yellow")) {
    setRed(ptjson.get<bool>("yellow"));
  }
}

void RpiLed::setRed(bool on) {
  red = on;
  digitalWrite(RED, red ? HIGH : LOW);
}

void RpiLed::setYellow(bool on) {
  yellow = on;
  digitalWrite(YELLOW, yellow ? HIGH : LOW);
}

void RpiLed::setGreen(bool on) {
  green = on;
  digitalWrite(GREEN, green ? HIGH : LOW);
}

PLUGIN_API POCO_BEGIN_MANIFEST(HyphaBasePlugin)
    POCO_EXPORT_CLASS(RpiLed) POCO_END_MANIFEST
