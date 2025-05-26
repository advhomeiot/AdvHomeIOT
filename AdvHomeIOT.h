#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "AdvHomeIOT.h"

// Token (user only provides this)
AdvHomeIOT advHome("bf1daa05d008ee35");  // 👈 Only input required

// Relay Pins D1 to D6
const int relayPins[] = {D1, D2, D3, D4, D5, D6};
bool relayState[] = {false, false, false, false, false, false};

// Current Sensor
const int sensorIn = A0;
int mVperAmp = 100;
int decimalPrecision = 3;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

// Timers
unsigned long lastCurrentPublish = 0;
const unsigned long currentPublishInterval = 5000; // 5 seconds

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  for (int i = 0; i < 6; i++) {
    String controlTopic = "cust_67f22068e628a/relay" + String(i + 1) + "/control";
    String statusTopic = "cust_67f22068e628a/relay" + String(i + 1) + "/status";

    if (String(topic) == controlTopic) {
      if (message == "ON") {
        digitalWrite(relayPins[i], LOW);
        relayState[i] = true;
        Serial.println("Relay " + String(i + 1) + " ON");
        advHome.getClient().publish(statusTopic.c_str(), "ON");
      } else if (message == "OFF") {
        digitalWrite(relayPins[i], HIGH);
        relayState[i] = false;
        Serial.println("Relay " + String(i + 1) + " OFF");
        advHome.getClient().publish(statusTopic.c_str(), "OFF");
      } else if (message == "GET_STATUS") {
        advHome.getClient().publish(statusTopic.c_str(), relayState[i] ? "ON" : "OFF");
      }
    }
  }

  if (String(topic) == "nodemcu/relay/status/request") {
    String response = "";
    for (int i = 0; i < 6; i++) {
      response += "Relay" + String(i + 1) + ": " + (relayState[i] ? "ON" : "OFF") + "\n";
    }
    advHome.getClient().publish("nodemcu/relay/status", response.c_str());
    Serial.println("All relays status sent.");
  }
}

void readAndPublishCurrent() {
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 1024;
  uint32_t start_time = millis();

  while ((millis() - start_time) < 1000) {
    readValue = analogRead(sensorIn);
    if (readValue > maxValue) maxValue = readValue;
    if (readValue < minValue) minValue = readValue;
  }

  result = ((maxValue - minValue) * 5.0) / 1024.0;
  Voltage = result;
  VRMS = (Voltage / 2.0) * 0.707;
  AmpsRMS = (VRMS * 1000) / mVperAmp;

  Serial.print("Voltage: ");
  Serial.print(Voltage, decimalPrecision);
  Serial.print(" V, VRMS: ");
  Serial.print(VRMS, decimalPrecision);
  Serial.print(" V, AmpsRMS: ");
  Serial.print(AmpsRMS, decimalPrecision);
  Serial.println(" A");

  String currentValue = String(AmpsRMS, decimalPrecision);
  advHome.getClient().publish("cust_67f22068e628a/Current/status", currentValue.c_str());
}

void setup() {
  Serial.begin(115200);

  // Setup relays
  for (int i = 0; i < 6; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // Relays off
  }

  // Connect WiFi using WiFiManager
  WiFiManager wm;
  wm.autoConnect("AutoConnectAP");

  // Begin AdvHomeIOT (includes firmware update + MQTT connect)
  advHome.begin();

  // Set MQTT callback
  advHome.getClient().setCallback(mqttCallback);
}

void loop() {
  advHome.loop();  // Handles MQTT connection + keepalive

  // Publish current sensor reading every 5 seconds
  unsigned long currentMillis = millis();
  if (currentMillis - lastCurrentPublish >= currentPublishInterval) {
    lastCurrentPublish = currentMillis;
    readAndPublishCurrent();
  }
}
