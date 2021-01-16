/***************************

Membaca E-KTP (KTP Elektronik) dengan RFID Reader RC522 Berbasis Arduino Uno

Oleh : Arduino Indonesia
Website : www.arduinoindonesia.id
Toko Online : www.workshopelectronics3in1.com
Blog : www.edukasielektronika.com

Copyright @2020

****************************/


#include <SPI.h>
#include <RFID.h>
#define SDA_PIN D4 //10
#define RST_PIN D3 //9
RFID rfid(SDA_PIN,RST_PIN);

int serNum[5];          //Variable buffer Scan Card

void setup()
{
  Serial.begin(9600);                      
  SPI.begin();
  rfid.init();
  delay(1000);
  Serial.println("Sistem Pembacaan E-KTP Ready...");
  delay(2000);
  Serial.println("Tempelkan E-KTP Anda");
  Serial.println("");
}

void loop()
{
  if(rfid.isCard())
  {
    if(rfid.readCardSerial())
    {
      Serial.print("Kode Tag E-KTP");
      Serial.print(" : ");
      Serial.print(rfid.serNum[0], HEX);
      Serial.print(" ");
      Serial.print(rfid.serNum[1], HEX);
      Serial.print(" ");
      Serial.print(rfid.serNum[2], HEX);
      Serial.print(" ");
      Serial.print(rfid.serNum[3], HEX);
      Serial.print(" ");
      Serial.print(rfid.serNum[4], HEX);
      Serial.println("");
    }
  }
  rfid.halt();
  delay(1000);
}
