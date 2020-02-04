#include <sstream>
#include <iostream>
#include <string>

#include "config.h"
#include "config_remote.h"
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

    if (!board.SetupTime()) {
        Restart("Time fetch timeout.");
    }

    messaging.Setup(config.mqtt_broker);

    config_remote.Read(config.config_url);
    uint16_t enabled = config_remote.GetField1();

    if (enabled == 1) {
        LOGLNT("Thermostat is ENABLED.");
    } else {
        LOGLNT("Thermostat is DISABLED.");
    }

    thermostat = new Thermostat(enabled, previously_run, config.rfm69_cs, config.rfm69_int, config.rfm69_rst);

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
    bool should_start = thermostat->HandleThermostat();

    if (messaging.Connect(config.mqtt_client_id)) {
        const char* message = GenerateMessage(config.thermostat_id, should_start ? 'O': 'X');
        messaging.Publish(config.mqtt_topic, message);
    }

    previously_run = should_start;

    board.DeepSleep(thermostat->GetSleepTime());
}
