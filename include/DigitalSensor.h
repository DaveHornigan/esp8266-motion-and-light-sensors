//
// Created by dave on 21.02.23.
//

#ifndef ESP8266_MOTION_AND_LIGHT_SENSORS_DIGITALSENSOR_H
#define ESP8266_MOTION_AND_LIGHT_SENSORS_DIGITALSENSOR_H

class DigitalSensor {
  uint8 pinNumber;
  uint8 currentState = 0;
  uint64 lastChecked = 0;
  uint32 timeOut = 0;
  LED *statusLed = nullptr;

public:
  DigitalSensor(uint8 pin, uint32 timeout = 0, uint8 initState = 0) {
    pinNumber = pin;
    timeOut = timeout;
    currentState = initState;
  }

  [[nodiscard]] bool isTimeOutReached() const {
    return millis() - lastChecked > timeOut;
  }

  [[nodiscard]] bool isEnabled() const {
    return currentState == 1;
  }

  void check() {
    currentState = digitalRead(pinNumber);
    if (statusLed != nullptr) {
      isEnabled() ? statusLed->enable() : statusLed->disable();
    }
    lastChecked = millis();
  }

  [[nodiscard]] uint8 getPinNumber() const {
    return pinNumber;
  }

  void setStatusLed(LED *led) {
    statusLed = led;
  }
};

#endif //ESP8266_MOTION_AND_LIGHT_SENSORS_DIGITALSENSOR_H
