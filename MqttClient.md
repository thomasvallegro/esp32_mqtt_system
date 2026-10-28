# Classe MqttClient

## Descrizione
La classe `MqttClient` fornisce un'implementazione autonoma e leggera del protocollo client MQTT 3.1.1 per ESP32. Consente al nodo di connettersi via TCP al Broker ad IP fisso, effettuare la sottoscrizione a topic di comando e pubblicare letture di telemetria (come la temperatura CPU) senza dover ricorrere a pesanti librerie di terze parti.

## Responsabilità
* Gestione della connessione socket TCP verso `broker_ip:broker_port`.
* Invio del pacchetto di handshake `CONNECT` e verifica della risposta `CONNACK`.
* Invio di richieste di sottoscrizione `SUBSCRIBE` a topic specifici.
* Serializzazione e invio di messaggi `PUBLISH` con payload arbitrari in formato stringa.
* Parsing in ricezione dei messaggi `PUBLISH` provenienti dal broker e inoltro al callback applicativo registrato (`MessageCallback`).
* Invio periodico di pacchetti keep-alive `PINGREQ` per mantenere viva la connessione.

## Metodi Principali
* `bool connect()`: Apre il socket TCP e completa la negoziazione MQTT con il broker.
* `void disconnect()`: Invia il pacchetto `DISCONNECT` e arresta il socket.
* `bool isConnected()`: Verifica lo stato di connessione TCP.
* `void loop()`: Metodo non bloccante da invocare ciclicamente nel `loop()` principale per gestire I/O e keep-alive.
* `bool subscribe(const String& topic)`: Effettua la sottoscrizione a un topic.
* `bool publish(const String& topic, const String& payload)`: Pubblica un messaggio su un topic.
* `void setMessageCallback(MessageCallback callback)`: Registra la funzione lambda o callback richiamata alla ricezione di messaggi sui topic sottoscritti.
