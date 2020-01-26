#ifndef MESSAGING_H
    #define MESSAGING_H

    #include <MQTT.h>

    extern MQTTClient mqtt_client;

    void SetupMQTT(const char *mqtt_broker);
    bool ConnectMQTT(const char *client_id);
    bool PublishMessage(const char* mqtt_topic, const char* message);
#endif
