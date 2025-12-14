/*
    File name: pH_measurement.h
    Date: 2025.01.07
    Author: Wilhelm Kuckelsberg
    Description: PH measurement with PH4502C-Sensor
    Quellen: https://github.com/McOrts/M5StickC_PH_sensor
             https://github.com/nthnn/PH4502C-Sensor

*/

#pragma once
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <Arduino.h>
#include <TaskManager.h>
#include <ph4502c_sensor.h>
#include "..\lib\def.h"

/* Pinout: https://cdn.awsli.com.br/969/969921/arq uivos/ph-sensor-ph-4502c.pdf */
#define PH4502C_TEMPERATURE_PIN 4
#define PH4502C_PH_PIN A0
#define PH4502C_PH_TRIGGER_PIN 5

#define PH4502C_CALIBRATION 14.8f
#define PH4502C_READING_INTERVAL 100
#define PH4502C_READING_COUNT 10
// NOTE: The ESP32 ADC has a 12-bit resolution (while most arduinos have 10-bit)
#define ADC_RESOLUTION 1024.0f

// Create an instance of the PH4502C_Sensor
PH4502C_Sensor ph4502c(
    PH4502C_PH_PIN,
    PH4502C_TEMPERATURE_PIN,
    PH4502C_CALIBRATION,
    PH4502C_READING_INTERVAL,
    PH4502C_READING_COUNT,
    ADC_RESOLUTION);

class pH_measurement : public Task::Base
{

public:
    pH_measurement(const String &name)
        : Task::Base(name)
    {
    }

    virtual void begin() override
    {
        Serial.println("Starting PH4502C Sensor...");
        ph4502c.init();
    }

    virtual void update() override
    {
        // // Read the temperature from the sensor
        Serial.println("Temperature reading:" + String(ph4502c.read_temp()));   

        // // Read the pH level by average
        Serial.println("pH Level Reading-new: " + String(ph4502c.read_ph_level())); //P0 ~ A0

        // // Read a single pH level value
        Serial.println("pH Level Single Reading: " + String(ph4502c.read_ph_level_single()));

        delay(1000);

        // int sensorValue = analogRead(A0);
        // float spannung = sensorValue*(5,0/1023);
        // Serial.print("sensorValue - ");Serial.println(sensorValue);
        // Serial.print("Spannung - ");Serial.println(spannung);
    }
};
#endif // MEASUREMENT_H