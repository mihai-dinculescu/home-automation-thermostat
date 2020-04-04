#include "sensor_rtc.h"
#include "MAD_ESP32.h"

bool SensorRTC::Setup()
{
    if (!_sensor.begin()) {
        return false;
    }

    if (_sensor.lostPower() || _sensor.alarmFired(1)) {
        if (_sensor.lostPower()) {
            LOGLNT("RTC lost power.");
        } else {
            LOGLNT("RTC refresh alarm fired.");
        }

        if (!board.SetupTime()) {
            return false;
        }

        // set sensor datetime
        _sensor.adjust(DateTime(board.GetTimestamp() / 1000));

        // set alarm for when next to update the time from NTP
        _sensor.clearAlarm(1);
        _sensor.setAlarm1(_sensor.now() + TimeSpan(1, 0, 0, 0), Ds3231Alarm1Mode::DS3231_A1_Minute);
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
