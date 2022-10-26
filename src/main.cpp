#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>

#include "app.h"

#define BUTTON_PIN 16

USBHIDKeyboard Keyboard;
WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

unsigned long
  lastWifiOnline = 0,
  lastWifiReconnect = 0,
  lastPubSubReconnectAttempt = 0,
  mqttUpdateMillis = 0;

bool waitHigh = true;

bool reconnectPubSub() {
  if (now - lastPubSubReconnectAttempt > MQTT_RECONNECT_MILLIS) {
    lastPubSubReconnectAttempt = now;

    if (pubSubClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_STATUS_TOPIC, MQTTQOS0, true, MQTT_STATUS_OFFLINE_MSG, true)) {
      pubSubClient.publish(MQTT_STATUS_TOPIC, MQTT_STATUS_ONLINE_MSG, true);
      pubSubClient.publish(MQTT_VERSION_TOPIC, VERSION, true);
    }
    
    return pubSubClient.connected();
  }

  return false;
}

void pubSubClientLoop() {
  if (!pubSubClient.connected() && !reconnectPubSub()) return;

  pubSubClient.loop();
}

bool wifi_loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiOnline > WIFI_WATCHDOG_MILLIS) ESP.restart();
    else if (now - lastWifiReconnect > WIFI_RECONNECT_MILLIS) {
      lastWifiReconnect = now;

      if (WiFi.reconnect()) {
        lastWifiOnline = now;
        return true;
      }
    }

    return false;
  }
  
  lastWifiReconnect = now;
  lastWifiOnline = now;

  pubSubClientLoop();

  ArduinoOTA.handle();

  return true;
}

void wifi_setup() {
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(wifi_ps_type_t::WIFI_PS_NONE);
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);

  now = millis();
  lastWifiOnline = now;

  // pubSubClient.setCallback(onMqttMessage);
  pubSubClient.setServer(MQTT_SERVER_NAME, MQTT_SERVER_PORT);

  ArduinoOTA.begin();
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Keyboard.begin();
  USB.begin();

  waitHigh = !(digitalRead(BUTTON_PIN) == LOW);

  wifi_setup();
}

void loop() {
  if (!waitHigh) {
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(100);
      wifi_loop();
    }
    
    waitHigh = true;
    delay(100);
  
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('q');
    delay(100);
    Keyboard.releaseAll();

    delay(200);
    Keyboard.write(KEY_ESC);

    pubSubClient.publish(MQTT_CLIENT_ID "/locked", "1");
  } else {
    while (digitalRead(BUTTON_PIN) == HIGH) {
      // do nothing until pin 2 goes low
      delay(100);
      wifi_loop();
    }
    waitHigh = false;

    Keyboard.press(' ');
    delay(100);
    Keyboard.releaseAll();

    pubSubClient.publish(MQTT_CLIENT_ID "/locked", "0");
  }

  wifi_loop();
}
