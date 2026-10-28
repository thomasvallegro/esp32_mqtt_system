#include <Arduino.h>
#include "NetworkClient.h"
#include "TemperatureSensor.h"
#include "LedController.h"
#include "MqttClient.h"

// Parametri di configurazione globale rete e broker MQTT
const char* WIFI_SSID = "ESP32_MQTT_BROKER_NET";
const char* WIFI_PASS = "12345678";
const IPAddress BROKER_IP(192, 168, 4, 1);
const uint16_t BROKER_PORT = 1883;
const char* MQTT_CLIENT_ID = "esp32_telemetry_client";

// Topic MQTT hard-coded
const char* TOPIC_CPU_TEMPERATURE = "esp32/cpu/temperature";
const char* TOPIC_LED_COMMAND     = "esp32/led/command";

// Istanze dei moduli
NetworkClient network_client(WIFI_SSID, WIFI_PASS);
TemperatureSensor temp_sensor;
LedController led_ctrl(2); // LED onboard su GPIO 2
MqttClient mqtt_client(BROKER_IP, BROKER_PORT, MQTT_CLIENT_ID);

unsigned long last_temp_publish_ms = 0;
const unsigned long TEMP_PUBLISH_INTERVAL_MS = 5000;

// Callback richiamata alla ricezione di messaggi sui topic sottoscritti
void handleMqttMessage(const String& topic, const String& payload) {
  Serial.printf("[MAIN] Ricevuto messaggio sul topic '%s': '%s'\n", topic.c_str(), payload.c_str());

  if (topic == TOPIC_LED_COMMAND) {
    String command = payload;
    command.trim();
    command.toUpperCase();

    if (command == "ON" || command == "1" || command == "TRUE") {
      led_ctrl.turnOn();
      Serial.println("[MAIN] Comando ricevuto: LED ACCESO.");
    } else if (command == "OFF" || command == "0" || command == "FALSE") {
      led_ctrl.turnOff();
      Serial.println("[MAIN] Comando ricevuto: LED SPENTO.");
    } else if (command == "TOGGLE") {
      led_ctrl.toggle();
      Serial.printf("[MAIN] Comando ricevuto: LED INVERTITO (Stato: %s).\n", 
                    led_ctrl.isOn() ? "ACCESO" : "SPENTO");
    } else {
      Serial.printf("[MAIN] Comando LED sconosciuto: '%s'. Ignorato.\n", payload.c_str());
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n==================================================");
  Serial.println("   ESP32 NODO 2: TELEMETRIA CPU & CONTROLLO LED   ");
  Serial.println("==================================================");

  // Inizializzazione hardware periferica LED
  led_ctrl.begin();

  // Registrazione callback per i messaggi in ingresso
  mqtt_client.setMessageCallback(handleMqttMessage);

  // Connessione iniziale alla rete Wi-Fi SoftAP dell'ESP32 Broker
  network_client.connect();
}

void loop() {
  // 1. Manutenzione e riconnessione Wi-Fi in caso di disconnessione
  network_client.maintainConnection();

  if (network_client.isConnected()) {
    // 2. Connessione o riconnessione al Broker MQTT
    if (!mqtt_client.isConnected()) {
      Serial.println("[MAIN] Connessione al broker in corso...");
      if (mqtt_client.connect()) {
        // Sottoscrizione al topic di comando del LED
        mqtt_client.subscribe(TOPIC_LED_COMMAND);
        Serial.printf("[MAIN] Sottoscritto con successo al topic: %s\n", TOPIC_LED_COMMAND);
      } else {
        delay(2000);
        return;
      }
    }

    // 3. Elaborazione pacchetti MQTT in ingresso ed emissione keep-alive
    mqtt_client.loop();

    // 4. Campionamento periodico e pubblicazione temperatura processore
    unsigned long current_time_ms = millis();
    if (current_time_ms - last_temp_publish_ms >= TEMP_PUBLISH_INTERVAL_MS) {
      last_temp_publish_ms = current_time_ms;

      float temp_c = temp_sensor.readCelsius();
      String payload = String(temp_c, 2);

      Serial.printf("[MAIN] Rilevata Temperatura CPU: %.2f °C -> Invio su '%s'...\n", 
                    temp_c, TOPIC_CPU_TEMPERATURE);

      bool publish_ok = mqtt_client.publish(TOPIC_CPU_TEMPERATURE, payload);
      if (publish_ok) {
        Serial.println("[MAIN] Temperatura pubblicata con successo!");
      } else {
        Serial.println("[MAIN] Errore durante la pubblicazione del messaggio.");
      }
    }
  }
}
