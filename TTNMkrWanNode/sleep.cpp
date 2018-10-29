/*
  sleep.cpp

  Enter sleep mode

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



void setupSleep() {
  // Setup the Done pin for the TPL5110
  pinMode(donePin, OUTPUT);
}

void enterSleep() {
  // Raise the Done pin for the TPL5110
  D_PRINTLN("Entering sleep mode");
  D_FLUSH;
  digitalWrite(donePin, HIGH);
  // Following code never reached if TPL510 works properly ;)
  // Used when node is powered from USB
  delay(5 * 60 * 1000L);
  D_PRINTLN("About to reset");
  D_FLUSH;
  NVIC_SystemReset();
  while (1) {}
}

const uint16_t blink_delay = 10;
void blink(uint16_t b_on, uint16_t b_off) {
  // Blink LED for blink_delay terminates
  unsigned long end_time = millis() + blink_delay * 1000L;
  while (millis() < end_time) {
    digitalWrite(ledPin, HIGH);
    delay(b_on);
    digitalWrite(ledPin, LOW);
    delay(b_off);
  }
  enterSleep();
}
