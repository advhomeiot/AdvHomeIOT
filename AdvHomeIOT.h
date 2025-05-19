#ifndef AdvHomeIOT_h
#define AdvHomeIOT_h

#include <ESP8266WiFi.h>  // Include the necessary Wi-Fi library for NodeMCU

class AdvHomeIOT {
  public:
    AdvHomeIOT();  // Constructor for the class (optional)

    // Function to check if the device is connected to Wi-Fi and internet
    bool isConnected();  
    
    // Function to retrieve the MAC address of the NodeMCU
    String getMACAddress();  
    
  private:
    // No need to store SSID/password in the header, as user handles them
};

#endif
