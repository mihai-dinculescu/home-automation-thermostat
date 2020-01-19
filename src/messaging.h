#ifndef MESSAGING_H
    #define MESSAGING_H

    #include <MQTT.h>

    extern MQTTClient mqtt_client;

    void SetupMQTT();
    void LogData(const uint16_t thermostat_id, const uint8_t command_key);
#endif
