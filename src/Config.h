#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

struct ConfigData {
  char ssid[32];        // max 31 + null
  char password[64];    // max 63 + null
  char deviceId[16];    // max 15 + null
};

class Config {
  public:
    Config();

    ConfigData data;

    bool save();
    bool load();
    void erase();

  private:
    static constexpr int SSID_ADDR      = 0;
    static constexpr int PASS_ADDR      = SSID_ADDR + 32;
    static constexpr int DEVICE_ID_ADDR = PASS_ADDR + 64;
    static constexpr int CRC_ADDR       = DEVICE_ID_ADDR + 16;

    uint8_t calculateCRC();
};

extern Config config;

#endif
