# Classe TemperatureSensor

## Descrizione
La classe `TemperatureSensor` fornisce l'interfaccia per il rilevamento termico interno del processore ESP32. Sfrutta il sensore a semiconduttore on-die integrato nel package del SoC senza richiedere termistori o sensori esterni (come DHT11/DS18B20).

## Responsabilità
* Interrogazione del registro del sensore di temperatura on-chip tramite `temperatureRead()`.
* Calcolo e conversione in gradi Celsius (°C) e Fahrenheit (°F).
* Memorizzazione dell'ultima lettura valida.

## Metodi Principali
* `float readCelsius()`: Restituisce la temperatura istantanea del die in gradi Celsius.
* `float readFahrenheit()`: Restituisce la temperatura convertita in scala Fahrenheit.
