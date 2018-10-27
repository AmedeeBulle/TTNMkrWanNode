/*
  fram.cpp

  Simple FRAM class to abstract key save / restore

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "fram.h"

#include <SPI.h>
#include <Adafruit_FRAM_SPI.h>

FRAM::FRAM () : Adafruit_FRAM_SPI() {
}

boolean FRAM::begin () {
  Adafruit_FRAM_SPI::begin(csFramPin, addrSizeInBytes);
}

boolean FRAM::read(uint8_t* framData, uint16_t framSize) {
  framHeader_t framHeader;

  Adafruit_FRAM_SPI::read(framAddr, (uint8_t*)&framHeader, sizeof(framHeader));
  if (framHeader.magic != framMagic) {
    return false;
  }
  Adafruit_FRAM_SPI::read(framAddr + sizeof(framHeader), framData, framSize);
  return (crc16(framData, framSize) == framHeader.crc);
}

void FRAM::write(uint8_t* framData, uint16_t framSize) {
  framHeader_t framHeader;

  framHeader.magic = framMagic;
  framHeader.crc = crc16(framData, framSize);

  Adafruit_FRAM_SPI::writeEnable(true);
  Adafruit_FRAM_SPI::write(framAddr, (uint8_t*)&framHeader, sizeof(framHeader));
  // Consecutive writes don't work
  Adafruit_FRAM_SPI::writeEnable(false);
  Adafruit_FRAM_SPI::writeEnable(true);
  Adafruit_FRAM_SPI::write(framAddr + sizeof(framHeader), framData, framSize);
  Adafruit_FRAM_SPI::writeEnable(false);
}

uint16_t FRAM::crc16(uint8_t* data, uint16_t len) {
  // CCITT 16 bits CRC
  // https://www.carnetdumaker.net/articles/les-sommes-de-controle/
  uint16_t crc = 0xFFFF;

  if (len == 0) {
    return 0;
  }

  for (uint16_t i = 0; i < len; i++) {
    uint16_t dbyte = data[i];
    crc ^= dbyte << 8;

    for (uint8_t j = 0; j < 8; j++) {
      uint16_t mix = crc & 0x8000;
      crc = (crc << 1);
      if (mix) {
        crc = crc ^ 0x1021;
      }
    }
  }

  return crc;
}
