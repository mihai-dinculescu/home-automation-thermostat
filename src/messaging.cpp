#include <sstream>

#include "messaging.h"
#include "config.h"
#include "MAD_ESP32.h"
#include "danfoss_rx.h"

MQTTClient mqtt_client(128);

uint16_t messages_since_restart = 0;

void SetupMQTT()
{
    mqtt_client.begin(config.mqtt_broker, board.wifi_client);
    LOGLN("MQTT init done.");

    if (!mqtt_client.connected()) {
        LOG("Connecting to MQTT.");
        while (!mqtt_client.connect(config.mqtt_client_id)) {
            LOG(".");
            delay(1000);
        }
        LOG(" CONNECTED.\n");
    }
}

void LogData(const uint16_t thermostat_id, const uint8_t command_key)
{
    int16_t command = 0;

    switch(command_key) {
        case DanfossRX::command_off:
            command = 0;
        break;
        case DanfossRX::command_on:
            command = 1;
        break;
        case DanfossRX::command_learn:
            command = 2;
        break;
        default:
            command = -1;
        break;
    }

    std::ostringstream messageStream;

    messageStream << "{";
    messageStream << "\"thermostat_id\":" << thermostat_id;
    messageStream << ",\"command\":" << command;
    messageStream << ",\"messages_since_restart\":" << ++messages_since_restart;
    messageStream << "}";

    if (mqtt_client.publish(config.mqtt_topic, messageStream.str().c_str(), false, 2)) {
        Serial.printf("Message #%d published to MQTT. Last error: %d.\n", messages_since_restart, mqtt_client.lastError());
    } else {
        Serial.printf("Error: %d.RESTARTING.\n", mqtt_client.lastError());
        Serial.flush();
        ESP.restart();
    }
}
