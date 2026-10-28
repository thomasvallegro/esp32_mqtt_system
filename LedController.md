# Classe LedController

## Descrizione
La classe `LedController` incapsula la gestione dell'attuatore luminoso (LED onboard, collegato al pin `GPIO 2` della maggior parte delle schede ESP32). Isola la logica di I/O digitale rispetto al resto dell'applicazione.

## Responsabilità
* Inizializzazione della direzione del pin (`OUTPUT`) tramite `pinMode`.
* Pilotaggio dei livelli logici `HIGH` / `LOW` per l'accensione e lo spegnimento.
* Inversione dello stato (`toggle`).
* Esposizione dello stato logico corrente (`isOn`).

## Metodi Principali
* `void begin()`: Configura il pin hardware e forza lo stato iniziale su spento.
* `void turnOn()`: Accende il LED.
* `void turnOff()`: Spegne il LED.
* `void toggle()`: Inverte lo stato del LED.
* `void setState(bool is_on)`: Imposta esplicitamente lo stato desiderato (`true` = acceso, `false` = spento).
* `bool isOn() const`: Ritorna `true` se il LED è attualmente acceso.
