/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>


#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;

ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(9600); //Création de la liaison série
  Serial.println("ready"); //Premier envoi

  Serial.println();
  Serial.println();
  Serial.println();

  
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // use this instead: 
  dht.setup(16, DHTesp::DHT22); // Connect DHT sensor to GPIO 16 = D0
  dht2.setup(5, DHTesp::DHT22); // Connect DHT sensor to GPIO 5 = D1

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  //connexion au réseau wifi 
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Baudouin", "baudouin"); 

}

void loop() {
  delay(dht.getMinimumSamplingPeriod());
  delay(dht2.getMinimumSamplingPeriod());

//récupération des données des capteurs 
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float luminosite = dht2.getLuminosite();


//affichage sur le moniteur
  Serial.println(dht.getStatusString());
  Serial.print("Humidity: ");
  Serial.println(humidity, 1);
  Serial.print("Temperature: ");
  Serial.println(temperature, 1);
  Serial.println(dht2.getStatusString());
  Serial.print("Luminosite: ");
  Serial.println(luminosite, 1);
  Serial.print(" ");
  delay(2000);

  //conversion en string des valeurs des capteurs pour les passer dans l'Url 
  String tempString;
  tempString = String(temperature);
  String humString;
  humString = String(humidity);
  String lumString;
  lumString = String(luminosite);
  String url1 = tempString + "&humidite=" + humString + "&luminosite=" + lumString;

  
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://172.20.10.2:8888/ING4_TP/IoT/wifi/data.php?temperature="+ url1)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(10000);
}&
