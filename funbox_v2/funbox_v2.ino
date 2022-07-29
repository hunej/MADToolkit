// https://github.com/miguelbalboa/rfid/issues/352#issue-282870788

// Header
#include <SPI.h>
#include <MFRC522.h>
#include <TimerOne.h>

// Define
#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above
#define BUZZER_PIN 3

// Global variables
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

bool rfid_tag_present_prev = false;
bool rfid_tag_present = false;
int _rfid_error_counter = 0;
bool _tag_found = false;

bool stage_ahead = false;
bool END = false;
int second = 20;
int minute = 0;
int halfsecond = 2;
int beeptoggle = 0;
int countdown = 0; // second
int sensing_time = 0;
int current_stage = 0;

//Enum
enum PLAY_MODE_STAGE
{
  EXP_MODE_STAGE0 = 0,
  EXP_MODE_STAGE1 = 1,
  EXP_MODE_STAGE2 = 2,
  DOM_MODE_STAGE0 = 3,

} playstage;

enum PLAYBACK_MODE
{
  START = 8,//I set 0001_START.mp3 and fucking dont know why got 8 here
  ALPHA_TEAM_LOSE_EXPLOSION = 7,
  ALPHA_TEAM_WIN_EXPLOSION = 6,
  BRAVO_TEAM_WIN = 5,
  MOUNTING_BOMB_SIREN = 4,
  BOMB_MOUNTED_SIREN = 3,
  GROUNDED_SIREN = 1,

} playback;

enum RFID_CARD_UID
{
  ALPHA1 = 0x02,
//  ALPHA2 = 0x??,  
  BRAVO1 = 0x53,
  BRAVO2 = 0xDB,
  MOUNT1 = 0x63,
  MOUNT2 = 0x22,

} card_uid;


void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ;          // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin(); // Init SPI bus

  // RFID
  mfrc522.PCD_Init(); // Init MFRC522

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT); // BUZZER
  beep_short(3);

  // ISR
  Timer1.initialize(500000); // 0.5s
  Timer1.attachInterrupt(TimingISR);

  // Countdown
  countdown = 20;
}


//current_stage
void loop()
{

  stage_ahead = rfid_sensing(current_stage);
  

  
  if(stage_ahead && current_stage == EXP_MODE_STAGE0)
  {
    current_stage++; 
    countdown = 10;
  }
  else if(stage_ahead)
  {
    current_stage++; 
  }
    
    
//  Serial.print("current_stage: ");
//  Serial.println(current_stage);   


/*
  EXP_MODE_STAGE0 = 0,
  EXP_MODE_STAGE1 = 1,
  EXP_MODE_STAGE2 = 2,
  DOM_MODE_STAGE0 = 3,
*/
  switch (current_stage)
  {
    case EXP_MODE_STAGE0:
      //
      if(countdown<=0)
      {
        Serial.println("ALPHA LOSE");
      }
      break;
    case EXP_MODE_STAGE1:
      // playback BOMB_MOUNTED_SIREN if mp3 idle
      if(countdown<=0)
      {
        Serial.println("ALPHA WIN");
      }
      break;
    case EXP_MODE_STAGE2:
      //
      Serial.println("BRAVO WIN");
      END = 1;
      break;       
    default:
      Serial.println("Invalid stage number");
  }   



  while(END);
}

bool rfid_sensing(int stage)
{
  int tlast;

  rfid_tag_present_prev = rfid_tag_present;

  _rfid_error_counter += 1;
  if (_rfid_error_counter > 2)
  {
    _tag_found = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  // Reset baud rates
  mfrc522.PCD_WriteRegister(mfrc522.TxModeReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.RxModeReg, 0x00);
  // Reset ModWidthReg
  mfrc522.PCD_WriteRegister(mfrc522.ModWidthReg, 0x26);

  MFRC522::StatusCode result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);

  if (result == mfrc522.STATUS_OK)
  {
    if (!mfrc522.PICC_ReadCardSerial())
    { // Since a PICC placed get Serial and continue
      return;
    }
    _rfid_error_counter = 0;
    _tag_found = true;
  }

  rfid_tag_present = _tag_found;

  if(!cmp_stage_match(stage, mfrc522.uid.uidByte[3]))
    return false;
    
  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev)
  {
    sensing_time = 0;
    Serial.print("_tag_found: ");
    Serial.print(_tag_found);
    Serial.print(", ");

    Serial.print("Tag 0x");
    Serial.print(mfrc522.uid.uidByte[3], HEX);
    Serial.println(" found");

    tlast = millis();
  }

  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev)
  {
    sensing_time = 0;
    Serial.print("_tag_found: ");
    Serial.print(_tag_found);
    Serial.print(", ");

    tlast = millis() - tlast;
    Serial.print("Tag gone: ");
    Serial.println(tlast);
  }

//  if(sensing_time>0){
//    Serial.print("sensing_time: ");
//    Serial.println(sensing_time);  
//  }

  if(sensing_time>3000)
  {
    Serial.print("sensing_time: ");
    Serial.println(sensing_time);      
    sensing_time = 0;
    return true;
  }
  else
    return false;
}

void TimingISR()
{ // 0.5s

  if (END)
  {
    return;
  }

  halfsecond--;

  
  if (_tag_found)
  {
    sensing_time += 500;
  }


  Serial.print("countdown: ");
  Serial.println(countdown);
  

  beeptoggle = (~beeptoggle);
  if (beeptoggle)
  {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }

  if (halfsecond == 0)
  {
    countdown--;

    if (countdown <= 0)
    {
      END = true;
    }

    second--;

    if (second == 0)
    {
      minute--;
      second = 60;
    }

    halfsecond = 2;
  }
}





/*
  ALPHA1 = 0x02, 
  BRAVO1 = 0x53,
  BRAVO2 = 0xDB,
  MOUNT1 = 0x63,
  MOUNT2 = 0x22,

  EXP_MODE_STAGE0 = 0,
  EXP_MODE_STAGE1 = 1,
  EXP_MODE_STAGE2 = 2,
  DOM_MODE_STAGE0 = 3,
*/
bool cmp_stage_match(int stage, byte uid)
{
  int uid32 = (int)uid;
//  Serial.print("got uid!!!!!!!!!!: 0x");
//  Serial.println(uid32, HEX);
//  Serial.print("stage: ");
//  Serial.println(stage);

  if(stage == EXP_MODE_STAGE0 && (uid==MOUNT1 || uid==MOUNT2))
  {
//    Serial.println("EXP_MODE_STAGE0 pass");
    return true;
  }
  else if(stage == EXP_MODE_STAGE1 && (uid==BRAVO1 || uid==BRAVO2))
  {
//    Serial.println("EXP_MODE_STAGE1 pass");
    return true;
  }
  else
  {
    return false;
  }
}


void beep_short(int count)
{
  while (count--)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
  digitalWrite(9, LOW);
}
void beep_long(int duration)
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration * 1000);
  digitalWrite(BUZZER_PIN, LOW);

  delay(100);
}
