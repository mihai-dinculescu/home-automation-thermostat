#ifndef SENSOR_RTC_H
    #define SENSOR_RTC_H

    #include <RTClib.h>

    class SensorRTC
    {
        RTC_DS3231 _sensor;

        public:
            bool Setup();
            float Read();
    };

    extern SensorRTC sensor_rtc;
#endif
