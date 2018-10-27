/*
  ttn.h

  Convenience TTN class to handle ABP after OTAA

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

#ifndef ttn_h
#define ttn_h

#include "fram.h"
#include <MKRWAN.h>

const uint8_t resetPin  = 1;        // Reset button

// Data struture for session keys and frame counters
const uint8_t devAddrSize = 8;
const uint8_t sKeySize = 32;
struct ttnData_t {
  uint8_t   ver;                    // Data version
  char      devAddr[devAddrSize];   // Device address
  char      nwkSKey[sKeySize];      // Network Session Key
  char      appSKey[sKeySize];      // Application Session Key
  uint16_t  fcu;                    // Frame counter uplink
  uint16_t  fcd;                    // Frame counter downlink
  uint8_t   port;                   // Port to use for sending data
  uint8_t   dataRate;               // Data Rate for sending data
};
const uint32_t ttnDataVer = 2;      // Struct data version

// Delay for polling send queue
const uint8_t pollPerSec = 5;

// Default LoraWan port and dataRate
const uint8_t ttnPort = 4;
const uint8_t ttnDataRate = 5;


class TTN: public LoRaModem, public FRAM
{
  private:
    ttnData_t ttnData;

  public:
    TTN ();
    boolean begin ();
    void join();
    void sendData(uint8_t* data, uint16_t dataSize);
    void saveKeys();

  private:
    void joinOTAA();
    void joinABP();
    bool getKeys();
};

#endif //ttn_h
