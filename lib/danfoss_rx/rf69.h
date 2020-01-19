#ifndef RF69_H
    #define RF69_H

    #include <RH_RF69.h>
    #include "config.h"

    class Radio
    {
        RH_RF69 rf69;
        uint8_t pin_cs;
        uint8_t pin_int;
        uint8_t pin_rst;

        public:
            void rf69_init(uint8_t sync_len, uint8_t sync_tol, const uint8_t *sync_val, uint8_t recv_packet_len);
            bool rf69_receiveDone(uint8_t *out, uint8_t *sz);
            void rf69_transmit(const uint8_t *data, size_t len, bool no_sync);
            Radio(uint8_t pin_cs, uint8_t pin_int, uint8_t pin_rst): rf69(pin_cs, pin_int), pin_cs(pin_cs), pin_int(pin_int), pin_rst(pin_rst) {}

        private:
            Radio() {}
    };
#endif
