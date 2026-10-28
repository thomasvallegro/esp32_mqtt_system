# Classe BrokerNetworkManager

## Descrizione
La classe `BrokerNetworkManager` si occupa della gestione dell'interfaccia di rete wireless per il nodo Broker MQTT su ESP32. 
Configura la scheda in modalità **SoftAP (Access Point)** con un indirizzo **IP fisso** predefinito (statico), creando una rete Wi-Fi locale privata e indipendente a cui gli altri nodi ESP32 (client/publisher) possono connettersi senza bisogno di router o infrastrutture esterne.

## Responsabilità
* Inizializzazione della modalità `WIFI_AP`.
* Configurazione dell'indirizzo IP statico, gateway e subnet mask tramite `WiFi.softAPConfig`.
* Avvio e monitoraggio della rete wireless tramite `WiFi.softAP`.
* Tracciamento del numero di stazioni client connesse alla rete.
* Gestione dello spegnimento pulito dell'interfaccia AP.

## Metodi Principali
* `bool startAccessPoint()`: Inizializza e attiva l'Access Point con IP fisso. Restituisce `true` in caso di successo.
* `void stopAccessPoint()`: Disattiva la rete Wi-Fi SoftAP.
* `bool isRunning() const`: Restituisce lo stato di esecuzione dell'Access Point.
* `int getConnectedStationsCount() const`: Ritorna il numero di periferiche connesse all'AP.
* `IPAddress getLocalIp() const`: Ritorna l'indirizzo IP effettivo dell'interfaccia SoftAP.
