/*
  debug.cpp

  Macros for debugging

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

#ifdef DEBUG
  #ifdef OLED
    #include <Wire.h>
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
    U8G2LOG debug;
    // Font selection -- Allows 8 lines of 25 characters
    const uint8_t *u8g2Font = u8g2_font_5x7_tr;
    const uint16_t u8gLogWidth = 25;
    const uint16_t u8gLogHeight = 8;
    uint8_t u8LogBuffer[u8gLogWidth * u8gLogHeight];
  #else
    Serial_ debug = Serial;
  #endif
#endif

void setupDebug() {
  #ifdef DEBUG
    #ifdef OLED
      // Setup display
      u8g2.begin();
      u8g2.clearBuffer();
      u8g2.setFont(u8g2Font);
      debug.begin(u8g2, u8gLogWidth, u8gLogHeight, u8LogBuffer);
      debug.setLineHeightOffset(0);
      debug.setRedrawMode(0);
    #else
      while (!debug);
      debug.begin(9600);
    #endif
  #endif
}
