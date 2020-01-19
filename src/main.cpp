#include "danfoss_rx.h"

#include "config.h"
#include "MAD_ESP32.h"
#include "messaging.h"

// WiFiEventHandler disconnectedEventHandler;

DanfossRX *danfoss_rx;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
    Serial.println("");

    board.SetupWifi(config.wifi_ssid, config.wifi_password);
    SetupMQTT();

    // disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
    // {
    //     Serial.printf("WiFi disconnected %d! Restarting...\n", event.reason);
    //     Serial.flush();
    //     ESP.restart();
    // });

    danfoss_rx = new DanfossRX(*config.rfm69_cs, *config.rfm69_int, *config.rfm69_rst);

    Serial.println("Danfoss thermostat transceiver");
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
        LogData(thermostat_id, command);
    }
}
