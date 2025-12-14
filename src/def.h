/*
    File name: def.h
    Date: 2024.11.14
    Author: Wilhelm Kuckelsberg
    Description: Definition of PINs and constants

    Usable PINs
    D1  GPIO5
    D2  GPIO4
    D3  GPIO0
    D5  GPIO14
    D6  GPIO12
    D7  GPIO13
    A0  ADC0
*/

#define DEBUG_MESSAGE_BUFFER_SIZE 200
#define GLOBAL_BUFFER_SIZE 100

#define PORT_FOR_PH 4002

/* Pinout: https://cdn.awsli.com.br/969/969921/arq uivos/ph-sensor-ph-4502c.pdf */
#define PH4502C_TEMPERATURE_PIN 4
#define PH4502C_PH_PIN A0
#define PH4502C_PH_TRIGGER_PIN 5
#define PH4502C_CALIBRATION 14.8f
#define PH4502C_READING_INTERVAL 100
#define PH4502C_READING_COUNT 10
// NOTE: The ESP32 ADC has a 12-bit resolution (while most arduinos have 10-bit)
#define ADC_RESOLUTION 1024.0f

