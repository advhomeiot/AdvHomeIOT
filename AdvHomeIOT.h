#ifndef AdvHomeIOT_h
#define AdvHomeIOT_h

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

class AdvHomeIOT {
  public:
    AdvHomeIOT(const char* token);  // Constructor takes only the token

    void beginMQTT();               // Starts MQTT connection
    void loop();                    // Call in main loop
    void publish(const char* topic, const char* payload);  // Optional utility

  private:
    const char* mqtt_server = "advhomeiot.shop";
    const int mqtt_port = 8883;
    const char* _token;
    String _mac;
    WiFiClientSecure _secureClient;
    PubSubClient _client;

    void connectMQTT();
};

#endif
