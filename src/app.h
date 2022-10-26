#ifndef VERSION
#define VERSION "1.1.0"
#endif

#ifndef __APP_H
#define __APP_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define WIFI_HOSTNAME                 "esp32-big-red-lock"
#define WIFI_SSID                     "qx.zone"
#define WIFI_PASSPHRASE               "1234Qwer-"
#define WIFI_RECONNECT_MILLIS         10000
#define WIFI_WATCHDOG_MILLIS          60000

#define MQTT_SERVER_NAME              "ns2.in.qx.zone"
#define MQTT_SERVER_PORT              1883
#define MQTT_USERNAME                 NULL
#define MQTT_PASSWORD                 NULL
#define MQTT_RECONNECT_MILLIS         5000
#define MQTT_CLIENT_ID                "dev/" WIFI_HOSTNAME
#define MQTT_STATUS_TOPIC             MQTT_CLIENT_ID "/status"
#define MQTT_VERSION_TOPIC            MQTT_CLIENT_ID "/version"
#define MQTT_RESTART_CONTROL_TOPIC    MQTT_CLIENT_ID "/restart"
#define MQTT_STATUS_ONLINE_MSG        "online"
#define MQTT_STATUS_OFFLINE_MSG       "offline"

unsigned long now = 0;

void wifi_setup();
bool wifi_loop();

#endif