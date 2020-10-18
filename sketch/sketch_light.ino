#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"

#define BUTTON_GPIO 0
#define LED_GPIO 13
#define RELAY_GPIO 12

WiFiClient espClient;
PubSubClient client(espClient);

bool wifiConnected = false;
bool lightState = false;

void setup() {
  pinMode(BUTTON_GPIO, INPUT);
  
  pinMode(RELAY_GPIO, OUTPUT);
  digitalWrite(RELAY_GPIO, LOW);

  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);

  // Connecting to Wifi
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (!wifiConnected) {
    blink(500, 1);
  
    // Connected to Wifi
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      
      blink(200, 3);
      digitalWrite(LED_GPIO, LOW);

      // Connecting to MQTT broker
      client.setServer(MQTT_HOST, MQTT_PORT);
      client.setCallback(callback);

      ArduinoOTA.setHostname(WIFI_HOSTNAME);
      ArduinoOTA.setPassword(OTA_PASSWORD);
      ArduinoOTA.begin();
    }
  }

  if (digitalRead(BUTTON_GPIO) == LOW) {
    setLightState(!lightState);
  }
  
  if (!client.connected()) {
    connectMQTT();
  }
   
  client.loop();

  ArduinoOTA.handle();
}

void callback(char* topic, byte* payload, unsigned int length) {  
  payload[length] = '\0';
  setLightState(strcmp((char*) payload, "ON") == 0);
}

void setLightState(boolean isOn) {
  blink(50, 1);
  
  digitalWrite(RELAY_GPIO, isOn ? HIGH : LOW);
  lightState = isOn;

  if (client.connected()) {
    publishLightState(); 
  }
}

void connectMQTT() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      blink(100, 3);
      client.subscribe(COMMAND_TOPIC);
      publishLightState();
    } else {
      blink(1000, 2);
    }
  }
}

void publishLightState() {
  if (client.publish(STATE_TOPIC, (char*)(lightState ? "ON" : "OFF"), true)) {
    blink(200, 1);
  } else {
    blink(1000, 2);
  }
}

void blink(int milliseconds, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_GPIO, HIGH);
    delay(milliseconds);                 
    digitalWrite(LED_GPIO, LOW);
    delay(milliseconds);
  }
}

