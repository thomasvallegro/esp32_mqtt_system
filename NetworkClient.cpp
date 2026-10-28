#include "NetworkClient.h"

NetworkClient::NetworkClient(const char* ssid, const char* password)
  : _ssid(ssid),
    _password(password),
    _last_reconnect_attempt_ms(0) {
}

NetworkClient::~NetworkClient() {
  disconnect();
}

bool NetworkClient::connect() {
  Serial.print("[NetworkClient] Connessione alla rete del Broker Wi-Fi: ");
  Serial.println(_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[NetworkClient] Connessione stabilita! IP Assegnato: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("[NetworkClient] Tentativo iniziale di connessione fallito.");
    return false;
  }
}

void NetworkClient::disconnect() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect(true);
    Serial.println("[NetworkClient] Wi-Fi disconnesso.");
  }
}

bool NetworkClient::isConnected() const {
  return (WiFi.status() == WL_CONNECTED);
}

void NetworkClient::maintainConnection() {
  if (!isConnected()) {
    unsigned long current_time_ms = millis();
    if (current_time_ms - _last_reconnect_attempt_ms >= _RECONNECT_INTERVAL_MS) {
      _last_reconnect_attempt_ms = current_time_ms;
      Serial.println("[NetworkClient] Connessione Wi-Fi persa. Tentativo di riconnessione...");
      WiFi.reconnect();
    }
  }
}

IPAddress NetworkClient::getLocalIp() const {
  return WiFi.localIP();
}
