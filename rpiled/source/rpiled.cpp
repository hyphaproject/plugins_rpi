// Copyright (c) 2015-2017 Hypha

#include "hypha/plugins/rpiled/rpiled.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <Poco/ClassLibrary.h>
#include <QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>

#include <wiringPi.h>

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
  QJsonDocument document = QJsonDocument::fromJson(message.c_str());
  QJsonObject object = document.object();
  if (object.contains("red")) {
    setRed(object.value("red").toBool());
  }
  if (object.contains("green")) {
    setGreen(object.value("green").toBool());
  }
  if (object.contains("yellow")) {
    setYellow(object.value("yellow").toBool());
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

POCO_BEGIN_MANIFEST(HyphaBasePlugin)
POCO_EXPORT_CLASS(RpiLed)
POCO_END_MANIFEST
