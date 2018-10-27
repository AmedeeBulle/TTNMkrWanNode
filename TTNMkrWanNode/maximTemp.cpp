/*
  maximTemp.cpp

  Read temperature from Maxim (Dallas) DS18B20

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

#include "maximTemp.h"
#include "debug.h"

// Array to hold device address
DeviceAddress thermometer  = { 0x28, 0xFF, 0x6B, 0x12, 0x71, 0x15, 0x03, 0xA3 };

MaximTemp::MaximTemp(): oneWire(oneWirePin), dallasSensors(&oneWire) {
}

void MaximTemp::setup() {
  dallasSensors.begin();
  dallasSensors.setResolution(thermometer, tempPrecision);
  // First read after power on is not correct
  dallasSensors.requestTemperatures();
  dallasSensors.getTempC(thermometer);
}

void MaximTemp::collect(CayenneLPP& lpp, int& payloadIndex) {
  float temp;

  dallasSensors.requestTemperatures();
  temp = dallasSensors.getTempC(thermometer);
  lpp.addTemperature(payloadIndex++, temp);
  D_PRINTLN("T: " + String(temp) + "°C");
}
