//https://blog.jmaker.com.tw/arduino-rfid/
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          
#define SS_PIN          10  //就是模組上的SDA接腳


enum RFID_CARD_UID
{
  ALPHA1 = 0x02,
//  ALPHA2 = 0x??,  
  BRAVO1 = 0x53,
  BRAVO2 = 0xDB,
  MOUNT1 = 0x63,
  MOUNT2 = 0x22,

} card_uid;


MFRC522 mfrc522;   // 建立MFRC522實體

void setup() {

  Serial.begin(115200); 

  SPI.begin();        // 初始化SPI介面

  mfrc522.PCD_Init(SS_PIN, RST_PIN); // 初始化MFRC522卡
  Serial.print(F("Reader "));
  Serial.print(F(": "));
  mfrc522.PCD_DumpVersionToSerial(); // 顯示讀卡設備的版本
  
}
void PrintHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
     char tmp[16];
       for (int i=0; i<length; i++) { 
         sprintf(tmp, "0x%.2X",data[i]); 
         Serial.print(tmp); Serial.print(" ");
       }
}
uint8_t buf[10]= {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

bool cmpid(MFRC522::Uid *id1, MFRC522::Uid *id2){
  return memcmp(id1, id2, sizeof(MFRC522::Uid));
}

void deregister_card(){
  is_card_present = false;
  memset(&id,0, sizeof(id));
}
uint8_t control = 0x00;



void loop() {
    bool cardRemoved = false;
    int counter = 0;
    bool current, previous;
    int sensing_time = 0;
    previous = !mfrc522.PICC_IsNewCardPresent();
    
    while(!cardRemoved){
      current =!mfrc522.PICC_IsNewCardPresent();

      if (current && previous) counter++;

      previous = current;
      cardRemoved = (counter>2); 
           

      if(counter==2 && mfrc522.PICC_ReadCardSerial())
      {
        delay(50);        
        sensing_time += 50;
        
        Serial.print(F("Card UID:"));
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size); // 顯示卡片的UID
        Serial.println();      
          
        Serial.println("Waiting for card to disappear");
        Serial.print(counter);
        Serial.print(", ");
        Serial.println(sensing_time);        
      }
        

    }  

    mfrc522_fast_Reset();
}

void mfrc522_fast_Reset()
{
  digitalWrite(RST_PIN, HIGH);
  mfrc522.PCD_Reset();
  mfrc522.PCD_WriteRegister(mfrc522.TModeReg, 0x80);      // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  mfrc522.PCD_WriteRegister(mfrc522.TPrescalerReg, 0x43);   // 10µs.
  
  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegH, 0x00);   // Reload timer with 0x01E = 30, ie 0.3ms before timeout.
  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x1E);
  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x5);
  
  mfrc522.PCD_WriteRegister(mfrc522.TxASKReg, 0x40);    // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  mfrc522.PCD_WriteRegister(mfrc522.ModeReg, 0x3D);   // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
  
  mfrc522.PCD_AntennaOn();            // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

void loop0() {

  // 檢查是不是一張新的卡
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // 顯示卡片內容
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size); // 顯示卡片的UID
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));  //顯示卡片的類型

      mfrc522.PICC_HaltA();  // 卡片進入停止模式
    }
}

/**
 * 這個副程式把讀取到的UID，用16進位顯示出來
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
