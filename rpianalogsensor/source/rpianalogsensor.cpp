// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpianalogsensor/rpianalogsensor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <hypha/utils/logger.h>

#include <Poco/ClassLibrary.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>

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
  QJsonDocument document =
      QJsonDocument::fromJson(QString::fromStdString(json).toUtf8());
  QJsonObject object = document.object();
  if (object.contains("alarm")) {
    alarm = object.value("alarm").toBool();
  }
  if (object.contains("pin")) {
    PIN = object.value("pin").toInt(0);
  }
  if (object.contains("min")) {
    min = object.value("min").toInt(1);
  }
  if (object.contains("max")) {
    max = object.value("max").toInt(1023);
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

POCO_BEGIN_MANIFEST(HyphaBasePlugin)
POCO_EXPORT_CLASS(RpiAnalogSensor)
POCO_END_MANIFEST
