#include <Arduino.h>
#include "settings.h"
#include "LED.h"
#include "DigitalSensor.h"
#include "AnalogSensor.h"

#define PIN_LED_YELLOW D0
#define PIN_LED_GREEN D5
#define PIN_LED_BLUE D6
#define PIN_MOTION D1
#define PIN_LIGHT A0
#define PIN_STATUS LED_BUILTIN

LED blue(PIN_LED_BLUE, LOW);
LED green(PIN_LED_GREEN, LOW);
LED yellow(PIN_LED_YELLOW, LOW);
DigitalSensor motion(PIN_MOTION);
AnalogSensor light(PIN_LIGHT, 5000);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(blue.getPinNumber(), OUTPUT);
  pinMode(green.getPinNumber(), OUTPUT);
  pinMode(yellow.getPinNumber(), OUTPUT);
  pinMode(motion.getPinNumber(), INPUT);
  pinMode(light.getPinNumber(), INPUT);
  motion.setStatusLed(&blue);
  setupConfigurationServer(PIN_STATUS);
}

void loop() {
  iotWebConf.doLoop();
  if (needReboot) {
    Serial.println("Rebooting after 1 second.");
    iotWebConf.delay(1000);
    EspClass::restart();
  }
  yield();
  if (needMqttConnect) {
    mqttReconnect();
  }
  yield();
//  if (iotWebConf.getState() == iotwebconf::OnLine) {
//    green.enable();
//  } else {
//    green.disable();
//  }
  yield();
  if (mqttClient.connected()) {
    mqttClient.loop();
    yellow.enable();
  } else {
    yellow.disable();
  }
  yield();
  if (motion.isTimeOutReached()) {
    motion.check();
  }
  yield();
  if (light.isTimeOutReached()) {
    light.check();
    Serial.printf("Light: %d\n", light.getCurrentState());
    if (light.getCurrentState() > 500) {
      green.enable();
    } else {
      green.disable();
    }
  }
}