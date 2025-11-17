#include "Config.h"

Config config;

Config::Config() {}

bool Config::save() {
  // Write SSID
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, data.ssid[i]);
  }

  // Write Password
  for (int i = 0; i < 64; i++) {
    EEPROM.write(PASS_ADDR + i, data.password[i]);
  }

  // Write Device ID
  for (int i = 0; i < 16; i++) {
    EEPROM.write(DEVICE_ID_ADDR + i, data.deviceId[i]);
  }

  // Write CRC of everything except CRC byte
  uint8_t crc = calculateCRC();
  EEPROM.write(CRC_ADDR, crc);

  return true;
}

bool Config::load() {
  // Read SSID
  for (int i = 0; i < 32; i++) {
    data.ssid[i] = EEPROM.read(SSID_ADDR + i);
  }
  data.ssid[31] = '\0';

  // Read Password
  for (int i = 0; i < 64; i++) {
    data.password[i] = EEPROM.read(PASS_ADDR + i);
  }
  data.password[63] = '\0';

  // Read Device ID
  for (int i = 0; i < 16; i++) {
    data.deviceId[i] = EEPROM.read(DEVICE_ID_ADDR + i);
  }
  data.deviceId[15] = '\0';

  // Validate CRC
  return EEPROM.read(CRC_ADDR) == calculateCRC();
}

void Config::erase() {
  for (int i = 0; i < 32; i++) EEPROM.write(SSID_ADDR + i, 0);
  for (int i = 0; i < 64; i++) EEPROM.write(PASS_ADDR + i, 0);
  for (int i = 0; i < 16; i++) EEPROM.write(DEVICE_ID_ADDR + i, 0);

  EEPROM.write(CRC_ADDR, 0);
}

uint8_t Config::calculateCRC() {
  uint8_t crc = 0;

  // SSID
  for (int i = 0; i < 32; i++) crc ^= EEPROM.read(SSID_ADDR + i);
  // PASS
  for (int i = 0; i < 64; i++) crc ^= EEPROM.read(PASS_ADDR + i);
  // DEVICE ID
  for (int i = 0; i < 16; i++) crc ^= EEPROM.read(DEVICE_ID_ADDR + i);

  return crc;
}
