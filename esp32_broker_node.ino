#include <Arduino.h>
#include "BrokerNetworkManager.h"
#include "MqttBroker.h"

// Parametri di configurazione globale rete e broker MQTT
const char* WIFI_AP_SSID = "ESP32_MQTT_BROKER_NET";
const char* WIFI_AP_PASS = "12345678";
const IPAddress BROKER_STATIC_IP(192, 168, 4, 1);
const IPAddress BROKER_GATEWAY(192, 168, 4, 1);
const IPAddress BROKER_SUBNET(255, 255, 255, 0);
const uint16_t BROKER_TCP_PORT = 1883;

// Istanze dei moduli di gestione
BrokerNetworkManager network_mgr(WIFI_AP_SSID, WIFI_AP_PASS, BROKER_STATIC_IP, BROKER_GATEWAY, BROKER_SUBNET);
MqttBroker mqtt_broker(BROKER_TCP_PORT);

unsigned long last_status_report_ms = 0;
const unsigned long STATUS_INTERVAL_MS = 5000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n==================================================");
  Serial.println("   ESP32 NODO 1: BROKER MQTT PRIVATO AD IP FISSO  ");
  Serial.println("==================================================");

  // 1. Inizializzazione rete wireless privata SoftAP
  if (!network_mgr.startAccessPoint()) {
    Serial.println("[MAIN] Errore critico: impossibile avviare SoftAP. Riavvio in corso...");
    delay(3000);
    ESP.restart();
  }

  // 2. Inizializzazione del broker MQTT sulla porta 1883
  if (!mqtt_broker.start()) {
    Serial.println("[MAIN] Errore critico: impossibile avviare il Broker MQTT.");
    return;
  }

  Serial.println("[MAIN] Sistema Broker operativo e pronto per i client.");
  Serial.println("==================================================");
}

void loop() {
  // Gestione asincrona pacchetti e client del broker
  mqtt_broker.process();

  // Telemetria diagnostica periodica dello stato del broker
  unsigned long current_time_ms = millis();
  if (current_time_ms - last_status_report_ms >= STATUS_INTERVAL_MS) {
    last_status_report_ms = current_time_ms;
    Serial.printf("[BROKER STATS] Stazioni Wi-Fi connesse: %d | Client MQTT attivi: %u | Free Heap: %d bytes\n",
                  network_mgr.getConnectedStationsCount(),
                  mqtt_broker.getConnectedClientsCount(),
                  ESP.getFreeHeap());
  }
}
