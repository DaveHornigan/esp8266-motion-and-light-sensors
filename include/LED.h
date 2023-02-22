//
// Created by dave on 21.02.23.
//

#ifndef ESP8266_MOTION_AND_LIGHT_SENSORS_LED_H
#define ESP8266_MOTION_AND_LIGHT_SENSORS_LED_H

class LED {
  uint8 pinNumber;
  uint8 currentState;
  uint64 lastChecked;
  uint32 timeOut;

public:
  LED(uint8 pin, uint8 currentState = LOW, uint32 timeout = 0) {
    this->pinNumber = pin;
    this->currentState = currentState;
    this->lastChecked = 0;
    this->timeOut = timeout;
  }

  [[nodiscard]] bool isTimeOutReached() const {
    return millis() - lastChecked > timeOut;
  }

  void check() {
    lastChecked = millis();
  }

  void switchState() {
    digitalWrite(pinNumber, !currentState);
    currentState = !currentState;
  }

  void enable() {
    if (currentState != HIGH) {
      digitalWrite(pinNumber, HIGH);
      currentState = HIGH;
    }
  }

  void disable() {
    if (currentState != LOW) {
      digitalWrite(pinNumber, LOW);
      currentState = LOW;
    }
  }

  [[nodiscard]] uint8 getPinNumber() const {
    return pinNumber;
  }
};

#endif //ESP8266_MOTION_AND_LIGHT_SENSORS_LED_H
