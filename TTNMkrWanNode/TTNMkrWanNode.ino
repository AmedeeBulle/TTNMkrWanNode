/*
  TTN MKRWAN Node

  Ultra Low Power node proof of concept

  Uses
    - a TPL5110 as timer / power control
    - a FRAM module to store session information

  The MkrWan initially joins with OTAA and stores session data in FRAM
  When the TMP5110 wakes up the node we
    - Retrieve the session data from FRAM
    - Join ABP
    - Send data
    - Save session data in FRAM
    - Enter sleep
*/

#include "TTNMkrWanNode.h"
#include "debug.h"
#include "ttn.h"
#include "sleep.h"
#ifdef SENSOR_VOLTAGE
#include "voltage.h"
#endif
#ifdef SENSOR_MAXIM
#include "maximTemp.h"
#endif
#ifdef SENSOR_BME280
#include "bme280.h"
#endif
#include <CayenneLPP.h>

TTN ttn;

// Cayenne LPP
const uint8_t maxPayloadSize = 16;  // Current size: 8 / 15
CayenneLPP lpp(maxPayloadSize);

// Arduino IDE does not link properly when using lists; use an array instead
#ifdef SENSOR_VOLTAGE
Voltage voltageSensor;
#endif
#ifdef SENSOR_MAXIM
MaximTemp maximTemp;
#endif
#ifdef SENSOR_BME280
BME280 bme280;
#endif

Sensor* sensors[] = {
#ifdef SENSOR_VOLTAGE
  &voltageSensor,
#endif
#ifdef SENSOR_MAXIM
  &maximTemp,
#endif
#ifdef SENSOR_BME280
  &bme280,
#endif
};

void setup() {
  // Setup debug output (OLed or Serial)
  setupDebug();
  D_PRINTLN("Initializing");

  // Setup sleep
  setupSleep();

  // Pin configuration
  pinMode(ledPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP);

  // Setup sensors
  for (uint8_t i = 0; i < (sizeof(sensors) / sizeof(sensors[0])); i++) {
    sensors[i]->setup();
  }

  // Init LoRaWan TTN
  ttn.begin();

  // Join (OTAA or ABP)
  ttn.join();

  // Prepare payload
  int payloadIndex = 1;
  lpp.reset();

  // Collect sensors data
  for (uint8_t i = 0; i < (sizeof(sensors) / sizeof(sensors[0])); i++) {
    sensors[i]->collect(lpp, payloadIndex);
  }

  // We are ready to send data
  D_PRINTLN(String("Payload: ") + lpp.getSize());
  ttn.sendData(lpp.getBuffer(), lpp.getSize());

  // Save keys
  ttn.saveKeys();

  // Enter sleep mode
  enterSleep();
}

void loop() {
}
