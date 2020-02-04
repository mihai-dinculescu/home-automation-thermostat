#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "config_remote.h"
#include "MAD_ESP32.h"

ConfigRemote config_remote;

void ConfigRemote::Read(const char *config_url)
{
    HTTPClient http_client;

    http_client.begin(config_url);

    int response_code = http_client.GET();

    if(response_code > 0) {
        String result = http_client.getString();

        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 120 * 3;
        StaticJsonDocument<capacity> doc;

        DeserializationError error = deserializeJson(doc, result);

        if (error) {
            LOGLNT("deserializeJson() failed: %s!", error.c_str());
        }

        _field1 = doc["feeds"][0]["field1"];
    } else {
        LOGLNT("Error in getting config: %d!", response_code);
    }
}

uint16_t ConfigRemote::GetField1()
{
    return _field1;
}
