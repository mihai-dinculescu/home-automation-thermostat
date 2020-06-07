#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "config_remote.h"
#include "MAD_ESP32.h"

ConfigRemote config_remote;

void ConfigRemote::Read(const char *config_key, const char *config_url)
{
    HTTPClient http_client;

    http_client.begin(config_url);
    http_client.addHeader("key", config_key);

    int response_code = http_client.GET();

    if(response_code > 0) {
        String result = http_client.getString();

        const size_t capacity = 3 * JSON_OBJECT_SIZE(1) + 500;
        StaticJsonDocument<capacity> doc;

        DeserializationError error = deserializeJson(doc, result);

        if (error) {
            LOGLNT("Config deserializeJson() failed: %s!", error.c_str());
        }

        JsonVariant graphql_error = doc["errors"][0]["message"];

        if (graphql_error) {
            LOGLNT("Config GraphQL error: %s!", graphql_error.as<char*>());
        } else {
            _status = doc["data"]["thermostatStatus"]["status"].as<bool>();
        }
    } else {
        LOGLNT("Error in getting config: %d!", response_code);
    }
}

uint16_t ConfigRemote::GetStatus()
{
    return _status;
}
