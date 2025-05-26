#include "AdvHomeIOT.h"
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

const char* VERSION_CHECK_URL = "https://advhomeiot.shop/advhomeiot/file/latest_version.txt";

AdvHomeIOT::AdvHomeIOT(const char* token)
  : _token(token), _client(_secureClient) {}

void AdvHomeIOT::begin() {
  _mac = WiFi.macAddress();
  _clientId = "NodeMCU_" + String(ESP.getChipId());

  Serial.print("📟 MAC Address: ");
  Serial.println(_mac);
  Serial.print("🆔 Client ID: ");
  Serial.println(_clientId);

  _secureClient.setInsecure();  // Accept all certs (insecure for dev)
  _client.setServer(mqtt_server, mqtt_port);

  checkFirmwareUpdate();  // Check firmware on boot
}

void AdvHomeIOT::checkFirmwareUpdate() {
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure();

  Serial.println("🔍 Checking firmware update...");

  if (http.begin(client, VERSION_CHECK_URL)) {
    int httpCode = http.GET();
    if (httpCode == 200) {
      String response = http.getString();
      int split = response.indexOf('\n');
      if (split > 0) {
        int newVersion = response.substring(0, split).toInt();
        String firmwareURL = response.substring(split + 1);

        Serial.printf("📦 FW Current: %d, Available: %d\n", FW_VERSION, newVersion);

        if (newVersion > FW_VERSION) {
          Serial.println("⚠️ New firmware found. Starting update...");
          t_httpUpdate_return ret = ESPhttpUpdate.update(client, firmwareURL);
          if (ret == HTTP_UPDATE_OK) {
            Serial.println("✅ Update successful. Rebooting...");
            ESP.restart();
          } else {
            Serial.printf("❌ Update failed: (%d) %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          }
        } else {
          Serial.println("✔️ Firmware is up to date.");
        }
      } else {
        Serial.println("⚠️ Invalid version response format.");
      }
    } else {
      Serial.printf("❌ HTTP error: %d\n", httpCode);
    }
    http.end();
  } else {
    Serial.println("❌ Unable to connect to version server.");
  }
}

bool AdvHomeIOT::authenticateToken() {
  return (_token && strlen(_token) > 0);
}

void AdvHomeIOT::connectMQTT() {
  while (!_client.connected()) {
    Serial.print("🔌 Connecting to MQTT...");
    if (authenticateToken()) {
      if (_client.connect(_clientId.c_str(), _token, _mac.c_str())) {
        Serial.println("✅ MQTT connected");
      } else {
        Serial.printf("❌ MQTT connect failed. State: %d\n", _client.state());
        delay(5000);
      }
    } else {
      Serial.println("❌ Invalid token. Retrying...");
      delay(5000);
    }
  }
}

void AdvHomeIOT::loop() {
  if (!_client.connected()) {
    connectMQTT();
  }
  _client.loop();
}

PubSubClient& AdvHomeIOT::getClient() {
  return _client;
}
