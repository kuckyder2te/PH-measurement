/*
    File name: pH_measurement.h
    Date: 2025.01.07
    Author: Wilhelm Kuckelsberg
    Description: PH measurement with PH4502C-Sensor
    Quellen: https://github.com/McOrts/M5StickC_PH_sensor
             https://github.com/nthnn/PH4502C-Sensor

*/

#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include <TaskManager.h>
#include "../network.h"
#include <Wire.h>
#include <ph4502c_sensor.h>
#include "def.h"

extern Network *_network;

namespace Services
{

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
        }

        pH_measurement *init(const uint8_t DallasPin)
        {
            LOGGER_VERBOSE("enter ...");
            ph4502c.init();
            LOGGER_VERBOSE("leave ...");
            return this;
        }

        virtual void update() override
        {
            // // Read the temperature from the sensor
            Serial.println("Temperature reading:" + String(ph4502c.read_temp()));

            // // Read the pH level by average
            Serial.println("pH Level Reading-new: " + String(ph4502c.read_ph_level())); // P0 ~ A0

            // // Read a single pH level value
            Serial.println("pH Level Single Reading: " + String(ph4502c.read_ph_level_single()));

            delay(1000);

            // int sensorValue = analogRead(A0);
            // float spannung = sensorValue*(5,0/1023);
            // Serial.print("sensorValue - ");Serial.println(sensorValue);
            // Serial.print("Spannung - ");Serial.println(spannung);
        }
    };
} // end of namespace Services