# Classe NetworkClient

## Descrizione
La classe `NetworkClient` gestisce la connettività di rete in modalità stazione (`WIFI_STA`) per il secondo nodo ESP32 (Publisher/Subscriber). Si connette alla rete Wi-Fi SoftAP generata dal primo ESP32 (Broker) e si occupa del mantenimento automatico della connessione con tentativi periodici di ripristino in caso di caduta del segnale.

## Responsabilità
* Inizializzazione della modalità stazione Wi-Fi.
* Negoziazione della connessione con l'Access Point del Broker (`SSID` e password).
* Monitoraggio continuo del link radio (`maintainConnection`).
* Riconnessione non bloccante temporizzata in caso di disconnessione.

## Metodi Principali
* `bool connect()`: Avvia la procedura di connessione all'AP con timeout.
* `void disconnect()`: Interrompe la connessione wireless.
* `bool isConnected() const`: Restituisce lo stato attuale della connessione Wi-Fi.
* `void maintainConnection()`: Metodo non bloccante da richiamare nel loop per il recupero automatico del link.
* `IPAddress getLocalIp() const`: Ritorna l'IP assegnato dal DHCP del Broker.
