#ifndef DANFOSS_RX_H
    #define DANFOSS_RX_H

    #include "rf69.h"

    class DanfossRX
    {
        static const uint8_t sync_val[];

        /* Note that these positions applied to packets with the sync word included,
        * which isn't the case for those received over RF. */
        static const uint16_t thermostat_id1_pos = 3;
        static const uint16_t thermostat_id2_pos = 4;
        static const uint16_t command_pos = 5;

        Radio *radio;

        uint8_t rfm69_cs;
        uint8_t rfm69_int;
        uint8_t rfm69_rst;

        public:
            static const uint8_t command_on = 0x33;
            static const uint8_t command_off= 0xcc;
            static const uint8_t command_learn= 0x77;

            bool receiveDone(uint16_t *thermostat_id, uint8_t *command);
            DanfossRX(uint8_t rfm69_cs, uint8_t rfm69_int, uint8_t rfm69_rst);
            ~DanfossRX();

        private:
            static void decode_3b(const uint8_t *in, uint8_t *out, size_t outsz);
            DanfossRX() {}
    };
#endif
