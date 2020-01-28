#include "sensor_temperature.h"

bool SensorTmperature::Setup()
{
    return bme.begin(BME280_ADDRESS_ALTERNATE, &Wire);
}

float SensorTmperature::ReadTemperature()
{
    return bme.readTemperature();
}

SensorTmperature sensor_temperature;
