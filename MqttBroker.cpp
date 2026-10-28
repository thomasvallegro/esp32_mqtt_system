#include "MqttBroker.h"

MqttBroker::MqttBroker(uint16_t port)
  : _port(port),
    _server(port) {
}

MqttBroker::~MqttBroker() {
  stop();
}

bool MqttBroker::start() {
  _server.begin();
  Serial.print("[MqttBroker] Broker MQTT in ascolto sulla porta TCP: ");
  Serial.println(_port);
  return true;
}

void MqttBroker::stop() {
  _server.stop();
  for (auto& session : _sessions) {
    if (session) {
      session->disconnect();
    }
  }
  _sessions.clear();
  Serial.println("[MqttBroker] Broker MQTT arrestato.");
}

void MqttBroker::process() {
  _acceptNewClients();
  _cleanDisconnectedClients();

  for (auto& session : _sessions) {
    if (session && session->isConnected()) {
      _handleClientData(session);
    }
  }
}

size_t MqttBroker::getConnectedClientsCount() const {
  return _sessions.size();
}

void MqttBroker::_acceptNewClients() {
  WiFiClient new_client = _server.available();
  if (new_client) {
    Serial.print("[MqttBroker] Nuova connessione client in arrivo da IP: ");
    Serial.println(new_client.remoteIP());
    auto session = std::make_shared<ClientSession>(new_client);
    _sessions.push_back(session);
  }
}

void MqttBroker::_cleanDisconnectedClients() {
  for (auto it = _sessions.begin(); it != _sessions.end();) {
    if (!(*it)->isConnected()) {
      Serial.print("[MqttBroker] Disconnessione client: ");
      Serial.println((*it)->getClientId());
      it = _sessions.erase(it);
    } else {
      ++it;
    }
  }
}

void MqttBroker::_handleClientData(std::shared_ptr<ClientSession>& session) {
  WiFiClient& socket = session->getSocket();
  int available_bytes = socket.available();
  if (available_bytes <= 0) {
    return;
  }

  uint8_t buffer[512];
  size_t bytes_to_read = (available_bytes > 512) ? 512 : available_bytes;
  size_t bytes_read = socket.read(buffer, bytes_to_read);

  if (bytes_read < 2) {
    return;
  }

  uint8_t packet_type = (buffer[0] >> 4) & 0x0F;

  switch (packet_type) {
    case 1: // CONNECT
      _handleConnect(session, buffer, bytes_read);
      break;

    case 3: // PUBLISH
      _handlePublish(session, buffer, bytes_read);
      break;

    case 8: // SUBSCRIBE
      _handleSubscribe(session, buffer, bytes_read);
      break;

    case 12: // PINGREQ
      _handlePingReq(session);
      break;

    case 14: // DISCONNECT
      Serial.print("[MqttBroker] Ricevuto DISCONNECT da client: ");
      Serial.println(session->getClientId());
      session->disconnect();
      break;

    default:
      Serial.print("[MqttBroker] Tipo pacchetto non gestito: ");
      Serial.println(packet_type);
      break;
  }
}

void MqttBroker::_handleConnect(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length) {
  // Extract client ID from payload if present
  if (length > 12) {
    size_t cid_len_offset = 12;
    if (length >= cid_len_offset + 2) {
      uint16_t cid_len = (buffer[cid_len_offset] << 8) | buffer[cid_len_offset + 1];
      if (length >= cid_len_offset + 2 + cid_len) {
        String client_id = "";
        for (size_t i = 0; i < cid_len; ++i) {
          client_id += (char)buffer[cid_len_offset + 2 + i];
        }
        session->setClientId(client_id);
      }
    }
  }

  Serial.print("[MqttBroker] Client autenticato con ID: ");
  Serial.println(session->getClientId());

  // Invia CONNACK (Packet Type 2, Remaining Length 2, Connect Acknowledge Flags 0, Return Code 0 = Accepted)
  const uint8_t connack_packet[] = { 0x20, 0x02, 0x00, 0x00 };
  session->sendRaw(connack_packet, sizeof(connack_packet));
}

void MqttBroker::_handlePublish(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length) {
  size_t offset = 1;
  size_t remaining_length = _readRemainingLength(buffer, length, offset);
  if (offset + 2 > length) {
    return;
  }

  uint16_t topic_len = (buffer[offset] << 8) | buffer[offset + 1];
  offset += 2;

  if (offset + topic_len > length) {
    return;
  }

  String topic = "";
  for (size_t i = 0; i < topic_len; ++i) {
    topic += (char)buffer[offset++];
  }

  size_t payload_len = length - offset;
  String payload = "";
  for (size_t i = 0; i < payload_len; ++i) {
    payload += (char)buffer[offset++];
  }

  Serial.printf("[MqttBroker] PUBLISH da [%s] -> Topic: '%s' | Payload: '%s'\n",
                session->getClientId().c_str(), topic.c_str(), payload.c_str());

  _broadcastPublish(topic, payload);
}

void MqttBroker::_handleSubscribe(std::shared_ptr<ClientSession>& session, const uint8_t* buffer, size_t length) {
  if (length < 6) {
    return;
  }

  size_t offset = 1;
  size_t remaining_length = _readRemainingLength(buffer, length, offset);

  uint16_t packet_id = (buffer[offset] << 8) | buffer[offset + 1];
  offset += 2;

  while (offset + 2 < length) {
    uint16_t topic_len = (buffer[offset] << 8) | buffer[offset + 1];
    offset += 2;

    if (offset + topic_len <= length) {
      String topic = "";
      for (size_t i = 0; i < topic_len; ++i) {
        topic += (char)buffer[offset++];
      }

      uint8_t qos = 0;
      if (offset < length) {
        qos = buffer[offset++];
      }

      session->addSubscription(topic);
      Serial.printf("[MqttBroker] Sottoscrizione registrata per [%s] sul topic: '%s'\n",
                    session->getClientId().c_str(), topic.c_str());
    }
  }

  // Invia SUBACK (Packet Type 9, Remaining Length 3, Packet ID msb, lsb, Return Code 0x00 QoS 0)
  uint8_t suback_packet[] = {
    0x90, 0x03,
    (uint8_t)((packet_id >> 8) & 0xFF),
    (uint8_t)(packet_id & 0xFF),
    0x00
  };
  session->sendRaw(suback_packet, sizeof(suback_packet));
}

void MqttBroker::_handlePingReq(std::shared_ptr<ClientSession>& session) {
  // Invia PINGRESP (Packet Type 13, Remaining Length 0)
  const uint8_t pingresp_packet[] = { 0xD0, 0x00 };
  session->sendRaw(pingresp_packet, sizeof(pingresp_packet));
}

void MqttBroker::_broadcastPublish(const String& topic, const String& payload) {
  uint16_t topic_len = topic.length();
  size_t payload_len = payload.length();
  size_t remaining_length = 2 + topic_len + payload_len;

  std::vector<uint8_t> packet;
  packet.push_back(0x30); // PUBLISH, QoS 0, no retain

  // Remaining length encoding
  size_t temp_len = remaining_length;
  do {
    uint8_t encoded_byte = temp_len % 128;
    temp_len /= 128;
    if (temp_len > 0) {
      encoded_byte |= 128;
    }
    packet.push_back(encoded_byte);
  } while (temp_len > 0);

  // Topic length & Topic string
  packet.push_back((topic_len >> 8) & 0xFF);
  packet.push_back(topic_len & 0xFF);
  for (size_t i = 0; i < topic_len; ++i) {
    packet.push_back(topic[i]);
  }

  // Payload
  for (size_t i = 0; i < payload_len; ++i) {
    packet.push_back(payload[i]);
  }

  // Forward to all clients subscribed to this topic
  for (auto& session : _sessions) {
    if (session && session->isConnected() && session->isSubscribedTo(topic)) {
      session->sendRaw(packet.data(), packet.size());
    }
  }
}

size_t MqttBroker::_readRemainingLength(const uint8_t* buffer, size_t total_length, size_t& offset) {
  size_t multiplier = 1;
  size_t value = 0;
  uint8_t encoded_byte = 0;
  do {
    if (offset >= total_length) {
      break;
    }
    encoded_byte = buffer[offset++];
    value += (encoded_byte & 127) * multiplier;
    multiplier *= 128;
  } while ((encoded_byte & 128) != 0);

  return value;
}
