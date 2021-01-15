#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

#define USE_SERIAL Serial

// Pin data yg terhubug DHT dan NodeMCU
// dan Type DHT yg digunakan
DHT dht(D1, DHT11);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  USE_SERIAL.begin(115200);

  // isi dengan SSID dan Password
  // wifi yg digunakan
  WiFi.begin("Accounting", "munculgroup");

  USE_SERIAL.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println("");
  USE_SERIAL.print("ip: ");
  USE_SERIAL.println(WiFi.localIP());

  dht.begin();
}

void loop() {
 if( WiFi.status() == WL_CONNECTED ){
  digitalWrite(LED_BUILTIN, LOW);

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celcius
  float f = dht.readTemperature(true); // Fahrenheit

  if (isnan(h) || isnan(t) || isnan(f)) {
    USE_SERIAL.println("Kosong");
  }else{
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

    // URL target untuk menyimpan data sensor DHT
    // ke database mysql
    String url = "http://munculmotor.com/";
    url += "iot/basic/index.php?suhu=";
    url += String(t) +  "&lembab=";
    url += String(h);

    
    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    
    http.begin( url ); 
    USE_SERIAL.print("[HTTP] GET...\n");
    
    int httpCode = http.GET();
    if(httpCode > 0) {
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", 
        httpCode);

        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            USE_SERIAL.println(payload);
        }
    } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", 
        http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  
  digitalWrite(LED_BUILTIN, HIGH);
 }

 // delay selama 1 menit
 // data akan di kirim 1 menit sekali
 delay(60000);
}
