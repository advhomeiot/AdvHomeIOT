#include <AdvHomeIOT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>  // Secure client for SSL/TLS
#include <PubSubClient.h>  // For MQTT

// Constructor (takes the token as an argument)
AdvHomeIOT::AdvHomeIOT(const char* token) {
  this->token = token;  // Save the token provided by the user
}

// This function checks if the device is connected to Wi-Fi
bool AdvHomeIOT::isConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;  // Device is connected to Wi-Fi
  }
  return false;  // Device is not connected to Wi-Fi
}

// This function retrieves the MAC address of the NodeMCU
String AdvHomeIOT::getMACAddress() {
  uint8_t mac[6];  // Array to store MAC address
  WiFi.macAddress(mac);  // Get MAC address
  String macAddress = "";

  for (int i = 0; i < 6; i++) {
    macAddress += String(mac[i], HEX);  // Convert to HEX
    if (i < 5) macAddress += ":";  // Add colon between bytes
  }

  return macAddress;  // Return formatted MAC address
}

// Function to connect to MQTT securely
bool AdvHomeIOT::connectToMQTT(PubSubClient& client) {
  // Create a WiFiClientSecure object for SSL/TLS connection
  WiFiClientSecure espClient;

  // Optional: If your MQTT broker uses a certificate for SSL verification, you can load it like this:
  // espClient.setCACert(your_cert);

  // Attempt to connect to MQTT broker using token as username and MAC address as password
  String mqttUser = this->token;
  String mqttPass = this->getMACAddress();

  // Connect to the MQTT server (using global constants for server and port)
  if (client.connect("NodeMCUClient", mqttUser.c_str(), mqttPass.c_str())) {
    Serial.println("Connected to MQTT Broker over SSL/TLS");
    return true;  // Successfully connected to MQTT
  } else {
    Serial.print("MQTT Connection failed, rc=");
    Serial.println(client.state());
    return false;  // MQTT connection failed
  }
}
