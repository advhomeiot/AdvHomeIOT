#include "AdvHomeIOT.h"

AdvHomeIOT::AdvHomeIOT(const char* token)
  : _token(token), _client(_secureClient) {}

void AdvHomeIOT::begin() {
  _mac = WiFi.macAddress();
  Serial.print("MAC Address: ");
  Serial.println(_mac);

  _secureClient.setInsecure();  // ⚠️ Replace with setCACert(...) in production
  _client.setServer(mqtt_server, mqtt_port);
}

bool AdvHomeIOT::authenticateToken() {
  // Send a request to the server to check if the token and MAC are valid
  // This part would need to interact with your server or an endpoint to validate credentials.
  // If valid, return true, otherwise return false.

  // For now, simulate successful authentication if the token is non-empty:
  if (_token && String(_token) != "") {
    return true;
  }

  return false;
}

void AdvHomeIOT::connectMQTT() {
  while (!_client.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "NodeMCUClient-";
    clientId += String(random(0xffff), HEX);

    if (authenticateToken()) {
      if (_client.connect(clientId.c_str(), _token, _mac.c_str())) {
        Serial.println("MQTT connected");
      } else {
        Serial.print("Failed, rc=");
        Serial.print(_client.state());
        Serial.println(" retrying in 5s...");
        delay(5000);
      }
    } else {
      Serial.println("Invalid token or MAC address. Retrying in 5 seconds...");
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
