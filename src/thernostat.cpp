#include "thermostat.h"
#include "sensor_temperature.h"

Thermostat::Thermostat(bool previously_run, const uint16_t *rfm69_cs, const uint16_t *rfm69_int, const uint16_t *rfm69_rst): previously_run(previously_run)
{
    danfoss_rx = new DanfossRX(*config.rfm69_cs, *config.rfm69_int, *config.rfm69_rst);

    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);

    hour = localTime->tm_hour;

    LOGLNT("Hour: %.2d", hour);

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
    if (hour > 7 && hour < 22) {
        return 23;
    }

    return 22;
}

float Thermostat::CalculateTargetTemperature()
{
    if (hour > 7 && hour < 22) {
        return 23;
    }

    return 22;
}

bool Thermostat::ShouldStart()
{
    if (previously_run && temperature_target > temperature_current + 0.5) {
        return true;
    }

    if (!previously_run && temperature_target > temperature_current + 0) {
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
