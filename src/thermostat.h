#ifndef THERMOSTAT_H
    #define THERMOSTAT_H

    #include "danfoss_rx.h"

    class Thermostat
    {
        DanfossRX *danfoss_rx;
        bool enabled;
        bool previously_run;
        float temperature_current;
        float temperature_target;

        public:
            Thermostat(bool enabled, bool previously_run, const uint16_t *rfm69_cs, const uint16_t *rfm69_int, const uint16_t *rfm69_rst);
            bool Init();
            bool HandleThermostat();
            float GetTemperatureCurrent();
            float GetTemperatureTarget();
            uint32_t GetSleepTime();

        private:
            bool ShouldStart();
            float CalculateTargetTemperature();
    };
#endif
