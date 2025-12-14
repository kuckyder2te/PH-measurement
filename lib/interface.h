#pragma once
/*
    File name. interface.h
    Date: 2024.10.06
    Author: Wilhelm Kuckelsberg / Stephan Scholz
    Description: Pool Control
*/

#include <Arduino.h>
#include <PubSubClient.h>
#include "..\lib\def.h"

namespace interface
{
    typedef struct
    {
        bool ph_state;
    } model_t;
}

extern char msg[50];
extern PubSubClient client;

void ph_pump(bool option)
{
    if (option)
    {
        Serial.println("pH Pump ON");
        digitalWrite(PH_PUMP, HIGH);
    }
    else
    {
        Serial.println("pH Pump OFF");
        digitalWrite(PH_PUMP, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0;
    client.publish("outPoolservice/ph_pump/state", msg);
} /*--------------------------------------------------------------------------*/

void ph_calibration(bool option)
{
    if (option)
    {
     // read the input on analog pin 0:
        int sensorValue = analogRead(A0);
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
        float voltage = sensorValue * (5.0 / 1023.0);
        // print out the value you read:
        Serial.println(voltage);
        delay(300);
    }
    else
    {
        Serial.println("pH calibration OFF");
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0;
    client.publish("outPoolservice/ph_calibration/state", msg);
} /*--------------------------------------------------------------------------*/

/*------------------------ end of interface.h------------------------------------*/