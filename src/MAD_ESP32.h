#ifndef MAD_ESP32_H
    #define MAD_ESP32_H

    #include <stdint.h>
    #include <WiFi.h>

    struct Pins {
        const uint16_t LED_ERROR = 13;
        const uint16_t A0 = 26;
        const uint16_t A1 = 25;
        const uint16_t P21 = 21;
        const uint16_t I2C_SDA = 23;
        const uint16_t I2C_SCL = 22;
    };

    class Board
    {
        public:
            const Pins pins;
            WiFiClient wifi_client;
            void SetupWifi(const char *ssid, const char *password);
            void SetupTime();
            int64_t GetTimestamp();
            void FatalError();
            void DeepSleep(uint16_t seconds);

        private:
            void BlinkErrorLed(uint16_t interval);
    };

    extern Board board;

    #define LOG(fmt, ...) (Serial.printf(fmt, ##__VA_ARGS__))
    #define LOGLN(fmt, ...) (Serial.printf("%09llu: " fmt "\n", board.GetTimestamp(), ##__VA_ARGS__))
#endif
