/*
  voltage.c

  Get battery voltage

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

#include "voltage.h"

void Voltage::setup() {
  pinMode(batteryPin, INPUT);
  analogReadResolution(analogBits);
  // The 1v reference is the only absolut eref available
  analogReference(AR_INTERNAL1V0);
  // From https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
  // After changing the analog reference, the first few readings from analogRead() may not be accurate.
  for (uint8_t i = 0; i < 8 ; i++) {
    analogRead(batteryPin);
  }
}

void Voltage::collect(CayenneLPP& lpp, int& payloadIndex) {
  uint16_t sensorRead = 0;

  for (uint8_t i = 0; i < overSampling ; i++) {
    sensorRead += analogRead(batteryPin);
  }

  lpp.addAnalogInput(payloadIndex++, sensorRead / overSampling * divider / analogMax);
}

