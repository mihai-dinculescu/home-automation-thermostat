#ifndef CONFIG_SECRETS_H
    #define CONFIG_SECRETS_H

    #include <stdint.h>

    struct ConfigSecrets {
        const char         *wifi_ssid            = "<wifi_ssid>";
        const char         *wifi_password        = "<wifi_password>";

        const uint16_t     thermostat_id         = 0;
    };

    extern ConfigSecrets config_secrets;
#endif
