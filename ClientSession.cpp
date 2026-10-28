#include "ClientSession.h"

ClientSession::ClientSession(WiFiClient client_socket)
  : _socket(client_socket),
    _client_id("unknown") {
}

ClientSession::~ClientSession() {
  disconnect();
}

bool ClientSession::isConnected() {
  return _socket.connected();
}

void ClientSession::disconnect() {
  if (_socket.connected()) {
    _socket.stop();
  }
  _subscriptions.clear();
}

WiFiClient& ClientSession::getSocket() {
  return _socket;
}

void ClientSession::setClientId(const String& client_id) {
  _client_id = client_id;
}

String ClientSession::getClientId() const {
  return _client_id;
}

void ClientSession::addSubscription(const String& topic) {
  if (!isSubscribedTo(topic)) {
    _subscriptions.push_back(topic);
  }
}

void ClientSession::removeSubscription(const String& topic) {
  for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it) {
    if (*it == topic) {
      _subscriptions.erase(it);
      break;
    }
  }
}

bool ClientSession::isSubscribedTo(const String& topic) const {
  for (const String& sub : _subscriptions) {
    if (sub == topic || sub == "#") {
      return true;
    }
  }
  return false;
}

bool ClientSession::sendRaw(const uint8_t* buffer, size_t length) {
  if (!_socket.connected()) {
    return false;
  }
  size_t bytes_written = _socket.write(buffer, length);
  _socket.flush();
  return (bytes_written == length);
}
