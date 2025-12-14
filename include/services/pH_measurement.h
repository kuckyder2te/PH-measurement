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
        char _msg[30];
        int samples = 10;
        float adc_resolution = 1024.0;

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

        float ph(float voltage)
        {
            return 7 + ((2.5 - voltage) / 0.18);  // Why 0.18
        }

        virtual void calibration()
        {
            int measurings = 0;

            for (int i = 0; i < samples; i++)
            {
                measurings += analogRead(PH4502C_PH_PIN);
                delay(10);
            }

            float voltage = 5 / adc_resolution * measurings / samples;
            Serial.print("pH= ");
            Serial.println(ph(voltage));
        }

        virtual void update() override
        {
            static float _ph_min = 100;
            static float _ph_max = -100;

            // // Read the temperature from the sensor
            LOGGER_NOTICE_FMT("Temperature reading: %.1f", ph4502c.read_temp());
            // // Read the pH level by average
            LOGGER_NOTICE_FMT("pH Level Reading-new: %.1f", ph4502c.read_ph_level()); // P0 ~ A0
                                                                                      // // Read a single pH level value
            LOGGER_NOTICE_FMT("pH Level Single Reading: %.1f", ph4502c.read_ph_level_single());

            float _current_ph = ph4502c.read_temp();

            if (_current_ph < _ph_min)
            {
                _ph_min = _current_ph;
                sprintf(_msg, "{ \"value\":%.1f }", _ph_min);
                _network->pubMsg("outGarden/ph_min", _msg);
                LOGGER_NOTICE_FMT("ph min: %.1f", _ph_min);
            }

            if (_current_ph >= _ph_max)
            {
                _ph_max = _current_ph;
                sprintf(_msg, "{ \"value\":%.1f }", _ph_max);
                _network->pubMsg("outGarden/ph_max", _msg);
                LOGGER_NOTICE_FMT("ph max: %.1f", _ph_max);
            }

            sprintf(_msg, "{ \"value\":%.1f }", _current_ph);
            _network->pubMsg("outGarden/current_ph", _msg);

            // int sensorValue = analogRead(A0);
            // float spannung = sensorValue*(5,0/1023);
            // Serial.print("sensorValue - ");Serial.println(sensorValue);
            // Serial.print("Spannung - ");Serial.println(spannung);
        }
    };
} // end of namespace Services