# Classe ClientSession

## Descrizione
La classe `ClientSession` incapsula la sessione di comunicazione TCP tra il broker MQTT e un singolo client connesso. Mantiene il riferimento al socket di rete, l'identificativo testuale del client MQTT e l'elenco dei topic a cui il client ha effettuato la sottoscrizione (subscribe).

## Responsabilità
* Gestione dello stato di connessione e ciclo di vita del socket client (`WiFiClient`).
* Memorizzazione dell'ID client inviato durante l'handshake `CONNECT`.
* Gestione dell'elenco delle sottoscrizioni a topic specifici o wildcard (`#`).
* Verifica della corrispondenza tra topic pubblicati e sottoscrizioni attive per l'instradamento selettivo dei messaggi.
* Trasmissione a basso livello di pacchetti binari sul canale TCP.

## Metodi Principali
* `bool isConnected()`: Controlla se il socket TCP sottostante è attivo.
* `void disconnect()`: Termina la connessione e pulisce l'elenco delle sottoscrizioni.
* `void addSubscription(const String& topic)`: Aggiunge un topic all'elenco dei canali monitorati.
* `bool isSubscribedTo(const String& topic) const`: Verifica se il client deve ricevere messaggi inviati sul dato topic.
* `bool sendRaw(const uint8_t* buffer, size_t length)`: Invia byte grezzi sul socket di rete.
