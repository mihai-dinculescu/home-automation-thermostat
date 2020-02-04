#include "messaging.h"
#include "MAD_ESP32.h"

Messaging messaging(256);

void Messaging::Setup(const char *mqtt_broker)
{
    _client.begin(mqtt_broker, board.wifi_client);
    LOGLNT("MQTT init done.");
}

bool Messaging::Connect(const char *client_id)
{
    LOGT("Connecting to MQTT.");

    uint64_t time_ms = millis();
    while (!_client.connect(client_id)) {
        if (millis() >= time_ms + 10 * 1000) {
            return false;
        }
        LOG(".");
        delay(500);
    }

    LOGLN(" CONNECTED.");
    return true;
}

bool Messaging::Publish(const char* mqtt_topic, const char* message)
{
    LOGT("Publishing to MQTT.");
    bool result = _client.publish(mqtt_topic, message, false, 2);
    LOGLN(" PUBLISHED with status %d.", _client.lastError());

    return result;
}

bool Messaging::Loop()
{
    return _client.loop();
}
