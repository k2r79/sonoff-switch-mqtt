#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const int BUTTON_GPIO = 0;
const int LED_GPIO = 13;
const int RELAY_GPIO = 12;

WiFiClient espClient;
PubSubClient client(espClient);

boolean lightState = false;

void setup() {
  pinMode(BUTTON_GPIO, INPUT);
  
  pinMode(RELAY_GPIO, OUTPUT);
  digitalWrite(RELAY_GPIO, LOW);

  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);

  Serial.begin(115200);

  // Connecting to Wifi
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    blink(500);
    Serial.print("Connecting to WiFi..");
  }

  // Connected to Wifi
  Serial.println("Connected to the WiFi network");
  tripleBlink();
  digitalWrite(LED_GPIO, LOW);

  // Connecting to MQTT broker
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  if (digitalRead(BUTTON_GPIO) == LOW) {
    setLightState(!lightState);
  }
  
  if (!client.connected()) {
    connectMQTT();
  }
   
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {  
  payload[length] = '\0';
  setLightState(strcmp((char*) payload, "ON") == 0);
}

void setLightState(boolean isOn) {
  blink(50);
  
  digitalWrite(RELAY_GPIO, isOn ? HIGH : LOW);
  lightState = isOn;
  
  publishLightState();
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
 
    if (client.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker");
      tripleBlink();

      client.subscribe(COMMAND_TOPIC);
      publishLightState();
    } else {
      Serial.print("Failed to connect to MQTT broker with state ");
      Serial.print(client.state());
      blink(1000);
      blink(1000);
    }
  }
}

void publishLightState() {
  if (client.publish(STATE_TOPIC, (char*)(lightState ? "ON" : "OFF"), true)) {
    blink(200);
  } else {
    blink(200);
    blink(200);
  }
}

void blink(int milliseconds) {
  digitalWrite(LED_GPIO, HIGH);
  delay(milliseconds);                 
  digitalWrite(LED_GPIO, LOW);
  delay(milliseconds); 
}

void tripleBlink() {
  for (int i = 0; i < 3; i++) {
    blink(200);
  }
}

