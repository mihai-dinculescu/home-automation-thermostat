#include <sstream>
#include <iostream>
#include <string>

#include "danfoss_rx.h"

#include "config.h"
#include "MAD_ESP32.h"
#include "messaging.h"

DanfossRX *danfoss_rx;
uint16_t messages_since_restart = 0;

const char* GenerateMessage(const uint16_t thermostat_id, const uint8_t command_key)
{
    int16_t command = 0;
    std::ostringstream messageStream;

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

    messageStream << "{";
    messageStream << "\"thermostat_id\":" << thermostat_id;
    messageStream << ",\"command\":" << command;
    messageStream << ",\"messages_since_restart\":" << ++messages_since_restart;
    messageStream << "}";

    std::string messageString = messageStream.str();
    const char* messageChar = messageString.c_str();

    return messageChar;
}

void Restart(const char *message)
{
    LOGLNT("%s RESTARTING.", message);

    Serial.flush();
    ESP.restart();
}

void WiFiEventHandler(WiFiEvent_t event)
{
    switch (event) {
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Restart("WiFi disconnected.");
            break;
        default:
            break;
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

    if (!board.SetupWifi(config.wifi_ssid, config.wifi_password)) {
        Restart("WiFi connect timeout.");
    }

    WiFi.onEvent(WiFiEventHandler);

    if (!board.SetupTime()) {
        Restart("Time fetch timeout.");
    }

    SetupMQTT(config.mqtt_broker);

    danfoss_rx = new DanfossRX(*config.rfm69_cs, *config.rfm69_int, *config.rfm69_rst);
    if (danfoss_rx->Init()) {
	    LOGLNT("RFM69 radio init OK.");
    } else {
		LOGLNT("RFM69 radio init failed!");
		board.FatalError();
    }

    LOGLNT("Danfoss thermostat transceiver");
}

void loop()
{
    #ifdef TRANSMIT
        while (Serial.available() > 0) {
            int val = Serial.read();
            if (val >= 0) {
                handle_serial_char((char)val);
            }
        }
    #endif

    mqtt_client.loop();
    delay(10);

    uint16_t thermostat_id;
    uint8_t command;

    if (danfoss_rx->receiveDone(&thermostat_id, &command)) {
        if (!ConnectMQTT(config.mqtt_client_id)) {
            Restart("MQTT connect timeout.");
        }

        const char* message = GenerateMessage(thermostat_id, command);

        if (!PublishMessage(config.mqtt_topic, message)) {
            Restart("MQTT publish failed.");
        }
    }
}
