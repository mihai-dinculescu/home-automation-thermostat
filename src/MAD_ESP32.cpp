#include <sys/time.h>

#include "MAD_ESP32.h"

int64_t Board::GetTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

bool Board::SetupWifi(const char *ssid, const char *password)
{
    LOGT("Connecting to %s.", ssid);
    WiFi.begin(ssid, password);

    uint64_t time_ms = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() >= time_ms + 30 * 1000) {
            return false;
        }
        LOG(".");
        delay(500);
    }

    LOGLN(" CONNECTED.");
    return true;
}

bool Board::SetupTime()
{
    configTime(0, 3600, "pool.ntp.org", "time.windows.com", "time.nist.gov");
    struct tm time_info;

    LOGT("Getting time.");
    uint16_t retries = 1;
    while (!getLocalTime(&time_info)) {
        if (retries >= 3) {
            return false;
        }

        configTime(0, 0, "pool.ntp.org", "time.windows.com", "time.nist.gov");

        LOG(".");
        retries++;
    }

    char datetime_buffer[128];
    strftime(datetime_buffer, sizeof(datetime_buffer), "%A, %B %d %Y %H:%M:%S", &time_info);

    LOGLN(" Current time is: %s.", datetime_buffer);
    return true;
}

void Board::BlinkErrorLed(uint16_t interval)
{
    pinMode(pins.LED_ERROR, OUTPUT);
    digitalWrite(pins.LED_ERROR, HIGH);
    delay(interval);
    digitalWrite(pins.LED_ERROR, LOW);
    delay(interval);
}

void Board::FatalError()
{
    while(true) {
        BlinkErrorLed(100);
    }
}

void Board::DeepSleep(uint16_t seconds)
{
    uint64_t time_us = seconds * 1000 * 1000ull - esp_timer_get_time();
    LOGLNT("Deep sleep for %llu ms!", time_us / 1000);
    esp_sleep_enable_timer_wakeup(time_us);
    esp_deep_sleep_start();
}

Board board;
