// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpidht11/rpidht11.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <Poco/ClassLibrary.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <wiringPi.h>

#include <hypha/utils/logger.h>

#define MAX_TIME 85
#define DHT11PIN 7

using namespace hypha::utils;
using namespace hypha::plugin;
using namespace hypha::plugin::dht11;

void RpiDht11::doWork() {
  if (alarm) {
    measure();
    if (this->temperature > this->maxtemp ||
        this->temperature < this->mintemp) {
      boost::property_tree::ptree pt;
      pt.put("alarm", true);
      pt.put("id", this->id);
      pt.put("type", name());
      pt.put("temperature", temperature);
      pt.put("value", temperature);
      std::stringstream ss;
      boost::property_tree::write_json(ss, pt);
      sendMessage(ss.str());
      std::this_thread::sleep_for(std::chrono::seconds(20));
    } else {
      boost::property_tree::ptree pt;
      pt.put("alarm", false);
      pt.put("id", this->id);
      pt.put("type", name());
      pt.put("temperature", temperature);
      pt.put("value", temperature);
      std::stringstream ss;
      boost::property_tree::write_json(ss, pt);
      sendMessage(ss.str());
    }
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void RpiDht11::setup() { wiringPiSetup(); }

std::string RpiDht11::communicate(std::string UNUSED(message)) {
  return getStatusMessage();
}

std::string RpiDht11::getStatusMessage() {
  measure();
  return "Temperature: " + std::to_string(temperature) + " Humidity: " +
         std::to_string(humidity);
}

void RpiDht11::loadConfig(std::string json) {
  boost::property_tree::ptree pt;
  std::stringstream ss(json);
  boost::property_tree::read_json(ss, pt);
  if (pt.get_optional<bool>("alarm")) {
    alarm = pt.get<bool>("alarm");
  }
  if (pt.get_optional<int>("maxtemp")) {
    maxtemp = pt.get<int>("maxtemp");
  }
  if (pt.get_optional<int>("mintemp")) {
    mintemp = pt.get<int>("mintemp");
  }
}

std::string RpiDht11::getConfig() { return "{}"; }

HyphaBasePlugin *RpiDht11::getInstance(std::string id) {
  RpiDht11 *instance = new RpiDht11();
  instance->setId(id);
  return instance;
}

void RpiDht11::receiveMessage(std::string UNUSED(message)) {}

void RpiDht11::measure() {
  measure_mutex.lock();
  uint8_t lststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  for (i = 0; i < 5; i++) dht11_val[i] = 0;
  pinMode(DHT11PIN, OUTPUT);
  digitalWrite(DHT11PIN, LOW);
  delay(18);
  digitalWrite(DHT11PIN, HIGH);
  delayMicroseconds(40);
  pinMode(DHT11PIN, INPUT);
  for (i = 0; i < MAX_TIME; i++) {
    counter = 0;
    while (digitalRead(DHT11PIN) == lststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) break;
    }
    lststate = digitalRead(DHT11PIN);
    if (counter == 255) break;
    // top 3 transistions are ignored
    if ((i >= 4) && (i % 2 == 0)) {
      dht11_val[j / 8] <<= 1;
      if (counter > 16) dht11_val[j / 8] |= 1;
      j++;
    }
  }
  // verify cheksum and print the verified data
  if ((j >= 40) &&
      (dht11_val[4] ==
       ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF))) {
    temperature = dht11_val[2];
    humidity = dht11_val[0];
  }
  measure_mutex.unlock();
}

PLUGIN_API POCO_BEGIN_MANIFEST(HyphaBasePlugin)
    POCO_EXPORT_CLASS(RpiDht11) POCO_END_MANIFEST
