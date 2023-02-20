//
// Created by dave on 10.02.23.
//

#ifndef ESP8266_WIFI_SNIFFER_SETTINGS_H
#define ESP8266_WIFI_SNIFFER_SETTINGS_H

#define IOTWEBCONF_DEBUG_PWD_TO_SERIAL

#include <DNSServer.h>
#include "PubSubClient.h"
#include "IotWebConf.h"
#include "IotWebConfUsing.h"
#include <ESP8266WiFi.h>

#define DEBUG_ENABLED 1
#define DEBUG_DISABLED 0
#define DEBUG_MODE DEBUG_ENABLED

#define SERIAL_BAUD_RATE 115200

#define DEVICE_NAME "Hornigan ESP Sensor"

#ifdef ESP8266
#define CHIP_ID String(EspClass::getChipId(), HEX)
#elif ESP32
#define CHIP_ID String((uint32_t) ESP.getEfuseMac(), HEX)
#endif

#define AP_WIFI_PASS "0123456789"
#define AP_WIFI_NAME (String(DEVICE_NAME) + String(" - ") + String(CHIP_ID))

#define SETTING_PARAM_LENGTH 60

bool needMqttConnect = false;
bool needReboot = false;
unsigned long lastMqttConnectionAttempt = 0;

// <Settings>
char mqttServer[SETTING_PARAM_LENGTH];
const char* mqttTopic = "hornigan/devices/sniffer";
char mqttUser[SETTING_PARAM_LENGTH];
char mqttPass[SETTING_PARAM_LENGTH];
// </Settings>


DNSServer dnsServer;
WebServer webServer(80);
IotWebConf iotWebConf(AP_WIFI_NAME.c_str(), &dnsServer, &webServer, AP_WIFI_PASS, "1");
WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

IotWebConfParameterGroup mqttGroup = IotWebConfParameterGroup("mqtt", "MQTT");
IotWebConfTextParameter mqttServerParam = IotWebConfTextParameter(
    "MQTT Server",
    "mqttServer",
    mqttServer,
    SETTING_PARAM_LENGTH,
    "192.168.0.2"
);
IotWebConfTextParameter mqttUserNameParam = IotWebConfTextParameter(
    "MQTT Username",
    "mqttUser",
    mqttUser,
    SETTING_PARAM_LENGTH,
    "mqtt"
);
IotWebConfPasswordParameter mqttPasswordParam = IotWebConfPasswordParameter(
    "MQTT Password",
    "mqttPass",
    mqttPass,
    SETTING_PARAM_LENGTH,
    ""
);

bool formValidator(iotwebconf::WebRequestWrapper *webRequestWrapper) {
  uint serverLength = webRequestWrapper->arg(mqttServerParam.getId()).length();
  bool hasErrors = false;
  if (serverLength == 0) {
    mqttServerParam.errorMessage = "Please provide an MQTT server";
    hasErrors = true;
  }
  uint ssidLength = iotWebConf.getWifiSsidParameter()->getLength();
  if (ssidLength == 0) {
    iotWebConf.getWifiSsidParameter()->errorMessage = "Please provide SSID";
    hasErrors = true;
  }
  return !hasErrors;
}

void mqttSubscribe() {
//  mqttClient.subscribe("");
}

void mqttMessageReceived(const char *topic, byte *payload, unsigned int length) {
  char message[MQTT_MAX_PACKET_SIZE];
  if (length > 0) {
    memset(message, '\0', sizeof(message));
    memcpy(message, payload, length);
    #if DEBUG_MODE == DEBUG_ENABLED
    Serial.println();
    Serial.print(F("Requested ["));
    Serial.print(topic);
    Serial.print(F("] "));
    Serial.println(message);
    #endif
  }
}

bool apConnect(const char *apName, const char *password) {
  #if DEBUG_MODE == DEBUG_ENABLED
  bool hidden = false;
  #else
  bool hidden = true;
  #endif
  if (iotWebConf.getState() != iotwebconf::NotConfigured) {
    WiFi.mode(WIFI_AP_STA);
  }

  return WiFi.softAP(AP_WIFI_NAME, AP_WIFI_PASS, 13, hidden);
}

bool mqttReconnect() {
  delay(1);
  unsigned long now = millis();
  if (1000 > now - lastMqttConnectionAttempt || !WiFi.isConnected()) {
    return false;
  }
  if (!mqttClient.connected()) {
    Serial.printf("Connecting to MQTT server...\n");
    mqttClient.setCallback(mqttMessageReceived);
    mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE);
    mqttClient.setServer(mqttServer, 1883);
    if (mqttClient.connect(
        AP_WIFI_NAME.c_str(),
        mqttUser,
        mqttPass
    )) {
      Serial.println(F("MQTT Connected"));
      needMqttConnect = false;
      mqttSubscribe();
    } else {
      lastMqttConnectionAttempt = now;
      Serial.print("MQTT connect failed with state ");
      Serial.println(mqttClient.state());

      return false;
    }
  }

  return true;
}

void wifiConnected() {
#if DEBUG_MODE == DEBUG_ENABLED
  Serial.println();
  Serial.println(F("=================================================================="));
  Serial.println(F(DEVICE_NAME));
  Serial.println(F("=================================================================="));
  Serial.println();
  Serial.print(F("Connected to Wifi \t["));
  Serial.print(WiFi.localIP());
  Serial.println(F("]"));
#endif
  needMqttConnect = true;
}

void configSaved() {
  Serial.println("Configuration was updated.");
  needReboot = true;
}

void setupDefaultSettingsValues() {
  strncpy(
      iotWebConf.getThingNameParameter()->valueBuffer,
      "admin",
      iotWebConf.getThingNameParameter()->getLength());
  iotWebConf.getThingNameParameter()->defaultValue = "admin";
}

void setupConfigurationServer(int statusPin = LED_BUILTIN) {
  setupDefaultSettingsValues();
  mqttGroup.addItem(&mqttServerParam);
  mqttGroup.addItem(&mqttUserNameParam);
  mqttGroup.addItem(&mqttPasswordParam);
  iotWebConf.addParameterGroup(&mqttGroup);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.setWifiConnectionTimeoutMs(15000);
  iotWebConf.setApConnectionHandler(&apConnect);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.setStatusPin(statusPin);
//  iotWebConf.skipApStartup();

  WiFi.setAutoReconnect(false);

  boolean validConfig = iotWebConf.init();
  if (!validConfig) {
    mqttServer[0] = '\0';
    mqttUser[0] = '\0';
    mqttPass[0] = '\0';
  }

  webServer.on("/", [] { iotWebConf.handleConfig(); });
  webServer.onNotFound([] { iotWebConf.handleNotFound(); });
}

#endif //ESP8266_WIFI_SNIFFER_SETTINGS_H
