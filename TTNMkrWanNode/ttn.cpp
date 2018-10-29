/*
  ttn.cpp

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

#include "TTNMkrWanNode.h"
#include "debug.h"
#include "sleep.h"
#include "ttn.h"

#include "arduino_secrets.h"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

TTN::TTN(): LoRaModem(), FRAM() {
}

boolean TTN::begin() {
  if (!LoRaModem::begin(EU868)) {
    D_PRINTLN("Failed to start module");
    blink(500,500);
  };
  D_PRINTLN(String("Ver: ") + TTN::version());
  D_PRINTLN(String("EUI: ") + TTN::deviceEUI());

  if (FRAM::begin()) {
    D_PRINTLN("FRAM initialized");
  } else {
    D_PRINTLN("No SPI FRAM found");
    blink(250,250);
  }
}

void TTN::join() {
  // Join network
  if(digitalRead(resetPin) == LOW) {
    joinOTAA();
  } else if (!getKeys()) {
    joinOTAA();
  } else {
    joinABP();
  }
}

void TTN::joinOTAA() {
  // Join in OTAA mode; Sleep in case of failure
  // On success save keys in structure
  D_PRINTLN("OTAA Join");
  digitalWrite(ledPin, HIGH);

  if (!LoRaModem::joinOTAA(appEui, appKey)) {
    D_PRINTLN("Failed");
    blink(1000,500);
  }
  D_PRINTLN("Joined");
  digitalWrite(ledPin, LOW);

  String devAddr = LoRaModem::getDevAddr();
  String nwkSKey = LoRaModem::getNwkSKey();
  String appSKey = LoRaModem::getAppSKey();

  if (devAddr.length() != devAddrSize ||
      nwkSKey.length() != sKeySize ||
      appSKey.length() != sKeySize) {
    // Can't get device address / keys. Invalidate ttnData for next round
    D_PRINTLN("Can't retrieve keys");
    ttnData.ver = 0;
  } else {
    D_PRINTLN(String("devAddr: ") + devAddr);
    D_PRINTLN(String("nwkSKey: ") + nwkSKey);
    D_PRINTLN(String("appSKey: ") + appSKey);
    ttnData.ver = ttnDataVer;
    memcpy((void*)ttnData.devAddr, (void*)devAddr.c_str(), devAddrSize);
    memcpy((void*)ttnData.nwkSKey, (void*)nwkSKey.c_str(), sKeySize);
    memcpy((void*)ttnData.appSKey, (void*)appSKey.c_str(), sKeySize);
    ttnData.fcu = 0;
    ttnData.fcd = 0;
    ttnData.port = ttnPort;
    ttnData.dataRate = ttnDataRate;
  }
}

bool TTN::getKeys() {
  // Get data from FRAM and populate struct
  // On failure (crc, ...) return false
  D_PRINT("Get FRAM data - ");

  if(!FRAM::read((uint8_t*)&ttnData, sizeof(ttnData))) {
    D_PRINTLN("No valid data found");
    return false;
  }

  if (ttnData.ver != ttnDataVer) {
    D_PRINTLN("Wrong version: " + String(ttnData.ver));
    return false;
  } else {
    D_PRINTLN("Done");
    D_PRINTLN(String("D") + ttnData.fcd + String(" U") + ttnData.fcu);
    return true;
  }
}

void TTN::saveKeys() {
  // Get session counters
  // Save data to FRAM
  int32_t fcu, fcd;
  D_PRINTLN("Save keys");
  fcu = LoRaModem::getFCU();
  // Sometimes first Downlink query gets uplink counter!
  fcd = LoRaModem::getFCD();
  fcd = LoRaModem::getFCD();
  if (fcu < 0 || fcd < 0) {
    // Can't retrieve session counters, invalidate ttnData
    D_PRINTLN("Can't get session counters");
    ttnData.ver = 0;
  }

  ttnData.fcu = fcu;
  ttnData.fcd = fcd;
  D_PRINTLN("D" + String(fcd) + " U" + String(fcu) + " V" + String(ttnData.ver));

  FRAM::write((uint8_t*)&ttnData, sizeof(ttnData));
}


void TTN::joinABP() {
  // Join in ABP mode; should always succeed
  // on failure (?) joinOTAA
  // On success set session counters
  D_PRINTLN("ABP Join");
  char devAddr[devAddrSize+1], nwkSKey[sKeySize+1], appSKey[sKeySize+1];
  memcpy((void*)devAddr, (void*)ttnData.devAddr, devAddrSize);
  devAddr[devAddrSize] = '\0';
  memcpy((void*)nwkSKey, (void*)ttnData.nwkSKey, sKeySize);
  nwkSKey[sKeySize] = '\0';
  memcpy((void*)appSKey, (void*)ttnData.appSKey, sKeySize);
  appSKey[sKeySize] = '\0';

  if (!LoRaModem::joinABP(devAddr, nwkSKey, appSKey)) {
    D_PRINTLN("Failed");
    blink(500,1000);
  }

  LoRaModem::setFCU(ttnData.fcu);
  LoRaModem::setFCD(ttnData.fcd);
}

void TTN::sendData(uint8_t* data, uint16_t dataSize) {
  D_PRINTLN("About to send data");
  int err;
  LoRaModem::setPort(ttnData.port);
  LoRaModem::dataRate(ttnData.dataRate);
  LoRaModem::beginPacket();
  LoRaModem::write(data, dataSize);
  err = LoRaModem::endPacket(false);
  if (err > 0) {
    D_PRINTLN("Message sent");
  } else {
    D_PRINTLN("Error sending message");
  }
  uint16_t i = 0;
  int32_t fcu = LoRaModem::getFCU();
  while (fcu < 0 || fcu == ttnData.fcu) {
    i++;
    delay(1000 / pollPerSec);
    fcu = LoRaModem::getFCU();
  }

  D_PRINTLN(String("Sent after: ") + ((float)i)/pollPerSec + " secs.");

  // Check for downlink
  delay(500);
  if (!LoRaModem::available()) {
    D_PRINTLN("No downlink");
    return;
  }

  D_PRINTLN("Processing downlink");
  uint8_t command, parameter;
  while (LoRaModem::available()) {
    switch(command = LoRaModem::read()) {
      case 1: // Reset
        ttnData.ver = 0;
        D_PRINTLN("Reset module");
        break;
      case 2: // Port
        if (LoRaModem::available()) {
          ttnData.port = parameter = LoRaModem::read();
          D_PRINTLN(String("Port set to ") + parameter);
        } else {
          D_PRINTLN("Missing Port argument");
        }
        break;
      case 3: // Data Rate
        if (LoRaModem::available()) {
          parameter = LoRaModem::read();
          if (parameter > 6) {
            D_PRINTLN(String("Invalid Data Rate: ") + parameter);
          } else {
            ttnData.dataRate = parameter;
            D_PRINTLN(String("Data Rate set to ") + parameter);
          }
        } else {
          D_PRINTLN("Missing Data Rate argument");
        }
        break;
      default:
        // Drain buffer
        while (LoRaModem::available()) {
          LoRaModem::read();
        }
        D_PRINTLN(String("Invalid downlink command: ") + command);
        break;
    }
  }
}
