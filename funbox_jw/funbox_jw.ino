// Softwarereset lib

// Header
#include <SPI.h>
#include <MFRC522.h>
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <IRremote.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "SoftwareReset.hpp"

// Define
#define RF_RST_PIN 9
#define RF_SS_SDA_PIN 10
#define BUZZER_PIN 4
#define MP3_RX_PIN 6
#define MP3_TX_PIN 7
//#define IR_RECV_PIN 2
#define GAME_MODE_PIN 2
#define KEY_PIN 5


#define MP3_VOL 30

//#define MILSIG


#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTHEX(x, y) Serial.print(x, y)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTHEX(x, y)
#define DEBUG_PRINTLN(x)
#endif

// Functions
bool rfid_exp_sensing(int stage);
void TimingISR();
void exp_lcd_judge(int exp_judge_mode);
void exp_lcd_handling(int cntdwn, int sensing_t, int stage);
bool exp_cmp_stage_match(int stage, byte uid);
void beep_short(int count);
void beep_long(int duration);


// Global variables
MFRC522 mfrc522(RF_SS_SDA_PIN, RF_RST_PIN);                    // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // LCD
SoftwareSerial MP3Serial(MP3_RX_PIN, MP3_TX_PIN);              // RX, TX
DFRobotDFPlayerMini myDFPlayer;
//IRrecv irrecv(IR_RECV_PIN);

bool rfid_tag_present_prev = false;
bool rfid_tag_present = false;
int rfid_error_counter = 0;
bool rfid_tag_found = false;

bool stage_ahead = false;
bool END = false;
int second = 20;
int minute = 0;
int halfsecond = 2;
int beeptoggle = 0;
int countdown = 0; // second
int sensing_time = 0;
int alpha_sensing_time = 0;
int bravo_sensing_time = 0;
int current_stage = 0;


unsigned long mp3_timer;


// Enum
enum PLAY_MODE_STAGE
{
  EXP_MODE_STAGE0 = 0,
  EXP_MODE_STAGE1 = 1,
  EXP_MODE_STAGE2 = 2,
  DOM_MODE_STAGE0 = 3,
  IDLE_MODE_STAGE0 = -1,

} playstage;

enum STAGE_CNTDWN_TIMING
{
  EXP_MODE_STAGE0_TIMING = 600, // 10min
  //EXP_MODE_STAGE1_TIMING = 30,  // 30sec
  EXP_MODE_STAGE2_TIMING = 0,
  DOM_MODE_STAGE0_TIMING = 600, // 10min

} cnt_time;

int EXP_MODE_STAGE1_TIMING = 30;
enum EXP_JUDGE_MODE
{
  EXP_MODE_ALPHA_WIN = 0,
  EXP_MODE_ALPHA_LOSE = 1,
  EXP_MODE_BRAVO_WIN = 2,

} exp_judge;

enum PLAYBACK_MODE//nonsense...
{
  MP3_START = 7,
  MP3_ALPHA_TEAM_LOSE_EXPLOSION = 1,
  MP3_ALPHA_TEAM_WIN_EXPLOSION = 2,
  MP3_BRAVO_TEAM_WIN = 3,
  MP3_MOUNTING_BOMB_SIREN = 4,
  MP3_UNMNTING_BOMB_SIREN = 5,
  MP3_BOMB_MOUNTED_SIREN = 6,
  //MP3_GROUNDED_SIREN = 1,//and 7, don't know why

} playback;

enum MP3_LENGTH
{
  MP3_START_LENGTH = 13000,
  MP3_ALPHA_TEAM_LOSE_EXPLOSION_LENGTH = 11000,
  MP3_ALPHA_TEAM_WIN_EXPLOSION_LENGTH = 9000,
  MP3_BRAVO_TEAM_WIN_LENGTH = 4000,
  MP3_MOUNTING_BOMB_SIREN_LENGTH = 2400,
  MP3_UNMNTING_BOMB_SIREN_LENGTH = 2400,
  MP3_BOMB_MOUNTED_SIREN_LENGTH = 7000,
  MP3_GROUNDED_SIREN_LENGTH = 6000,

} mp3len;

enum RFID_CARD_UID
{
  BRAVO1 = 0x1E,
  BRAVO2 = 0x19,
  MOUNT1 = 0x80,
  MOUNT2 = 0x20,

} card_uid;

void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ;          // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin(); // Init SPI bus

  // RFID
  mfrc522.PCD_Init(); // Init MFRC522

  // LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Milsig Taiwan");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("FunBox");
  delay(500);

  // MP3
  MP3Serial.begin(9600);
  DEBUG_PRINTLN();
  DEBUG_PRINTLN(F("DFRobot DFPlayer Mini Demo"));
  DEBUG_PRINTLN(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(MP3Serial))
  { // Use softwareSerial to communicate with mp3.
    DEBUG_PRINTLN(F("Unable to begin:"));
    DEBUG_PRINTLN(F("1.Please recheck the connection!"));
    DEBUG_PRINTLN(F("2.Please insert the SD card!"));
    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  DEBUG_PRINTLN(F("DFPlayer Mini online."));

  myDFPlayer.volume(MP3_VOL); // Set volume value. From 0 to 30

#ifdef MILSIG
  myDFPlayer.play(MP3_START);
  //delay(MP3_START_LENGTH);
#endif
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT); // BUZZER
  //beep_short(3);



  //read setting pin
  pinMode(GAME_MODE_PIN, INPUT_PULLUP);//selector1
  pinMode(KEY_PIN, INPUT_PULLUP);//
  pinMode(A0, INPUT_PULLUP);//selector1
  pinMode(A1, INPUT_PULLUP);//selector2



  lcd.clear();
  lcd.setCursor(0, 0);

  
  if (digitalRead(GAME_MODE_PIN)==1)
  {
    current_stage = EXP_MODE_STAGE0;
    lcd.print("EXPLOSION");
  }
  else
  {
    current_stage = DOM_MODE_STAGE0;
    lcd.print("DOMINATION");
  }
  lcd.print(" MODE");

      
    
  if (digitalRead(A0)==1 && digitalRead(A1)==1)//5min
  {
    countdown = 300;
    EXP_MODE_STAGE1_TIMING = 30;
  }else if (digitalRead(A0)==0 && digitalRead(A1)==1)//10min
  {
    countdown = 600;
    EXP_MODE_STAGE1_TIMING = 60;
  }else if (digitalRead(A0)==1 && digitalRead(A1)==0)//15min
  {
    countdown = 900;
    EXP_MODE_STAGE1_TIMING = 90;
  }else//20min
  {
    countdown = 1200;
    EXP_MODE_STAGE1_TIMING = 120;
  }

  lcd.setCursor(0, 1);
  lcd.print("COUNTDOWN: ");
  lcd.print(countdown);
  lcd.print("s");
  delay(3000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WAIT KEY UNLOCK");
  
  while (digitalRead(KEY_PIN)==1)
  {
    
  }
  lcd.clear();
  
  delay(1000);
  lcd.clear();
  lcd.print("3");
  beep_short(1);
  delay(800);
  lcd.clear();
  lcd.print("2");
  beep_short(1);
  delay(800);
  lcd.clear();
  lcd.print("1");
  beep_short(1);
  delay(800);
  lcd.print("START!");

    




  // ISR
  Timer1.initialize(500000); // 0.5s
  Timer1.attachInterrupt(TimingISR);

  
}

void loop()
{
  if (current_stage == DOM_MODE_STAGE0)
    dom_mode_loop();
  else
    exp_mode_loop();
}


void dom_mode_loop()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(alpha_sensing_time / 2);
  lcd.print("s");
//  if (alpha_sensing_time % 2)
//    lcd.print(".5s");
//  else
//    lcd.print(".0s");

  //lcd.setCursor(0, 1);
  lcd.print(" B:");
  lcd.print(bravo_sensing_time / 2);
  lcd.print("s");
//  if (bravo_sensing_time % 2)
//    lcd.print(".5s");
//  else
//    lcd.print(".0s");

  lcd.setCursor(0, 1);
  lcd.print("COUNTDOWN: ");
  lcd.print(countdown);
  lcd.print("s");

  

  rfid_dom_sensing();

  if (countdown <= 0/* || alpha_sensing_time > EXP_MODE_STAGE1_TIMING || bravo_sensing_time > EXP_MODE_STAGE1_TIMING*/)
  {

    if (alpha_sensing_time > bravo_sensing_time)
    {
      DEBUG_PRINTLN("ALPHA WIN");
      exp_lcd_judge(EXP_MODE_ALPHA_WIN);
    }
    else
    {

      DEBUG_PRINTLN("BRAVO WIN");
      exp_lcd_judge(EXP_MODE_BRAVO_WIN);
    }

    END = true;
  }

  if (END == true)
  {
    beep_short(5);
    //while(1);
    //lcd.clear();
    
    
    lcd.setCursor(0, 1);
    lcd.print("WAIT KEY RESET");
    while (digitalRead(KEY_PIN)==0)
    {
      
    }
    softwareReset::standard();
    //lcd.clear();

  
  }
}
void exp_mode_loop()
{

  if (halfsecond > 0)
  {
    DEBUG_PRINT("countdown: ");
    DEBUG_PRINTLN(countdown);
    DEBUG_PRINT("current_stage: ");
    DEBUG_PRINTLN(current_stage);
  }
  exp_lcd_handling(countdown, sensing_time, current_stage);

  stage_ahead = rfid_exp_sensing(current_stage);

  if (stage_ahead && current_stage == EXP_MODE_STAGE0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MOUNTING DONE!");
    countdown = 999;


    mfrc522.PICC_HaltA();

    mp3_timer = millis();
    myDFPlayer.play(MP3_BOMB_MOUNTED_SIREN);

    sensing_time = 0;
    current_stage++;
    delay(MP3_BOMB_MOUNTED_SIREN_LENGTH);
    countdown = EXP_MODE_STAGE1_TIMING;
  }
  else if (stage_ahead)
  {
    current_stage++;
  }

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
    if (countdown <= 0)
    {
      myDFPlayer.play(MP3_ALPHA_TEAM_LOSE_EXPLOSION);
      DEBUG_PRINTLN("ALPHA LOSE");
      exp_lcd_judge(EXP_MODE_ALPHA_LOSE);
      //        delay(8000);
    }
    break;
  case EXP_MODE_STAGE1:
    // playback BOMB_MOUNTED_SIREN if mp3 idle
    if (millis() - mp3_timer > 7000)
    {
      mp3_timer = millis();
      myDFPlayer.play(MP3_BOMB_MOUNTED_SIREN);
    }

    if (countdown <= 0)
    {
      myDFPlayer.play(MP3_ALPHA_TEAM_WIN_EXPLOSION);
      DEBUG_PRINTLN("ALPHA WIN");
      exp_lcd_judge(EXP_MODE_ALPHA_WIN);
      //        delay(8000);
    }
    break;
  case EXP_MODE_STAGE2:
    //
    myDFPlayer.play(MP3_BRAVO_TEAM_WIN);
    DEBUG_PRINTLN("BRAVO WIN");
    exp_lcd_judge(EXP_MODE_BRAVO_WIN);
    END = true;
    //      delay(8000);
    break;
  default:
    DEBUG_PRINTLN("Invalid stage number");
  }

  if (END == true)
  {
    beep_short(5);
    //while(1);

    lcd.setCursor(0, 1);
    lcd.print("WAIT KEY RESET");
    while (digitalRead(KEY_PIN)==0)
    {
      
    }
    softwareReset::standard();

        
  }
}

void rfid_dom_sensing(void)
{
  int tlast;

  rfid_tag_present_prev = rfid_tag_present;

  rfid_error_counter += 1;
  if (rfid_error_counter > 2)
  {
    rfid_tag_found = false;
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
    rfid_error_counter = 0;
    rfid_tag_found = true;
  }

  rfid_tag_present = rfid_tag_found;

  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev)
  {
    sensing_time = 0;
    DEBUG_PRINT("rfid_tag_found: ");
    DEBUG_PRINT(rfid_tag_found);
    DEBUG_PRINT(", ");

    DEBUG_PRINT("Tag 0x");
    DEBUG_PRINTHEX(mfrc522.uid.uidByte[3], HEX);
    DEBUG_PRINTLN(" found");

    tlast = millis();
  }

  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev)
  {

    //    if(mfrc522.uid.uidByte[3]==ALPHA1 || mfrc522.uid.uidByte[3]==ALPHA2)
    //    alpha_sensing_time+=sensing_time/1000;
    //    else if(mfrc522.uid.uidByte[3]==BRAVO1 || mfrc522.uid.uidByte[3]==BRAVO2)
    //    bravo_sensing_time+=sensing_time/1000;

    sensing_time = 0;
    DEBUG_PRINT("tag_found: ");
    DEBUG_PRINT(rfid_tag_found);
    DEBUG_PRINT(", ");

    tlast = millis() - tlast;
    DEBUG_PRINT("Tag gone: ");
    DEBUG_PRINTLN(tlast);
  }

  return;
}

bool rfid_exp_sensing(int stage)
{
  int tlast;

  rfid_tag_present_prev = rfid_tag_present;

  rfid_error_counter += 1;
  if (rfid_error_counter > 2)
  {
    rfid_tag_found = false;
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
    rfid_error_counter = 0;
    rfid_tag_found = true;
  }

  rfid_tag_present = rfid_tag_found;

  if (!exp_cmp_stage_match(stage, mfrc522.uid.uidByte[3]))
  {
    //    DEBUG_PRINT("false!!");
    sensing_time = 0;
    return false;
  }
  else
  {
    //    DEBUG_PRINT("true!!");
  }

  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev)
  {
    sensing_time = 0;
    DEBUG_PRINT("rfid_tag_found: ");
    DEBUG_PRINT(rfid_tag_found);
    DEBUG_PRINT(", ");

    DEBUG_PRINT("Tag 0x");
    DEBUG_PRINTHEX(mfrc522.uid.uidByte[3], HEX);
    DEBUG_PRINTLN(" found");

    tlast = millis();

    myDFPlayer.stop();
    if (current_stage == EXP_MODE_STAGE0)
    {
      myDFPlayer.play(MP3_MOUNTING_BOMB_SIREN);
    }
    else if (current_stage == EXP_MODE_STAGE1)
    {
      myDFPlayer.stop();
      myDFPlayer.play(MP3_UNMNTING_BOMB_SIREN);
      myDFPlayer.play(MP3_UNMNTING_BOMB_SIREN);
      DEBUG_PRINTLN("myDFPlayer.play(MP3_UNMNTING_BOMB_SIREN);");
    }
  }

  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev)
  {
    sensing_time = 0;
    DEBUG_PRINT("tag_found: ");
    DEBUG_PRINT(rfid_tag_found);
    DEBUG_PRINT(", ");

    tlast = millis() - tlast;
    DEBUG_PRINT("Tag gone: ");
    DEBUG_PRINTLN(tlast);

    myDFPlayer.stop();
  }

  //  if(sensing_time>0){
  //    DEBUG_PRINT("sensing_time: ");
  //    DEBUG_PRINTLN(sensing_time);
  //  }

  if (sensing_time > 3000)
  {
    DEBUG_PRINT("sensing_time: ");
    DEBUG_PRINTLN(sensing_time);
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
    // digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  halfsecond--;

  if (rfid_tag_found)
  {
    sensing_time += 500;

    if (mfrc522.uid.uidByte[3] == MOUNT1 || mfrc522.uid.uidByte[3] == MOUNT2)
      alpha_sensing_time++;
    else if (mfrc522.uid.uidByte[3] == BRAVO1 || mfrc522.uid.uidByte[3] == BRAVO2)
      bravo_sensing_time++;
  }

  //  DEBUG_PRINT("countdown: ");
  //  DEBUG_PRINTLN(countdown);

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

void exp_lcd_judge(int exp_judge_mode)
{
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (exp_judge_mode)
  {
  case EXP_MODE_ALPHA_WIN:
    lcd.print("ALPHA WIN");
    break;
  case EXP_MODE_ALPHA_LOSE:
    lcd.print("ALPHA LOSE");
    break;
  case EXP_MODE_BRAVO_WIN:
    lcd.print("BRAVO WIN");
    break;
  default:
    lcd.print("Invalid");
  }
}
void exp_lcd_handling(int cntdwn, int sensing_t, int stage)
{

  int whitespace = 6;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COUNTDOWN: ");
  lcd.print(cntdwn);
  lcd.print("s");

  DEBUG_PRINTLN(sensing_t);

  if (sensing_t > 0 && stage == EXP_MODE_STAGE0)
  {
    lcd.setCursor(0, 1);
    lcd.print("MOUNTING: ");
    while (sensing_t > 0)
    {

      lcd.print("O");
      whitespace--;
      sensing_t -= 500;
    }
    while (whitespace > 0)
    {
      lcd.print("X");
      whitespace--;
    }
  }
  else if (sensing_t > 0 && stage == EXP_MODE_STAGE1)
  {
    lcd.setCursor(0, 1);
    lcd.print("UNMNTING: ");
    while (sensing_t > 0)
    {

      lcd.print("O");
      whitespace--;
      sensing_t -= 500;
    }
    while (whitespace > 0)
    {
      lcd.print("X");
      whitespace--;
    }
  }
  else if (sensing_t <= 0 && stage == EXP_MODE_STAGE0)
  {
    lcd.setCursor(0, 1);
    lcd.print("WAIT MOUNTING...");
  }
  else if (sensing_t <= 0 && stage == EXP_MODE_STAGE1)
  {
    lcd.setCursor(0, 1);
    lcd.print("WAIT UNMNTING...");
  }

  return;
}
bool exp_cmp_stage_match(int stage, byte uid)
{
  int uid32 = (int)uid;
  //  DEBUG_PRINT("got uid!!!!!!!!!!: 0x");
  //  DEBUG_PRINTLN(uid32, HEX);
  //  DEBUG_PRINT("stage: ");
  //  DEBUG_PRINTLN(stage);

  if (stage == EXP_MODE_STAGE0 && (uid == MOUNT1 || uid == MOUNT2))
  {
    //    DEBUG_PRINTLN("EXP_MODE_STAGE0 pass");
    return true;
  }
  else if (stage == EXP_MODE_STAGE1 && (uid == BRAVO1 || uid == BRAVO2))
  {
    //    DEBUG_PRINTLN("EXP_MODE_STAGE1 pass");
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
  digitalWrite(BUZZER_PIN, LOW);
}
void beep_long(int duration)
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration * 1000);
  digitalWrite(BUZZER_PIN, LOW);

  delay(100);
}
