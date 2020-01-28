#ifndef CONFIG_H
    #define CONFIG_H

    #include <stdint.h>

    #include "config_secrets.h"
    #include "MAD_ESP32.h"

    struct Config {
        const char           *wifi_ssid              = config_secrets.wifi_ssid;
        const char           *wifi_password          = config_secrets.wifi_password;
        const char           *mqtt_broker            = "192.168.1.100";

        const char           *mqtt_location          = "Living Room";
        const char           *mqtt_client_id         = "mqtt_client_boiler_transceiver";
        const char           *mqtt_topic             = "boilers/main";

        const uint16_t       *rfm69_cs               = &board.pins.A1;
        const uint16_t       *rfm69_rst              = &board.pins.A0;
        const uint16_t       *rfm69_int              = &board.pins.P21;

        const uint16_t       thermostat_id           = config_secrets.thermostat_id;
    };

    extern Config config;
#endif
