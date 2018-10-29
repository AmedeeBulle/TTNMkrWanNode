/*
  voltage.h

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

#ifndef voltage_h
#define voltage_h

#include <Arduino.h>

#include "Sensor.h"

const uint8_t batteryPin = ADC_BATTERY;           // Battery pin
const uint8_t analogBits = 10;                    // ADC resolution
const uint16_t analogMax = (1 << analogBits)-1;   // Max value for ADC
const float divider = 3.018;                      // Voltage divider should be (680 + 330) / 330 == 3.061;
                                                  // set to 3.018 after calibration
const uint8_t overSampling = 8;                   // Read oversampling

class Voltage: public Sensor
{
  public:
    void setup();
    void collect(CayenneLPP& lpp, int& payloadIndex);
};

#endif // voltage_h
