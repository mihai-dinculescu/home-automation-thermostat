#include <sstream>
#include <iostream>
#include <string>

#include "config.h"
#include "MAD_ESP32.h"
#include "messaging.h"
#include "thermostat.h"
#include "sensor_temperature.h"

Thermostat *thermostat;
RTC_DATA_ATTR bool previously_run = false;

const char* GenerateMessage(const uint16_t thermostat_id, const char command_key)
{
    int16_t command = 0;
    std::ostringstream messageStream;

    switch(command_key) {
        case 'X':
            command = 0;
        break;
        case 'O':
            command = 1;
        break;
        default:
            command = -1;
        break;
    }

    messageStream << "{";
    messageStream << "\"thermostat_id\":" << thermostat_id;
    messageStream << ",\"command\":" << command;
    messageStream << ",\"temperature_current\":" << thermostat->GetTemperatureCurrent();
    messageStream << ",\"temperature_target\":" << thermostat->GetTemperatureTarget();
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

    if (sensor_temperature.Setup()) {
        LOGLNT("Temperature sensor init OK.");
    } else {
        LOGLNT("Temperature sensor init failed!");
        board.FatalError();
    }

    if (!board.SetupWifi(config.wifi_ssid, config.wifi_password)) {
        Restart("WiFi connect timeout.");
    }

    WiFi.onEvent(WiFiEventHandler);

    if (!board.SetupTime()) {
        Restart("Time fetch timeout.");
    }

    SetupMQTT(config.mqtt_broker);

    thermostat = new Thermostat(previously_run, config.rfm69_cs, config.rfm69_int, config.rfm69_rst);

    if (thermostat->Init()) {
	    LOGLNT("RFM69 radio init OK.");
    } else {
		LOGLNT("RFM69 radio init failed!");
		board.FatalError();
    }

    LOGLNT("Danfoss thermostat transceiver init OK.");
}

void loop()
{
    mqtt_client.loop();
    delay(10);

    bool started = thermostat->HandleThermostat();

    if (started) {
        if (!ConnectMQTT(config.mqtt_client_id)) {
            Restart("MQTT connect timeout.");
        }

        const char* message = GenerateMessage(config.thermostat_id, 'O');

        if (!PublishMessage(config.mqtt_topic, message)) {
            Restart("MQTT publish failed.");
        }

        previously_run = true;
        board.DeepSleep(2 * 60);
    } else {
        if (!ConnectMQTT(config.mqtt_client_id)) {
            Restart("MQTT connect timeout.");
        }

        const char* message = GenerateMessage(config.thermostat_id, 'X');

        if (!PublishMessage(config.mqtt_topic, message)) {
            Restart("MQTT publish failed.");
        }

        previously_run = false;
        board.DeepSleep(5 * 60);
    }
}
