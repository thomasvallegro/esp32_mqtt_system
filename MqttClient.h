#pragma once

#include <WiFi.h>
#include <functional>

/**
 * @brief Lightweight embedded MQTT 3.1.1 Client for ESP32.
 * 
 * Connects to an external or local MQTT Broker over TCP, supports subscribing
 * to topics with callback dispatching, and publishing string payloads.
 */
class MqttClient {
public:
  typedef std::function<void(const String& topic, const String& payload)> MessageCallback;

  MqttClient(IPAddress broker_ip, uint16_t broker_port, const String& client_id);
  ~MqttClient();

  bool connect();
  void disconnect();
  bool isConnected();
  void loop();

  bool subscribe(const String& topic);
  bool publish(const String& topic, const String& payload);
  void setMessageCallback(MessageCallback callback);

private:
  IPAddress _broker_ip;
  uint16_t _broker_port;
  String _client_id;
  WiFiClient _socket;
  MessageCallback _callback;
  uint16_t _packet_id_counter;

  unsigned long _last_ping_ms;
  const unsigned long _PING_INTERVAL_MS = 15000;

  bool _sendConnectPacket();
  bool _readConnack();
  void _processIncomingData();
  void _sendPingReq();
};
