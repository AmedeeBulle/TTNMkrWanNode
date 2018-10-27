/*
  fram.h

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

#ifndef fram_h
#define fram_h

#include <SPI.h>
#include <Adafruit_FRAM_SPI.h>

struct framHeader_t {
  uint32_t  magic;                  // Magic number
  uint16_t  crc;                    // Checksum
};

const uint32_t  framMagic       = 0x13040902;   // Magic cookie
const uint8_t   csFramPin       = 7;            // SPI CS for the FRAM module
const uint8_t   addrSizeInBytes = 2;            // Address size for the FRAM is 2 bytes
const uint32_t  framAddr        = 0;            // Address for the data structure

class FRAM: public Adafruit_FRAM_SPI
{
  public:
    FRAM ();
    boolean begin ();
    boolean read(uint8_t* framData, uint16_t framSize);
    void write(uint8_t* framData, uint16_t framSize);

  private:
    uint16_t crc16(uint8_t* data, uint16_t len);
};

#endif //fram_h
