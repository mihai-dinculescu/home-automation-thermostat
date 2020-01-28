#ifndef THERMOSTAT_H
    #define THERMOSTAT_H

    #include "danfoss_rx.h"

    class Thermostat
    {
        DanfossRX *danfoss_rx;
        bool previously_run;
        uint16_t hour;
        float temperature_current;
        float temperature_target;

        public:
            Thermostat(bool previously_run, const uint16_t *rfm69_cs, const uint16_t *rfm69_int, const uint16_t *rfm69_rst);
            bool Init();
            bool HandleThermostat();
            float GetTemperatureCurrent();
            float GetTemperatureTarget();

        private:
            bool ShouldStart();
            float CalculateTargetTemperature();
    };
#endif
