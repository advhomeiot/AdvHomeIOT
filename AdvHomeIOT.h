#ifndef AdvHomeIOT_h
#define AdvHomeIOT_h

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

class AdvHomeIOT {
  public:
    AdvHomeIOT(const char* token);   // Only token required from user

    void begin();                    // Call in setup() after WiFi connection
    void loop();                     // Call in loop()
    PubSubClient& getClient();       // Access MQTT client for publish/subscribe

  private:
    const char* mqtt_server = "advhomeiot.shop";
    const int mqtt_port = 8883;
    const char* _token;
    String _mac;

    WiFiClientSecure _secureClient;
    PubSubClient _client;

    void connectMQTT();
    void checkFirmwareUpdate();
    bool authenticateToken();
};

#endif
