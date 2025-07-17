/*
File name: main.cpp
Author:    Stefan Scholz / Wilhel Kuckelsberg
Date:      2025.05.25
Project:   ph Mesurement

https://github.com/Edistechlab/DIY-Heimautomation-Buch/tree/master/Sensoren/Regensensor
*/


/*
ich was eingefügt
*/

#include <Arduino.h>
#include <TaskManager.h>
#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
// #include "..\lib\model.h"
#include "..\lib\interface.h"
#include "..\lib\pH_measurement.h"
#include "..\lib\secrets.h"
#include "..\lib\def.h"

// #include <ArduinoOTA.h>

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

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
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

  String topicStr(topic); // macht aus dem Topic ein String -> topicStr

  if (topicStr.indexOf('/') >= 0)
  /*prüft ob die Nachricht ein / enthält was ja den Pfad des Topics aufteilt
  und mindestens eins sollte bei inPump/Egon ja drin sein
  */
  {
    Serial.print("topic = ");
    Serial.println(topic);
    //  The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/') + 1);
    /*
      löscht inPump/ so dass in topicStr nur noch Egon übrig bleibt
    */
    if (topicStr.indexOf('/') >= 0)
    {
      String rootStr = topicStr.substring(0, topicStr.indexOf('/'));
      Serial.println(rootStr);
      if (rootStr == "ph_pump")
      {
        switch (payload[0])
        {
        case '0': // false
          // Pump off
          ph_pump(false);
          break;
        case '1':
          // Pump on
          ph_pump(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
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

  //setup_wifi();

  // ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  // ArduinoOTA.begin();

  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);

  Tasks.add<pH_measurement>("pH_measurement")->startFps(100);

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
  // if WiFi is down, try reconnecting
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
  if(lastMillis - millis() >= 1000 )
  {
    digitalWrite(LED_BUILTIN, STATE_LED);
    STATE_LED =! STATE_LED;
    lastMillis = millis();
  }

  // if (!client.connected())
  // {
  //   reconnect();
  // }
  // client.loop();

  // client.publish("outGarden/pH", String(ph4502c.read_ph_level()).c_str());

} /*--------------------------------------------------------------------------*/