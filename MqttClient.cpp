#include "MqttClient.h"

MqttClient::MqttClient(IPAddress broker_ip, uint16_t broker_port, const String& client_id)
  : _broker_ip(broker_ip),
    _broker_port(broker_port),
    _client_id(client_id),
    _callback(nullptr),
    _packet_id_counter(1),
    _last_ping_ms(0) {
}

MqttClient::~MqttClient() {
  disconnect();
}

bool MqttClient::connect() {
  if (_socket.connected()) {
    return true;
  }

  Serial.printf("[MqttClient] Tentativo connessione TCP a Broker %s:%d...\n", 
                _broker_ip.toString().c_str(), _broker_port);
  if (!_socket.connect(_broker_ip, _broker_port, 4000)) {
    Serial.println("[MqttClient] Errore connessione socket TCP.");
    return false;
  }

  if (!_sendConnectPacket()) {
    Serial.println("[MqttClient] Invio CONNECT fallito.");
    _socket.stop();
    return false;
  }

  if (!_readConnack()) {
    Serial.println("[MqttClient] Mancata ricezione CONNACK valido.");
    _socket.stop();
    return false;
  }

  _last_ping_ms = millis();
  Serial.println("[MqttClient] Connesso con successo al Broker MQTT!");
  return true;
}

void MqttClient::disconnect() {
  if (_socket.connected()) {
    const uint8_t disconnect_packet[] = { 0xE0, 0x00 };
    _socket.write(disconnect_packet, sizeof(disconnect_packet));
    _socket.stop();
    Serial.println("[MqttClient] Disconnessione eseguita.");
  }
}

bool MqttClient::isConnected() {
  return _socket.connected();
}

void MqttClient::loop() {
  if (!isConnected()) {
    return;
  }

  _processIncomingData();

  unsigned long current_time_ms = millis();
  if (current_time_ms - _last_ping_ms >= _PING_INTERVAL_MS) {
    _last_ping_ms = current_time_ms;
    _sendPingReq();
  }
}

bool MqttClient::subscribe(const String& topic) {
  if (!isConnected()) {
    return false;
  }

  uint16_t topic_len = topic.length();
  size_t remaining_length = 2 + 2 + topic_len + 1; // packet ID (2) + topic len (2) + topic + qos (1)

  std::vector<uint8_t> packet;
  packet.push_back(0x82); // SUBSCRIBE, QoS 1
  packet.push_back((uint8_t)remaining_length);

  uint16_t packet_id = _packet_id_counter++;
  packet.push_back((packet_id >> 8) & 0xFF);
  packet.push_back(packet_id & 0xFF);

  packet.push_back((topic_len >> 8) & 0xFF);
  packet.push_back(topic_len & 0xFF);
  for (size_t i = 0; i < topic_len; ++i) {
    packet.push_back(topic[i]);
  }
  packet.push_back(0x00); // Requested QoS 0

  size_t written = _socket.write(packet.data(), packet.size());
  _socket.flush();
  Serial.printf("[MqttClient] SUBSCRIBE inviato per topic: '%s'\n", topic.c_str());
  return (written == packet.size());
}

bool MqttClient::publish(const String& topic, const String& payload) {
  if (!isConnected()) {
    return false;
  }

  uint16_t topic_len = topic.length();
  size_t payload_len = payload.length();
  size_t remaining_length = 2 + topic_len + payload_len;

  std::vector<uint8_t> packet;
  packet.push_back(0x30); // PUBLISH, QoS 0, no retain

  size_t temp_len = remaining_length;
  do {
    uint8_t encoded_byte = temp_len % 128;
    temp_len /= 128;
    if (temp_len > 0) {
      encoded_byte |= 128;
    }
    packet.push_back(encoded_byte);
  } while (temp_len > 0);

  packet.push_back((topic_len >> 8) & 0xFF);
  packet.push_back(topic_len & 0xFF);
  for (size_t i = 0; i < topic_len; ++i) {
    packet.push_back(topic[i]);
  }

  for (size_t i = 0; i < payload_len; ++i) {
    packet.push_back(payload[i]);
  }

  size_t written = _socket.write(packet.data(), packet.size());
  _socket.flush();
  return (written == packet.size());
}

void MqttClient::setMessageCallback(MessageCallback callback) {
  _callback = callback;
}

bool MqttClient::_sendConnectPacket() {
  uint16_t client_id_len = _client_id.length();
  const char* protocol_name = "MQTT";
  uint16_t protocol_len = 4;
  uint8_t protocol_level = 4; // MQTT 3.1.1
  uint8_t connect_flags = 0x02; // Clean session
  uint16_t keep_alive = 60; // seconds

  size_t variable_header_len = 2 + protocol_len + 1 + 1 + 2; // 10 bytes
  size_t payload_len = 2 + client_id_len;
  size_t remaining_length = variable_header_len + payload_len;

  std::vector<uint8_t> packet;
  packet.push_back(0x10); // CONNECT
  packet.push_back((uint8_t)remaining_length);

  // Variable Header
  packet.push_back(0x00);
  packet.push_back(protocol_len);
  for (size_t i = 0; i < protocol_len; ++i) {
    packet.push_back(protocol_name[i]);
  }
  packet.push_back(protocol_level);
  packet.push_back(connect_flags);
  packet.push_back((keep_alive >> 8) & 0xFF);
  packet.push_back(keep_alive & 0xFF);

  // Payload: Client ID
  packet.push_back((client_id_len >> 8) & 0xFF);
  packet.push_back(client_id_len & 0xFF);
  for (size_t i = 0; i < client_id_len; ++i) {
    packet.push_back(_client_id[i]);
  }

  size_t written = _socket.write(packet.data(), packet.size());
  _socket.flush();
  return (written == packet.size());
}

bool MqttClient::_readConnack() {
  unsigned long start_time_ms = millis();
  while (_socket.available() < 4 && (millis() - start_time_ms < 3000)) {
    delay(10);
  }

  if (_socket.available() < 4) {
    return false;
  }

  uint8_t response[4];
  _socket.read(response, 4);

  // response[0] == 0x20 (CONNACK), response[3] == 0x00 (Accepted)
  return (response[0] == 0x20 && response[1] == 0x02 && response[3] == 0x00);
}

void MqttClient::_processIncomingData() {
  int available_bytes = _socket.available();
  if (available_bytes <= 0) {
    return;
  }

  uint8_t buffer[512];
  size_t bytes_to_read = (available_bytes > 512) ? 512 : available_bytes;
  size_t bytes_read = _socket.read(buffer, bytes_to_read);

  if (bytes_read < 2) {
    return;
  }

  uint8_t packet_type = (buffer[0] >> 4) & 0x0F;
  if (packet_type == 3) { // PUBLISH
    size_t offset = 1;
    // skip remaining length
    while (offset < bytes_read && (buffer[offset++] & 128) != 0) { }

    if (offset + 2 <= bytes_read) {
      uint16_t topic_len = (buffer[offset] << 8) | buffer[offset + 1];
      offset += 2;

      if (offset + topic_len <= bytes_read) {
        String topic = "";
        for (size_t i = 0; i < topic_len; ++i) {
          topic += (char)buffer[offset++];
        }

        size_t payload_len = bytes_read - offset;
        String payload = "";
        for (size_t i = 0; i < payload_len; ++i) {
          payload += (char)buffer[offset++];
        }

        if (_callback) {
          _callback(topic, payload);
        }
      }
    }
  }
}

void MqttClient::_sendPingReq() {
  const uint8_t pingreq_packet[] = { 0xC0, 0x00 };
  _socket.write(pingreq_packet, sizeof(pingreq_packet));
  _socket.flush();
}
