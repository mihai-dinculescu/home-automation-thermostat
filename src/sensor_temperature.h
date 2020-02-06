#ifndef SENSOR_TEMPERATURE_H
    #define SENSOR_TEMPERATURE_H

    #include <Adafruit_MCP9808.h>

    class SensorTmperature
    {
        Adafruit_MCP9808 _sensor;

        public:
            bool Setup();
            float Read();
    };

    extern SensorTmperature sensor_temperature;
#endif
