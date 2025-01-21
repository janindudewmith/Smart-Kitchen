#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "MQ135.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>

// Wi-Fi credentials for ThingsBoard connection
const char* wifi_ssid = "Deshan";
const char* wifi_password = "deshanls";

// Wi-Fi credentials for ESP-Dash Access Point
const char* ap_ssid = "Smart_Kitchen";
const char* ap_password = "00000000";

// ThingsBoard server and token
const char* thingsboardServer = "demo.thingsboard.io";
const char* accessToken = "mmtuzczvwohl6wphwmgu";

// Initialize ThingsBoard MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// ESP-Dash Configuration
AsyncWebServer server(80);
ESPDash dashboard(&server);

// Dashboard cards
Card temp(&dashboard, TEMPERATURE_CARD, "Temperature", "°C");
Card hum(&dashboard, HUMIDITY_CARD, "Humidity", "%");
Card gas(&dashboard, GENERIC_CARD, "Gas PPM", "PPM");
Card buzz(&dashboard, STATUS_CARD, "Alarm", "");
Card relay(&dashboard, STATUS_CARD, "Relay Status", "");

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ135PIN 34
#define BUZZER_PIN 25
#define RELAY_PIN 27

#define TEMP_THRESHOLD 32
#define HUM_THRESHOLD 50
#define GAS_THRESHOLD 750

// Function to update the buzzer card status on the dashboard
void updateBuzzerStatus(bool isActive) {
  if (isActive) {
    buzz.update("Active", "danger");
  } else {
    buzz.update("Inactive", "normal");
  }
}

// Function to update the relay card status on the dashboard
void updateRelayStatus(bool isActive) {
  if (isActive) {
    relay.update("Triggered", "danger");
  } else {
    relay.update("Normal", "normal");
  }
}

// ThingsBoard reconnection function
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32", accessToken, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi Station mode for ThingsBoard
  WiFi.mode(WIFI_AP_STA); // Set WiFi to AP+STA mode
  
  // Connect to WiFi network for ThingsBoard
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Connecting to WiFi network: ");
  Serial.println(wifi_ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi network");
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up Access Point for ESP-Dash
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point Started");
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize MQTT client for ThingsBoard
  client.setServer(thingsboardServer, 1883);

  // Initialize ESP-Dash server
  server.begin();

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_BLACK, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(MQ135PIN, INPUT);
}

void loop() {
  // ThingsBoard connection check
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int gasValue = analogRead(MQ135PIN);
  float gasPPM = map(gasValue, 0, 4095, 0, 1000);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("DHT sensor error"));
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("DHT Sensor Error"));
    display.display();
    delay(2000);
    return;
  }

  // Check thresholds and control buzzer/relay
  bool buzzerActive = (temperature > TEMP_THRESHOLD || humidity < HUM_THRESHOLD || gasPPM > GAS_THRESHOLD);

  // Send data to ThingsBoard
  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity) + ",";
  payload += "\"gasPPM\":" + String(gasPPM) + ","; 
  payload += "\"buzzerStatus\":" + String(buzzerActive) + ","; 
  payload += "\"relayStatus\":" + String(buzzerActive);
  payload += "}";

  // Send data to ThingsBoard
  /*String payload = "{";
  //payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity) + ",";
  payload += "\"gasPPM\":" + String(gasPPM);
  payload += "}";*/
  client.publish("v1/devices/me/telemetry", payload.c_str());

  // Update ESP-Dash cards for sensor values
  temp.update(temperature);
  hum.update(humidity);
  gas.update((int)gasPPM);
  
  // Update device states and ESP-Dash status
  digitalWrite(BUZZER_PIN, buzzerActive ? HIGH : LOW);
  digitalWrite(RELAY_PIN, buzzerActive ? HIGH : LOW);
  
  // Update ESP-Dash status cards
  updateBuzzerStatus(buzzerActive);
  updateRelayStatus(buzzerActive);
  
  // Send the updates to the dashboard
  dashboard.sendUpdates();

  if (buzzerActive) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("*-DANGER-*"));
    display.display();
    delay(2000);
  } else {
    // Display temperature on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("DHT11 Sensor Data"));
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.println(F("Temp:"));
    display.setCursor(0, 40);
    display.print(temperature);
    display.println(F(" C"));
    display.display();
    delay(2000);

    // Display humidity on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("DHT11 Sensor Data"));
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.println(F("Hum:"));
    display.setCursor(0, 40);
    display.print(humidity);
    display.println(F(" %"));
    display.display();
    delay(2000);

    // Display MQ-135 gas data on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("MQ-135 Sensor Data"));
    display.setCursor(0, 20);
    display.setTextSize(1.5);
    display.println(F("Gas:"));
    display.setCursor(0, 40);
    display.print(gasPPM);
    display.println(F(" PPM"));
    display.display();
    delay(2000);
  }
}
