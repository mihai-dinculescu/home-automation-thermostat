#ifndef SENSORS_BME280_H
    #define SENSORS_BME280_H

    #include <Adafruit_BME280.h>

    class SensorTmperature
    {
        Adafruit_BME280 bme;

        public:
            bool Setup();
            float ReadTemperature();
    };

    extern SensorTmperature sensor_temperature;
#endif
