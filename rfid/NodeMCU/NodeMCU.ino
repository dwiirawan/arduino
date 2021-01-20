// Wifi NodeMCU
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// RFID Reader
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D4  //D2
#define RST_PIN D3 //D1

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  Serial.println(F("Selamat datang di pembacaan RFID."));
  //Serial.print(F("Using the following key:"));
  //printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  
  // Connect Wifi
  WiFi.begin("Redmi 5", "internet");
  
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
 
void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC tipe: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  } // end if

  // Get NUID
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {  
    Serial.println(F("Kartu baru ditemukan"));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    //Show UID on serial monitor
    Serial.print(F("NUID kartu adalah: "));
    String content= "";
    byte letter;
    for (byte i = 0; i < rfid.uid.size; i++) 
    {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
      content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(rfid.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    content.replace(" ", "-");
    Serial.println();
    String card_id = content.substring(1);
    //Serial.println(card_id);
    
    // Wifi Status
    if( WiFi.status() == WL_CONNECTED )
    {
      // Save to Database MySQL
      String url = "http://munculmotor.com/";
      url += "iot/rfid/rfid-post.php?nuid=";
      url += card_id;
    
      HTTPClient http;
      Serial.print("[HTTP] begin...\n");  
      http.begin( url ); 
      Serial.print("[HTTP] GET...\n");
        
      int httpCode = http.GET();
      if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", 
        httpCode);
    
        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", 
        http.errorToString(httpCode).c_str());
      } // end if hhtpCode
      
    http.end();
    } // End if Wifi Status
 
  } else {
    Serial.println("Kartu sudah discan");
  } // End if Get NUID
  
  Serial.println("---------------------------------");
  Serial.println();
    
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

 delay(2000);
} // End loop
