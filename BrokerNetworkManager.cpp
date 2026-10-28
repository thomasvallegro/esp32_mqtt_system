#include "BrokerNetworkManager.h"

BrokerNetworkManager::BrokerNetworkManager(const char* ssid, const char* password, IPAddress local_ip, IPAddress gateway, IPAddress subnet)
  : _ssid(ssid),
    _password(password),
    _local_ip(local_ip),
    _gateway(gateway),
    _subnet(subnet),
    _is_running(false) {
}

BrokerNetworkManager::~BrokerNetworkManager() {
  stopAccessPoint();
}

bool BrokerNetworkManager::startAccessPoint() {
  WiFi.mode(WIFI_AP);
  bool config_ok = WiFi.softAPConfig(_local_ip, _gateway, _subnet);
  if (!config_ok) {
    Serial.println("[BrokerNetworkManager] Errore nella configurazione dell'IP statico SoftAP.");
    return false;
  }

  bool ap_ok = WiFi.softAP(_ssid, _password);
  if (ap_ok) {
    _is_running = true;
    Serial.println("[BrokerNetworkManager] Rete Wi-Fi SoftAP avviata con successo.");
    Serial.print("[BrokerNetworkManager] SSID: ");
    Serial.println(_ssid);
    Serial.print("[BrokerNetworkManager] IP Fisso Broker: ");
    Serial.println(WiFi.softAPIP());
    return true;
  } else {
    _is_running = false;
    Serial.println("[BrokerNetworkManager] Fallimento avvio SoftAP.");
    return false;
  }
}

void BrokerNetworkManager::stopAccessPoint() {
  if (_is_running) {
    WiFi.softAPdisconnect(true);
    _is_running = false;
    Serial.println("[BrokerNetworkManager] SoftAP arrestato.");
  }
}

bool BrokerNetworkManager::isRunning() const {
  return _is_running;
}

int BrokerNetworkManager::getConnectedStationsCount() const {
  return WiFi.softAPgetStationNum();
}

IPAddress BrokerNetworkManager::getLocalIp() const {
  return WiFi.softAPIP();
}
