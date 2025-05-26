#ifndef AdvHomeIOT_h
#define AdvHomeIOT_h

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define FW_VERSION 5
extern const char* VERSION_CHECK_URL;

class AdvHomeIOT {
  public:
    AdvHomeIOT(const char* token);

    void begin();
    void loop();
    PubSubClient& getClient();

  private:
    const char* mqtt_server = "advhomeiot.shop";
    const int mqtt_port = 8883;

    const char* _token;
    String _mac;
    String _clientId;

    WiFiClientSecure _secureClient;
    PubSubClient _client;

    void connectMQTT();
    bool authenticateToken();
    void checkFirmwareUpdate();
};

#endif
