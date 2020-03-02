#include "sensor_rtc.h"
#include "MAD_ESP32.h"

bool SensorRTC::Setup()
{
    if (!_sensor.begin()) {
        return false;
    }

    if (_sensor.lostPower()) {
        LOGLNT("RTC lost power.");

        if (!board.SetupTime()) {
            return false;
        }

        _sensor.adjust(DateTime(board.GetTimestamp() / 1000));
    } else {
        LOGT("Setting time from RTC.");

        int epoch_time = _sensor.now().unixtime();
        timeval epoch = {epoch_time, 0};
        const timeval *tv = &epoch;

        settimeofday(tv, NULL);

        struct tm time_info;

        getLocalTime(&time_info);

        char datetime_buffer[128];
        strftime(datetime_buffer, sizeof(datetime_buffer), "%A, %B %d %Y %H:%M:%S", &time_info);

        LOGLN(" Current time is: %s.", datetime_buffer);
    }

    return true;
}

SensorRTC sensor_rtc;
