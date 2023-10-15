#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define MQ2pin (A0)
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid = "Balance";
const char* password = "balance123";

int buzzer = D0;
int valve = D5;

float sensorValue;
int sensorThreshold = 400, pstatus = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(valve, OUTPUT);
  //  digitalWrite(valve, HIGH);
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
            bool gasDetected = doc["gas_detected"];

            Serial.print(F("Valve Open: "));
            Serial.println(valveOpen);
            Serial.print(F("Gas Detected: "));
            Serial.println(gasDetected);
        }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println();
  Serial.println("Waiting 2min before the next round...");
}
