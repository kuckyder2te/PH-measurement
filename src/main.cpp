/*
File name: main.cpp
Author:    Stefan Scholz / Wilhelm Kuckelsberg
Date:      2025.05.25
Project:   ph Mesurement

https://github.com/Edistechlab/DIY-Heimautomation-Buch/tree/master/Sensoren/Regensensor
*/
///@cond
#include <Arduino.h>
#include <TaskManager.h>
#include "def.h"

#define LOCAL_DEBUG
char logBuf[DEBUG_MESSAGE_BUFFER_SIZE];

#include "../include/myLogger.h"

#include "../include/network.h"
#include "../include/messageBroker.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <ArduinoJson.h>
// #include "..\include\services\interface.h"
#include "..\include\services\pH_measurement.h"
#include "secrets.h"
/// @endcond

Network *_network;
JsonDocument doc;

HardwareSerial *TestOutput = &Serial;
HardwareSerial *DebugOutput = &Serial;

MessageBroker msgBroker;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

bool STATE_LED;


void setup()
{
  delay(2000);
  Serial.begin(115200);
  DebugOutput->begin(DEBUG_SPEED);
  Logger::setOutputFunction(&MyLoggerOutput::localUdpLogger);
  // Logger::setOutputFunction(&MyLoggerOutput::willyUdpLogger);
  Logger::setLogLevel(Logger::DEBUG); // Muss immer einen Wert in platformio.ini haben (SILENT)
  delay(500);                         // For switching on Serial Monitor
  LOGGER_NOTICE_FMT("************************* pH measurement (%s) *************************", __TIMESTAMP__);
  LOGGER_NOTICE("Start building pH measurement");
  _network = new Network(SID, PW, HOSTNAME, MQTT, MessageBroker::callback);
  _network->begin(MQTT, PORT_FOR_PH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Tasks.add<Services::pH_measurement>("ph.measurement")
      ->init(PH4502C_PH_PIN)
      ->startFps(0.017); // 0,017 ~ 1 minute

  msgBroker.printTopics();
  LOGGER_NOTICE("Finished building pH measurement. Will enter infinite loop");

} /*--------------------------------------------------------------------------*/

void loop()
{
   static unsigned long lastMillis;
  static bool lastState = LOW;
  _network->update();

  Tasks.update();

  if (millis() - lastMillis >= 1000) // This can also be used to test the main loop.
  {

    digitalWrite(LED_BUILTIN, lastState);
    lastState = !lastState;
    lastMillis = millis();
  }

} /*--------------------------------------------------------------------------*/