//
// Created by dave on 25.02.23.
//

#ifndef ESP8266_MOTION_AND_LIGHT_SENSORS_MQTTTOPIC_H
#define ESP8266_MOTION_AND_LIGHT_SENSORS_MQTTTOPIC_H

#include <string>

class MqttTopic {
  std::string rootTopic;

public:
  MqttTopic(char rootTopic[]) {
    this->rootTopic = rootTopic;
  }
  MqttTopic(String rootTopic) {
    this->rootTopic = rootTopic.c_str();
  }

  std::string getAvailabilityTopic() {
    return this->rootTopic + "/iwt";
  }

  std::string getCommandTopic() {
    return this->rootTopic + "/command";
  }

  std::string getMotionStateTopic() {
    return this->rootTopic + "/motion/state";
  }

  std::string getLightStateTopic() {
    return this->rootTopic + "/light/state";
  }
};

#endif //ESP8266_MOTION_AND_LIGHT_SENSORS_MQTTTOPIC_H
