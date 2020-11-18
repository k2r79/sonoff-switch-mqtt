#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include "config.h"

#define BUTTON_GPIO 0
#define LED_GPIO 13
#define RELAY_GPIO 12

WiFiClient espClient;
PubSubClient client(espClient);

bool lightState = false;
bool wifiConnected = false;
long buttonWaitInterval = 8000;

void setup() {
  pinMode(BUTTON_GPIO, INPUT);
  
  pinMode(RELAY_GPIO, OUTPUT);
  digitalWrite(RELAY_GPIO, LOW);

  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);

  // Connecting to Wifi
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Connecting to MQTT broker
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  // Connected to Wifi
  if (WiFi.status() == WL_CONNECTED) { 

    if (!wifiConnected) {
      blink(200, 3);
      digitalWrite(LED_GPIO, LOW);
      
      wifiConnected = true;
    }

    if (!client.connected()) {
      connectMQTT();
    }
  } else {
    blink(1000, 1);
    wifiConnected = false;
  }

  client.loop();

  listenForButton();
}

void listenForButton() {
  long startMillis = millis();

  while (startMillis - millis() < buttonWaitInterval) {
    if (digitalRead(BUTTON_GPIO) == LOW) {
      setLightState(!lightState);
    }
  }
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
  if (client.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    client.subscribe(COMMAND_TOPIC);
    publishLightState();
    blink(100, 6);
  } else {
    blink(500, 2);
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
