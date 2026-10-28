# Changelog

Tutti i cambiamenti e le versioni di questo progetto sono documentati in questo file secondo le linee guida del [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-10-28

### Aggiunto
- **Nodo 1 - Broker MQTT Privato (`esp32_broker_node`)**:
  - `BrokerNetworkManager`: creazione SoftAP Wi-Fi ad isolamento locale con IP fisso (`192.168.4.1`).
  - `ClientSession`: gestione ad oggetti delle sessioni TCP client e delle sottoscrizioni a topic.
  - `MqttBroker`: broker MQTT 3.1.1 integrato autonomo (CONNECT, CONNACK, PUBLISH, SUBSCRIBE, SUBACK, PINGREQ, PINGRESP, DISCONNECT) senza dipendenze da librerie esterne.
  - Documentazione specifica `.md` per ciascuna classe.
- **Nodo 2 - Client Telemetria & Controllo LED (`esp32_client_node`)**:
  - `NetworkClient`: gestione connessione automatica all'Access Point del Broker con riconnessione non bloccante.
  - `TemperatureSensor`: campionamento della temperatura della CPU interna all'ESP32 tramite `temperatureRead()`.
  - `LedController`: attuazione hardware e commutazione del LED onboard su GPIO 2.
  - `MqttClient`: client MQTT embedded conforme con callback per dispatching comandi e keep-alive ping.
  - Pubblicazione automatica temperatura su topic fisso: `esp32/cpu/temperature`.
  - Ricezione ed esecuzione comandi di commutazione LED su topic: `esp32/led/command`.
  - Documentazione specifica `.md` per ciascuna classe.
- **Documentazione & Repository**:
  - `README.md` esaustivo con architettura del sistema, contratti dei topic, schema di rete e guida al flashing.
  - `.gitignore` per artefatti di compilazione Arduino.
