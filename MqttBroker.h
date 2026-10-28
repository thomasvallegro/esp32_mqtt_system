#pragma once

#include <WiFi.h>
#include <vector>
#include <memory>
#include "ClientSession.h"

/**
 * @brief Embedded MQTT Broker for ESP32.
 * 
 * Listens for incoming TCP connections on port 1883, parses MQTT 3.1.1 control
 * packets (CONNECT, PUBLISH, SUBSCRIBE, PINGREQ, DISCONNECT), and dispatches
 * published payloads to all subscribed clients.
 */
class MqttBroker {
public:
  explicit MqttBroker(uint16_t port = 1883);
  ~MqttBroker();

  bool start();
  void stop();
  void process();

  size_t getConnectedClientsCount() const;

private:
  uint16_t _port;
  WiFiServer _server;
  std::vector<std::shared_ptr<ClientSession>> _sessions;

  void _acceptNewClients();
  void _cleanDisconnectedClients();
  void _handleClientData(std::shared_ptr<ClientSession>& session);

  void _handleConnect(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length);
  void _handlePublish(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length);
  void _handleSubscribe(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length);
  void _handlePingReq(std::shared_ptr<ClientSession>& session);

  void _broadcastPublish(const String& topic, const String& payload);
  static size_t _readRemainingLength(const uint8_t* buffer, size_t total_length, size_t& offset);
};
