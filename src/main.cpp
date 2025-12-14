/*
File name: main.cpp
Author:    Stefan Scholz / Wilhelm Kuckelsberg
Date:      2025.05.25
Project:   ph Mesurement

https://github.com/Edistechlab/DIY-Heimautomation-Buch/tree/master/Sensoren/Regensensor
*/

#include <Arduino.h>
#include <TaskManager.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "..\include\interface.h"
#include "..\include\pH_measurement.h"
#include "secrets.h"
#include "def.h"

const char *ssid = SID;
const char *password = PW;
const char *mqtt_server = MQTT;

WiFiClient espClient;
PubSubClient client(espClient);
JsonDocument doc;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

unsigned long lastReconnectAttempt = 0;

bool STATE_LED;

// ----- OTA begin --------
#include <ElegantOTA.h>

AsyncWebServer server(80);

unsigned long ota_progress_millis = 0;

void onOTAStart()
{
  Serial.println("OTA update started!");
}

void onOTAProgress(size_t current, size_t final)
{
  if (millis() - ota_progress_millis > 1000)
  {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success)
{
  if (success)
  {
    Serial.println("OTA update finished successfully!");
  }
  else
  {
    Serial.println("There was an error during OTA update!");
  }
}
// ----- OTA end --------

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

} /*--------------------------------------------------------------------------*/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String topicStr(topic);

  if (topicStr.indexOf('/') >= 0)
  {
    Serial.print("topic = ");
    Serial.println(topic);

    topicStr.remove(0, topicStr.indexOf('/') + 1);

    if (topicStr.indexOf('/') >= 0)
    {
      String rootStr = topicStr.substring(0, topicStr.indexOf('/'));
      Serial.println(rootStr);
      if (rootStr == "ph_pump")
      {
        switch (payload[0])
        {
        case '0': // false
          ph_pump(false);
          break;
        case '1':
          ph_pump(true);
          break;
        default:
          break;
        }
      }
      else
      {
        Serial.println("Unknown topic");
      }
    }
  }
} /*--------------------------------------------------------------------------*/

void setup()
{
  delay(2000);
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(PH_PUMP, OUTPUT_OPEN_DRAIN);
  digitalWrite(PH_PUMP, LOW);

  Serial.println();
  Serial.println("pH Messung");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Tasks.add<pH_measurement>("pH_measurement")->startFps(100);

  // Code for current date and time
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(200, "text/plain", "Garden-Service"); });

  // Code for current Build-date/-time   code von ChatGPT
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String message = "pH measurement (Build: ";
    message += __DATE__;
    message += " ";
    message += __TIME__;
    message += ")";
    request->send(200, "text/plain", message); });

  ElegantOTA.begin(&server); // Start ElegantOTA
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.setAutoReboot(true);
  ElegantOTA.onEnd(onOTAEnd);
  server.begin();
  Serial.println("HTTP server started");

} /*--------------------------------------------------------------------------*/

bool reconnect()
{
  Serial.print("Attempting MQTT connection...");
  String clientId = "ESP32Client-";
  clientId += String(random(0xffff), HEX);

  if (client.connect(clientId.c_str()))
  {
    Serial.println("connected");
    client.publish("outGarden", "Garden control");
    client.subscribe("inGarden/#");
    return true;
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    return false;
  }
} /*--------------------------------------------------------------------------*/

void loop()
{
  static unsigned long previousMillis = millis();
  ElegantOTA.loop();

  if ((WiFi.status() != WL_CONNECTED) && (millis() - previousMillis >= 30000))
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = millis();
  }

  if (!client.connected())
  {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      if (reconnect())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    client.loop();
  }

  Tasks.update();

  static long lastMillis = 0;
  if (lastMillis - millis() >= 1000)
  {
    digitalWrite(LED_BUILTIN, STATE_LED);
    STATE_LED = !STATE_LED;
    lastMillis = millis();
  }

} /*--------------------------------------------------------------------------*/