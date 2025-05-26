#include "AdvHomeIOT.h"

#define CURRENT_FW_VERSION 5
const char* versionCheckURL = "https://advhomeiot.shop/advhomeiot/file/latest_version.txt";

AdvHomeIOT::AdvHomeIOT(const char* token)
  : _token(token), _client(_secureClient) {}

void AdvHomeIOT::begin() {
  _mac = WiFi.macAddress();
  Serial.print("📟 MAC Address: ");
  Serial.println(_mac);

  _secureClient.setInsecure();  // ⚠️ Accept all certs (for dev only)
  _client.setServer(mqtt_server, mqtt_port);

  checkFirmwareUpdate();        // ✅ Firmware update check at startup
}

bool AdvHomeIOT::authenticateToken() {
  return (_token && String(_token) != "");
}

void AdvHomeIOT::connectMQTT() {
  while (!_client.connected()) {
    Serial.print("🔌 Connecting to MQTT...");

    String clientId = "NodeMCUClient-" + String(random(0xffff), HEX);

    if (authenticateToken()) {
      if (_client.connect(clientId.c_str(), _token, _mac.c_str())) {
        Serial.println("✅ MQTT connected");
      } else {
        Serial.print("❌ MQTT failed, rc=");
        Serial.println(_client.state());
        delay(5000);
      }
    } else {
      Serial.println("⚠️ Invalid token. Retrying in 5s...");
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

// ✅ Firmware Update Logic
void AdvHomeIOT::checkFirmwareUpdate() {
  Serial.println("🔍 Checking for firmware update...");
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  if (!http.begin(client, versionCheckURL)) {
    Serial.println("❌ Failed to connect to update server");
    return;
  }

  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    int splitIndex = payload.indexOf('\n');

    if (splitIndex > 0) {
      String versionStr = payload.substring(0, splitIndex);
      String firmwareURL = payload.substring(splitIndex + 1);
      int latestVersion = versionStr.toInt();

      Serial.printf("🔧 Current FW: %d, Available FW: %d\n", CURRENT_FW_VERSION, latestVersion);

      if (latestVersion > CURRENT_FW_VERSION) {
        Serial.println("⬆️ Updating firmware...");
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, firmwareURL);

        switch (ret) {
          case HTTP_UPDATE_OK:
            Serial.println("✅ Firmware updated successfully");
            break;
          case HTTP_UPDATE_FAILED:
            Serial.printf("❌ Update failed: %s\n", ESPhttpUpdate.getLastErrorString().c_str());
            break;
          case HTTP_UPDATE_NO_UPDATES:
            Serial.println("ℹ️ No updates found");
            break;
        }
      } else {
        Serial.println("✅ Already on latest firmware");
      }
    } else {
      Serial.println("❌ Invalid update response format");
    }
  } else {
    Serial.printf("❌ Update check failed, HTTP code: %d\n", httpCode);
  }

  http.end();
}
