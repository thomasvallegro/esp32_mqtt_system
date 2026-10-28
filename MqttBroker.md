# Classe MqttBroker

## Descrizione
La classe `MqttBroker` implementa un broker MQTT conforme allo standard MQTT 3.1.1 leggero ed ottimizzato per l'esecuzione direttamente sul microcontrollore ESP32. Gestisce socket TCP asincroni sulla porta 1883, analizza i pacchetti di controllo ricevuti dai client e provvede all'instradamento automatico dei payload ai client sottoscrittori.

## Responsabilità
* Apertura del socket server TCP sulla porta specificata (default: `1883`).
* Accettazione delle connessioni in ingresso e creazione di oggetti `ClientSession`.
* Parsing dei pacchetti di controllo MQTT:
  * `CONNECT` (Type 1): convalida protocollo e risposta con `CONNACK` (Type 2).
  * `PUBLISH` (Type 3): estrazione di topic e payload, log diagnostico e broadcast ai sottoscrittori.
  * `SUBSCRIBE` (Type 8): registrazione dell'interesse del client verso un dato topic e risposta con `SUBACK` (Type 9).
  * `PINGREQ` (Type 12): risposta keep-alive con `PINGRESP` (Type 13).
  * `DISCONNECT` (Type 14): chiusura ordinata della sessione.
* Codifica corretta della lunghezza residua MQTT (Variable Byte Integer).
* Rilevamento e pulizia automatica dei client disconnessi.

## Metodi Principali
* `bool start()`: Avvia il server TCP e inizia ad ascoltare nuove connessioni.
* `void stop()`: Disconnette tutti i client e arresta il server TCP.
* `void process()`: Ciclo di polling da chiamare all'interno della funzione `loop()` dell'ESP32.
* `size_t getConnectedClientsCount() const`: Ritorna il numero di client MQTT attualmente connessi.
