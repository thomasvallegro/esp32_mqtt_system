#pragma once

#include <WiFi.h>
#include <vector>

/**
 * @brief Represents an active client connection to the MQTT Broker.
 * 
 * Stores the underlying TCP client socket, the MQTT client identifier,
 * and the list of subscribed topics for message routing.
 */
class ClientSession {
public:
  explicit ClientSession(WiFiClient client_socket);
  ~ClientSession();

  bool isConnected();
  void disconnect();
  WiFiClient& getSocket();
  
  void setClientId(const String& client_id);
  String getClientId() const;

  void addSubscription(const String& topic);
  void removeSubscription(const String& topic);
  bool isSubscribedTo(const String& topic) const;

  bool sendRaw(const uint8_t* buffer, size_t length);

private:
  WiFiClient _socket;
  String _client_id;
  std::vector<String> _subscriptions;
};
