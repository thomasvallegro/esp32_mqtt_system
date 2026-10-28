#pragma once

#include <WiFi.h>

/**
 * @brief Manages the Wi-Fi station connection for the MQTT Client node.
 * 
 * Handles connecting to the private SoftAP created by the Broker node,
 * tracking connection state and automatic reconnection.
 */
class NetworkClient {
public:
  NetworkClient(const char* ssid, const char* password);
  ~NetworkClient();

  bool connect();
  void disconnect();
  bool isConnected() const;
  void maintainConnection();
  IPAddress getLocalIp() const;

private:
  const char* _ssid;
  const char* _password;
  unsigned long _last_reconnect_attempt_ms;
  const unsigned long _RECONNECT_INTERVAL_MS = 5000;
};
