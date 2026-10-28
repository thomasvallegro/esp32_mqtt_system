#pragma once

#include <WiFi.h>

/**
 * @brief Manages the Wi-Fi Access Point and network configuration for the MQTT Broker.
 * 
 * Configures the ESP32 in SoftAP mode with a fixed static IP address, allowing
 * other ESP32 nodes to connect to a private, isolated wireless network.
 */
class BrokerNetworkManager {
public:
  BrokerNetworkManager(const char* ssid, const char* password, IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  ~BrokerNetworkManager();

  bool startAccessPoint();
  void stopAccessPoint();
  bool isRunning() const;
  int getConnectedStationsCount() const;
  IPAddress getLocalIp() const;

private:
  const char* _ssid;
  const char* _password;
  IPAddress _local_ip;
  IPAddress _gateway;
  IPAddress _subnet;
  bool _is_running;
};
