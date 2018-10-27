/*
  maximTemp.h
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

#ifndef maximTemp_h
#define maximTemp_h

#include <OneWire.h>              // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>    // https://github.com/milesburton/Arduino-Temperature-Control-Library

#include "Sensor.h"

const int oneWirePin = 6;         // Pin for the oneWire bus
const int tempPrecision = 12;     // 12 bits temp precision

class MaximTemp: public Sensor
{
  public:
    MaximTemp();
    void setup();
    void collect(CayenneLPP& lpp, int& payloadIndex);
  private:
    OneWire oneWire;
    DallasTemperature dallasSensors;
};


#endif // maximTemp_h
