#ifndef MESSAGING_H
    #define MESSAGING_H

    #include <MQTT.h>

    class Messaging
    {
        MQTTClient _client;

        public:
            Messaging(uint16_t bufSize = 128): _client(bufSize) {};
            void Setup(const char *mqtt_broker);
            bool Connect(const char *client_id);
            bool Publish(const char* mqtt_topic, const char* message);
            bool Loop();
    };

    extern Messaging messaging;
#endif
