#include "sensor_temperature.h"

bool SensorTmperature::Setup()
{
    bool result = _sensor.begin(MCP9808_I2CADDR_DEFAULT);
    _sensor.setResolution(3);

    return result;
}

float SensorTmperature::Read()
{
    _sensor.wake();
    float value = _sensor.readTempC();
    _sensor.shutdown_wake(1);

    return value;
}

SensorTmperature sensor_temperature;
