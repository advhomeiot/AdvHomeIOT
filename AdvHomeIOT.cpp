#include "AdvHomeIOT.h"   // Include the header file

// Constructor (optional)
AdvHomeIOT::AdvHomeIOT() {
  // Optional initialization code
}

// This function checks if the device is connected to both Wi-Fi and the internet
bool AdvHomeIOT::isConnected() {
  // Check Wi-Fi connection status
  if (WiFi.status() == WL_CONNECTED) {
    // Optionally, you can check internet access here by pinging a known server
    if (WiFi.ping("google.com") >= 0) {  // Test internet connection (ping google)
      return true;  // Device is connected to both Wi-Fi and the internet
    }
  }
  return false;  // If not connected to Wi-Fi or internet
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
