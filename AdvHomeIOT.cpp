#include <AdvHomeIOT.h>
#include <ESP8266WiFi.h>

// Constructor (optional)
AdvHomeIOT::AdvHomeIOT() {
  // Optional initialization code
}

// This function checks if the device is connected to Wi-Fi
bool AdvHomeIOT::isConnected() {
  // Check Wi-Fi connection status
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
  
  // Format MAC address into a string
  for (int i = 0; i < 6; i++) {
    macAddress += String(mac[i], HEX);  // Convert to HEX
    if (i < 5) macAddress += ":";  // Add colon between bytes
  }
  
  return macAddress;  // Return formatted MAC address
}
