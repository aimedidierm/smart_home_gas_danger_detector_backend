#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#define MQ3pin (A0)
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid = "gas_detector";
const char* password = "gas_detector123";

int buzzerPin = D0;
int valve = D5;
bool gasDetected = false;
float sensorValue;
int sensorThreshold = 400, pstatus = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(valve, OUTPUT);
  digitalWrite(valve,HIGH);
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Gas Danger");
  lcd.setCursor(0, 1);
  lcd.print("Detector System");
  delay(3000);
  lcd.clear();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WiFi network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  lcd.clear(),
  lcd.print("System");
  lcd.setCursor(0, 1);
  lcd.print("Starting");
  delay(3000);

}

void loop() {
  updateStatus();
  float sensorValue = analogRead(MQ3pin);  // Read analog value from MQ3 sensor
  Serial.println("Sensor:");
  Serial.println(sensorValue);
  if(sensorValue>70)
  {
    gasDetected = true;
    lcd.clear();
    lcd.println("Gas detected");
    playBeep(1000, 500);
  } else {
    gasDetected = false;
    lcd.clear();
    lcd.println("Normal");
  }
  delay(1000);
  updateHardwareStatus();
}

void updateStatus() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://umutoni.itaratec.com/api/status")) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
          DynamicJsonDocument doc(512); // Adjust the size based on your payload size
        DeserializationError error = deserializeJson(doc, payload);

        // Check for parsing errors
        if (error) {
            Serial.print(F("Error parsing JSON: "));
            Serial.println(error.c_str());
        } else {
            // Access JSON data
            bool valveOpen = doc["valve_open"];

            Serial.print(F("Valve Open: "));
            Serial.println(valveOpen);
            if(valveOpen)
            {
              digitalWrite(valve, LOW);
              lcd.clear();
              lcd.println("Valve open");
            } else{
              digitalWrite(valve, HIGH);
              lcd.clear();
              lcd.println("Valve closed");
            }
        }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      lcd.clear();
      lcd.println("Unable to connect\n");
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}

void updateHardwareStatus() {
  if (WiFi.status() == WL_CONNECTED) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation (for testing only)
    client->setInsecure();
    HTTPClient http;

    Serial.println("[HTTPS] begin...");

    if (http.begin(*client, "https://umutoni.itaratec.com/api/status")) {  // HTTPS
      Serial.println("[HTTPS] POST...");

      http.addHeader("Content-Type", "application/json");

      DynamicJsonDocument jsonDoc(128); // Adjust the buffer size as needed

      jsonDoc["gas_detected"] = gasDetected;

      String jsonPayload;
      serializeJson(jsonDoc, jsonPayload);

      // Send the POST request with the JSON payload
      int httpCode = http.POST(jsonPayload);

      // Check the HTTP response code
      if (httpCode > 0) {
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // Read and print the response from the server
        String payload = http.getString();
        Serial.println(payload);
        DynamicJsonDocument doc(512); // Adjust the size based on your payload size
        DeserializationError error = deserializeJson(doc, payload);

        // Check for parsing errors
        if (error) {
          Serial.print(F("Error parsing JSON: "));
          Serial.println(error.c_str());
        } else {
          // Access JSON data
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        lcd.clear(),
        lcd.print("Sending failed");
        delay(3000);
      }

      // End the HTTP connection
      http.end();
    } else {
      Serial.println("[HTTPS] Unable to connect");
      lcd.clear(),
      lcd.print("Unable to connect");
      delay(3000);
    }
  }
}

void playBeep(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
  delay(duration);
  noTone(buzzerPin);
}
