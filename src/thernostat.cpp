#include "thermostat.h"
#include "sensor_temperature.h"

Thermostat::Thermostat(bool previously_run, const uint16_t *rfm69_cs, const uint16_t *rfm69_int, const uint16_t *rfm69_rst): previously_run(previously_run)
{
    danfoss_rx = new DanfossRX(*config.rfm69_cs, *config.rfm69_int, *config.rfm69_rst);

    temperature_current = sensor_temperature.ReadTemperature();
    temperature_target = CalculateTargetTemperature();
    LOGLNT("Current Temperature: %.2f", temperature_current);
    LOGLNT("Target Temperature: %.2f", temperature_target);
    LOGLNT("Previously Run: %s", previously_run ? "true" : "false");
}

bool Thermostat::Init()
{
    return danfoss_rx->Init();
}

float Thermostat::GetTemperatureCurrent()
{
    return temperature_current;
}

float Thermostat::GetTemperatureTarget()
{
    return temperature_target;

}

float Thermostat::CalculateTargetTemperature()
{
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);

    uint16_t month = localTime->tm_mon + 1;
    uint16_t hour = localTime->tm_hour;

    LOGLNT("Month: %.2d", month);
    LOGLNT("Hour: %.2d", hour);

    // default target - night time
    float target = 21.5;

    // day time
    if (hour >= 7 && hour < 21) {
        target = 22.5;
    }

    // warm period adjustment
    if (month >= 3 && month < 10) {
        target -= 1;
    }

    return target;
}

bool Thermostat::ShouldStart()
{
    if (previously_run && temperature_target >= temperature_current + 0.2) {
        return true;
    }

    if (!previously_run && temperature_target >= temperature_current) {
        return true;
    }

    return false;
}

bool Thermostat::HandleThermostat()
{
    if (ShouldStart()) {
        LOGLNT("Thermostat should START.");

        danfoss_rx->IssueCommand('O', config.thermostat_id);

        return true;
    } else {
        LOGLNT("Thermostat should STOP.");

        danfoss_rx->IssueCommand('X', config.thermostat_id);

        return false;
    }
}

uint32_t Thermostat::GetSleepTime()
{
    if (previously_run || ShouldStart()) {
        return 2 * 60;
    }

    return 5 * 60;
}
