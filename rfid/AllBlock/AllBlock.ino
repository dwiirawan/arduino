// READ ALL BLOCK

#include <SPI.h>      //include the SPI bus library
#include <MFRC522.h>  //include the RFID reader library

#define SS_PIN 10  //slave select pin 10
#define RST_PIN 9  //reset pin 5

MFRC522 mfrc522(SS_PIN, RST_PIN);  // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information

//This array is used for reading out a block.
byte readbackblock[18];

void setup() 
{
    Serial.begin(9600);        // Initialize serial communications with the PC
    SPI.begin();               // Init SPI bus
    mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
    Serial.println("Scan a MIFARE Classic card");
  
  // Prepare the security key for the read and write functions.
//  for (byte i = 0; i < 6; i++) {
//    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
//  }

  key.keyByte[0] = 0xFF;
  key.keyByte[1] = 0xFF;
  key.keyByte[2] = 0xFF;
  key.keyByte[3] = 0xFF;
  key.keyByte[4] = 0xFF;
  key.keyByte[5] = 0xFF;
}

void loop()
{  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.println("card selected");
         
   //read all block (0 - 63)
   for (int a=0 ; a<64 ; a++)
   {

    // Cek tariler block
   if (a > 2 && (a+1)%4 == 0){
      Serial.print(a);
      Serial.println(" is a trailer block");
   } else {
      readBlock(a, readbackblock);
      
     //print the block contents
     Serial.print("read block ");
     Serial.print(a);
     Serial.print(": ");
     for (int j=0 ; j<16 ; j++)
     {
       Serial.write (readbackblock[j]);
     }
     Serial.println("");
   } // end if
  
   } // end for a
   delay(5000);
}

//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }

  //reading a block
  byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(mfrc522.GetStatusCodeName(status));
          return 4;//return "4" as error message
  }
}
