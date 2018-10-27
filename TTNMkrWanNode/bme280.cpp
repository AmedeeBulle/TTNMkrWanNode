/*
  bme280.cpp

  Read data from BME280

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

#include "bme280.h"
#include "debug.h"
#include "sleep.h"

BME280::BME280(): bme() {
}

void BME280::setup() {
  if (! bme.begin(&Wire)) {
    D_PRINTLN("Can't find BME280 Sensor");
    blink(2000,2000);
  }

  // Weather monitoring profile
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
}

void BME280::collect(CayenneLPP& lpp, int& payloadIndex) {
  float temp;

  bme.takeForcedMeasurement();

  temp = bme.readTemperature();
  lpp.addTemperature(payloadIndex++, temp);
  D_PRINTLN("T: " + String(temp) + "°C");

  lpp.addBarometricPressure(payloadIndex++, bme.readPressure() / 100.0F);

  lpp.addRelativeHumidity(payloadIndex++, bme.readHumidity());

}
