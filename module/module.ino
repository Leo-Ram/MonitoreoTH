#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>              //sensor de temperatura
#include "credentials.h"


#define DHT_PIN 14 
DHT dht(DHT_PIN, DHT11);

float temperature;
float humidity;

// Configuración de la zona horaria de Colombia (UTC-5)
const long utcOffsetInSeconds = -5 * 3600; // -5 horas en segundos
const int daylightOffsetInSeconds = 0; // No hay horario de verano en Colombia

void setup() {
    Serial.begin(115200);
    
    // Conectar a WiFi
    WiFi.begin(ssid, password);
    Serial.println("Conectando a WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("Conectado a WiFi!");
    
}

void sendToSupabase(float temp, float hum) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // Construir URL completa
        String url = String(supabaseUrl) + "/rest/v1/" + tableName;
        
        // Crear JSON con los datos
        StaticJsonDocument<200> doc;
        doc["sensor_id"] = "ESP32_001";  // Identificador único para tu ESP32
        doc["temperature"] = temp;
        doc["humidity"] = hum;
        
        String jsonString;
        serializeJson(doc, jsonString);
        
        // Configurar headers y realizar petición POST
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("apikey", supabaseKey);
        http.addHeader("Authorization", "Bearer " + String(supabaseKey));
        
        int httpResponseCode = http.POST(jsonString);
        
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Código HTTP: " + String(httpResponseCode));
            Serial.println("Respuesta: " + response);
        } else {
            Serial.println("Error en petición HTTP: " + String(httpResponseCode));
        }
        
        http.end();
    }
}

void loop() {
    // Simular lecturas de sensores (reemplaza esto con tus lecturas reales)
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();   // Ejemplo

    // Controlar si hubo algún error durante la lectura de sensores
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Error al leer los sensores!");
        delay(1000);
        return;
    }

    // Enviar datos a Supabase
    sendToSupabase(temperature, humidity);
    
    esp_sleep_enable_timer_wakeup(600000000); // Microsegundos (10 minutos)
    Serial.println("ESP32 entrando en deep sleep por 10 minutos...");
    esp_deep_sleep_start();
}