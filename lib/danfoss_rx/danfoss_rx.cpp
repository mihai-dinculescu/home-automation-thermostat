#include "danfoss_rx.h"

const uint8_t DanfossRX::sync_val[] = { 0x6c, 0xb6, 0xcb, 0x2c, 0x92, 0xd9 };

DanfossRX::DanfossRX(uint8_t rfm69_cs, uint8_t rfm69_int, uint8_t rfm69_rst): rfm69_cs(rfm69_cs), rfm69_int(rfm69_int), rfm69_rst(rfm69_rst)
{
    radio = new Radio(rfm69_cs, rfm69_int, rfm69_rst);
}

bool DanfossRX::Init()
{
    return radio->rf69_init(sizeof sync_val, 27, sync_val, 2);
}

DanfossRX::~DanfossRX()
{
    free(radio);
}

void DanfossRX::Decode3b(const uint8_t *in, uint8_t *out, size_t outsz)
{
    unsigned int i;
    memset(out, 0, outsz);
    for (i = 0; i < outsz * 8; i++) {
        unsigned int inbit = 1 + i * 3;
        uint8_t val = (in[inbit / 8] & (1 << (7 - (inbit % 8)))) ? 1 : 0;
        out[i / 8] |= val << (7 - (i % 8));
    }
}

bool DanfossRX::ReceiveDone(uint16_t *thermostat_id, uint8_t *command) {
    uint8_t data[30];
    uint8_t data_sz = sizeof data;

    if (radio->rf69_receiveDone(data, &data_sz)) {
        uint8_t decoded[9];

        Decode3b(data, decoded, 9);

        /* Now decode it: check for message consistency */
        if (decoded[3] != 0xAA ||
            decoded[4] != 0xDD ||
            decoded[5] != 0x46 ||
            decoded[0] != decoded[6] ||
            decoded[1] != decoded[7]) {
            return false;
        }

        /* Check the thermostat ID */
        *thermostat_id = (decoded[1] << 8) | decoded[0];
        *command = decoded[2];

        Serial.print("RECV 0x");
        Serial.print(*thermostat_id, HEX);

        if (decoded[2] == command_off) {
            Serial.println(" OFF");
        } else if (decoded[2] == command_learn) {
            Serial.println(" LEARN");
        } else if (decoded[2] == command_on) {
            Serial.println(" ON");
        }

        return true;
    }

    return false;
}

/*
 * Packet definition and construction
 */
const unsigned char DanfossRX::thermostat_packet[] = {  0xAA, 0xDD, 0x46, 0x00, 0x00, 0x00 };

void DanfossRX::Encode3b(const uint8_t *in, uint8_t *out, size_t insz)
{
    unsigned int i;
    memset(out, 0, insz * 3);
    for (i = 0; i < insz * 8; i++) {
        unsigned int inval = in[i / 8] & (1 << (7 - (i % 8))) ? 1 : 0;
        unsigned int outbit = i * 3;
        /* Set out-value to 011 or 001: first bit already cleared,
        * last bit always 1: */
        out[(outbit + 1) / 8] |= inval << (7 - ((outbit + 1) % 8));
        out[(outbit + 2) / 8] |= 1 << (7 - ((outbit + 2) % 8));
    }
}

/* out must be an array of length at least 3 * (sizeof thermostat_packet) */
void DanfossRX::MakeDanfossPacket(uint8_t *out, unsigned int thermid, uint8_t cmd)
{
    uint8_t packet[sizeof thermostat_packet];
    memcpy(packet, thermostat_packet, sizeof packet);
    packet[thermostat_id1_pos] = thermid & 0xff;
    packet[thermostat_id2_pos] = (thermid & 0xff00) >> 8;
    packet[command_pos] = cmd;
    Encode3b(packet, out, sizeof packet);
}

/* Command is O for on, X for off, L for learn. */
void DanfossRX::IssueCommand(char command, unsigned int thermid)
{
    uint8_t packet[6 * sizeof thermostat_packet];
    uint8_t packet_cmd = command == 'O' ? command_on
                        : command == 'X' ? command_off
                        : command == 'L' ? command_learn : 0;
    MakeDanfossPacket(packet, thermid, packet_cmd);
    /* Transmit two copies back-to-back */
    memcpy(&packet[3 * sizeof thermostat_packet], packet, 3 * sizeof thermostat_packet);
    radio->rf69_transmit(packet, sizeof packet, true);
}
