# Home Automation - Thermostat

# Configuration
Rename and update `config_secrets_sample.h` to `config_secrets.h`.
More settings can be changed in `config.h`.

# Remote Configuration
A remote API (Rust + GraphQL + Docker) exists at [https://github.com/mihai-dinculescu/home-automation-config](https://github.com/mihai-dinculescu/home-automation-config). It controls the status (on or off) of the thermostat.

# Setup
- BME280
- Adafruit MicroSD Card Breakout Board
- Adafruit RFM69HCW Transceiver Radio Breakout - 433 MHz
- Adafruit DS3231 Precision RTC Breakout
