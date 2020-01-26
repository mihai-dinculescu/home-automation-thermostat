#include "messaging.h"
#include "MAD_ESP32.h"

MQTTClient mqtt_client(256);

void SetupMQTT(const char *mqtt_broker)
{
    mqtt_client.begin(mqtt_broker, board.wifi_client);
    LOGLNT("MQTT init done.");
}

bool ConnectMQTT(const char *client_id)
{
    LOGT("Connecting to MQTT.");

    uint64_t time_ms = millis();
    while (!mqtt_client.connect(client_id)) {
        if (millis() >= time_ms + 10 * 1000) {
            return false;
        }
        LOG(".");
        delay(500);
    }

    LOGLN(" CONNECTED.");
    return true;
}

bool PublishMessage(const char* mqtt_topic, const char* message)
{
    LOGT("Publishing to MQTT.");
    bool result = mqtt_client.publish(mqtt_topic, message, false, 2);
    LOGLN(" PUBLISHED with status %d.", mqtt_client.lastError());

    return result;
}
