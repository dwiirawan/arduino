#include <SPI.h>
#include <MFRC522.h>
#define pinSDA 10
#define pinRST 9
MFRC522 RFID(pinSDA, pinRST);
void setup() {
  Serial.begin(9600);
  SPI.begin();
  RFID.PCD_Init();
  Serial.println("RFID Reader");
  Serial.println("");
  Serial.println("Tap Kartu/Gantungan !");
  Serial.println();
}
void loop() {
  if ( ! RFID.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! RFID.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("ID Tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < RFID.uid.size; i++) {
    
    Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(RFID.uid.uidByte[i], HEX);
    content.concat(String(RFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(RFID.uid.uidByte[i], HEX));
  }
  Serial.println();
  
  content = String(RFID.uid.uidByte[0], HEX) + String(RFID.uid.uidByte[1], HEX) + String(RFID.uid.uidByte[2], HEX) + String(RFID.uid.uidByte[3], HEX);
  Serial.println(content);
  delay(1500);
}
