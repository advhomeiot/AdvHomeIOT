#ifndef ADVHOMEIOT_H
#define ADVHOMEIOT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class AdvHomeIOT {
public:
    AdvHomeIOT(const char* token);

    void begin();   // Start MQTT connection + check updates
    void loop();    // Maintain connection + MQTT loop

    PubSubClient& getClient();  // Access MQTT client (subscribe/publish)

private:
    const char* _token;
    String _mac;
    WiFiClientSecure _wifiClientSecure;
    PubSubClient _mqttClient;

    static const char* _mqttServer;
    static const int _mqttPort;
    static const char* _versionCheckURL;
    static const int _currentFWVersion;

    void connectMQTT();
    void checkForUpdates();
    bool isConnected;
};

#endif
