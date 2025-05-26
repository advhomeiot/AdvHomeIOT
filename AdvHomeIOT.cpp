#include "AdvHomeIOT.h"
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* AdvHomeIOT::_mqttServer = "advhomeiot.shop";
const int AdvHomeIOT::_mqttPort = 8883;
const char* AdvHomeIOT::_versionCheckURL = "https://advhomeiot.shop/advhomeiot/file/latest_version.txt";
const int AdvHomeIOT::_currentFWVersion = 5;

AdvHomeIOT::AdvHomeIOT(const char* token) : _token(token), _mqttClient(_wifiClientSecure), isConnected(false) {
    _mac = WiFi.macAddress();
}

void AdvHomeIOT::begin() {
    _wifiClientSecure.setInsecure();  // Accept any certificate
    _mqttClient.setServer(_mqttServer, _mqttPort);

    connectMQTT();
    checkForUpdates();
}

void AdvHomeIOT::connectMQTT() {
    while (!_mqttClient.connected()) {
        Serial.print("Connecting to MQTT with token: ");
        Serial.print(_token);
        Serial.print(" and MAC as password: ");
        Serial.println(_mac);

        if (_mqttClient.connect(("NodeMCU_" + _mac).c_str(), _token, _mac.c_str())) {
            Serial.println("MQTT connected");
            isConnected = true;
            // User handles subscriptions in sketch
        } else {
            Serial.print("MQTT connection failed, rc=");
            Serial.println(_mqttClient.state());
            delay(5000);
        }
    }
}

void AdvHomeIOT::checkForUpdates() {
    Serial.println("Checking firmware updates...");

    HTTPClient http;
    http.begin(_wifiClientSecure, _versionCheckURL);
    http.addHeader("User-Agent", "ESP8266-HTTP-Client");
    http.addHeader("Accept", "text/plain");

    int httpCode = http.GET();
    Serial.print("HTTP code: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
        String payload = http.getString();
        int newlineIndex = payload.indexOf('\n');

        if (newlineIndex != -1) {
            String newVersionStr = payload.substring(0, newlineIndex);
            String fwURL = payload.substring(newlineIndex + 1);
            int newVersion = newVersionStr.toInt();

            Serial.print("Current FW Version: ");
            Serial.println(_currentFWVersion);
            Serial.print("Available FW Version: ");
            Serial.println(newVersion);

            if (newVersion > _currentFWVersion) {
                Serial.println("New firmware available. Starting OTA update...");
                t_httpUpdate_return ret = ESPhttpUpdate.update(_wifiClientSecure, fwURL);

                switch (ret) {
                    case HTTP_UPDATE_FAILED:
                        Serial.printf("Update failed: (%d) %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                        break;
                    case HTTP_UPDATE_NO_UPDATES:
                        Serial.println("No updates available.");
                        break;
                    case HTTP_UPDATE_OK:
                        Serial.println("Update successful, restarting...");
                        delay(1000);
                        ESP.restart();
                        break;
                }
            } else {
                Serial.println("Firmware is up to date.");
            }
        } else {
            Serial.println("Invalid response format from update server.");
        }
    } else {
        Serial.println("Failed to check firmware updates.");
    }

    http.end();
}

void AdvHomeIOT::loop() {
    if (!_mqttClient.connected()) {
        connectMQTT();
    }
    _mqttClient.loop();
}

PubSubClient& AdvHomeIOT::getClient() {
    return _mqttClient;
}
