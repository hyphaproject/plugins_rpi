// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpipir/rpipir.h"

#include <hypha/utils/logger.h>

#include <Poco/ClassLibrary.h>

#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>

#include <wiringPi.h>

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
  if (movement) {
    if (log) Logger::info("Movement detected: " + this->getId());

    QJsonObject object;
    object["alarm"] = true;
    object["id"] = QString::fromStdString(this->id);
    object["type"] = QString::fromStdString(name());
    object["movement"] = movement;
    object["value"] = movement;
    QJsonDocument document(object);
    sendMessage(QString(document.toJson()).toStdString());
    std::this_thread::sleep_for(std::chrono::seconds(2));
  } else {
    QJsonObject object;
    object["alarm"] = false;
    object["id"] = QString::fromStdString(this->id);
    object["type"] = QString::fromStdString(name());
    object["movement"] = movement;
    object["value"] = movement;
    QJsonDocument document(object);
    sendMessage(QString(document.toJson()).toStdString());
  }
}

bool RpiPir::getMovement() {
  movement_mutex.lock();
  bool ret = movement;
  movement_mutex.unlock();
  return ret;
}

void RpiPir::loadConfig(std::string json) {
  QJsonDocument document =
      QJsonDocument::fromJson(QString::fromStdString(json).toUtf8());
  QJsonObject object = document.object();
  if (object.contains("log")) {
    log = object.value("log").toBool();
  }
  if (object.contains("pin")) {
    PIN = object.value("pin").toInt(29);
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

POCO_BEGIN_MANIFEST(HyphaBasePlugin)
POCO_EXPORT_CLASS(RpiPir)
POCO_END_MANIFEST
