//
// Created by dave on 21.02.23.
//

#ifndef ESP8266_TOILET_SENSORS_ANALOGSENSOR_H
#define ESP8266_TOILET_SENSORS_ANALOGSENSOR_H

class AnalogSensor {
  uint8 pinNumber;
  uint32 currentState;
  uint64 lastChecked = 0;
  uint32 timeOut = 0;

public:
  AnalogSensor(uint8 pin, uint32 timeout = 0, uint32 initState = 0) {
    pinNumber = pin;
    timeOut = timeout;
    currentState = initState;
  }

  [[nodiscard]] bool isTimeOutReached() const {
    return millis() - lastChecked > timeOut;
  }

  [[nodiscard]] uint32 getCurrentState() const {
    return currentState;
  };

  void check() {
    currentState = analogRead(pinNumber);
    lastChecked = millis();
  }

  [[nodiscard]] uint8 getPinNumber() const {
    return pinNumber;
  }
};

#endif //ESP8266_TOILET_SENSORS_ANALOGSENSOR_H
