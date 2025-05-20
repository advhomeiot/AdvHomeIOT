#ifndef AdvHomeIOT_h
#define AdvHomeIOT_h

#include <ESP8266WiFi.h>  // Include the necessary Wi-Fi library for NodeMCU
#include <WiFiClientSecure.h>  // For secure MQTT communication (SSL/TLS)
#include <PubSubClient.h>  // For MQTT communication

// MQTT Broker settings (Directly mentioned here)
#define MQTT_SERVER "advhomeiot.shop"  // MQTT Broker IP (update if needed)
#define MQTT_PORT 8883  // MQTT Broker port (SSL/TLS)

class AdvHomeIOT {
  public:
    AdvHomeIOT(const char* token);  // Constructor to accept the token

    // Function to check if the device is connected to Wi-Fi
    bool isConnected();  
    
    // Function to retrieve the MAC address of the NodeMCU
    String getMACAddress();  
    
    // Function to connect to MQTT securely
    bool connectToMQTT(PubSubClient& client);

  private:
    String token;
    String macAddress;  // Store the MAC address
};

#endif

