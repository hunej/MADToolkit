#include <Wire.h>
#include <MFRC522.h>
#include <IRremote.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);
decode_results results;
int ir_result;
int ir_timer;

#define RST_PIN         8         //MFRC522 RST 
#define SS_PIN          10  //MFRC522 SDA
MFRC522 mfrc522;

#include <TimerOne.h>
#include "TM1637.h"

enum PLAYBACK_MODE
{
    START,
    ALPHA_TEAM_LOSE_EXPLOSION,
    ALPHA_TEAM_WIN_EXPLOSION,
    BRAVO_TEAM_WIN,
    MOUNTING_BOMB_SIREN,
    BOMB_MOUNTED_SIREN,
    TIKTOK,
    GROUNDED_SIREN,

} playback;


#define ON 1
#define OFF 0

#define CLK 5              //CLK
#define DIO 4              //DIO

#define RFID_MOUNT 10
#define RFID_ALPHA 51
#define RFID_BRAVO 172

#define MOUNTED_TIME 30
int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
unsigned char Update;
int rfid_sensed = 0;

int END = 0;
char second = 20;
char minute = 0;
char halfsecond = 2;
int beeptoggle = 0;
volatile int countdown = 0;  //second

int timeout = 0;
int grounded = 0;
int tiktop_stopped = 0;


int stage2 = 0;
int mounted = 0;
int isplaying = 0;
int nowPlayingpPiority = 0;

int alpha_near = 0;
int bravo_near = 0;

TM1637 tm1637(CLK, DIO);



void setup(){ 
  Serial.begin(115200); //Use serial for debugging


    
  Serial.println("Milsig Dartsoft Arduino Toolkit");

pinMode(9,OUTPUT);
//  digitalWrite(9,HIGH);
//  delay(1000);
//  digitalWrite(9,LOW);
//  delay(2000);

  

  tm1637.set();
  tm1637.init(); 

Wire.begin(); //I2C master
Timer1.initialize(500000);//1s
Timer1.attachInterrupt(TimingISR);

  SPI.begin();        //SPI
  mfrc522.PCD_Init(SS_PIN, RST_PIN); 
  Serial.print(F("Reader "));
  Serial.print(F(": "));
  mfrc522.PCD_DumpVersionToSerial(); 


  hint_playback(START, 0);
  delay(5000); //fucking magic number








  irrecv.enableIRIn(); // Start the receiver

  ir_result = -2;
  ir_timer = 0;
  
  simple_beep();
  simple_beep();
  simple_beep();
  
while(IrReceiver.decodedIRData.command!=0x1C) {//OK button
//  if (irrecv.decode(&results)) {
//    Serial.println(results.value, HEX);
//    irrecv.resume(); // Receive the next value
//  }

   if (IrReceiver.decode())
   {
//      Serial.println(IrReceiver.decodedIRData.command, HEX);
      ir_result = IrReceiver.decodedIRData.command;
      ir_result = ir_input_mapping(ir_result);

      Serial.println(ir_result);

if(ir_result<0)
  continue;
else
  Update = ON;

          if (Update == ON) {
        TimeUpdate();
        tm1637.display(TimeDisp);
    }

    
ir_timer = ir_timer*10 + ir_result;


  simple_beep();


        Update = ON;
        IRTimerUpdate(ir_timer);
        tm1637.display(TimeDisp);

        
  
      IrReceiver.resume();
   }


  
}
simple_beep();
simple_beep();
simple_beep();

Serial.print("ir_timer: ");
Serial.print(ir_timer);
Serial.println(", got ir result!!");









  
alpha_near = 0;
bravo_near = 0;
  countdown = ir_timer;//set countdown to 20 sec


  second = ir_timer%60;

  halfsecond = 2;
  minute = ir_timer/60;
stage2 = 0;
  END = 0;

mounted = 0;
isplaying = 0;
nowPlayingpPiority = 0;




} 

void simple_beep(void)
{
  
    digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(9,LOW);
  delay(100);
  }
void loop(){ 

    int sensorValue = analogRead(A0);
//  Serial.println(sensorValue);
  if(sensorValue < 300){   //當光敏電阻給的值小於基準值。這個基準值依個別情況修改
    grounded = 1;
    Serial.println("grounded");
  }else{
    grounded = 0;
  }
  
// Serial.print(mounted);

  // 檢查是不是一張新的卡
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // 顯示卡片內容
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size); // 顯示卡片的UID
      Serial.println();
      
      Serial.println(mfrc522.uid.uidByte[0]);//10: mount

      rfid_sensed = mfrc522.uid.uidByte[0];
    if(rfid_sensed==RFID_MOUNT)
    {
      Serial.println("mounted");
      mounted = 1;
      countdown = MOUNTED_TIME;
      second = MOUNTED_TIME;
      stage2 = 99;
      delay(10);


      }
      else if(rfid_sensed==RFID_ALPHA)
    {
      Serial.println("alpha");
      alpha_near = 1;
      delay(100);
     
      }
      else if(rfid_sensed==172)
    {
      Serial.println("bravo");
      bravo_near = 1; 
      delay(100);
      }

      
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));  //顯示卡片的類型

      mfrc522.PICC_HaltA();  // 卡片進入停止模式
  }

    if (Update == ON) {
        TimeUpdate();
        tm1637.display(TimeDisp);
    }





    if(countdown > 0 && mounted == 0 && stage2==0)// if(countdown > 0 && mounted == 1 && stage2==1)
    {
        if(grounded == 1)
            hint_playback(GROUNDED_SIREN, 4);
        else
            hint_playback(TIKTOK, 5);
            
    }else if(countdown > 0 && mounted == 1 && stage2==1)
    {
        hint_playback(BOMB_MOUNTED_SIREN, 5);   
    }
    
    if(countdown <= 0 && stage2==0)
    {
        hint_playback(ALPHA_TEAM_LOSE_EXPLOSION, 0);
        END = 1;
    }
    else if(stage2>90)
    {
        hint_playback(MOUNTING_BOMB_SIREN, 0);
        stage2--;
//        while(1){//TRAP CITYYYYY
//            Serial.print("TRAP ");
//        }        
      }else if(stage2>=80 && stage2<=90)
    {
        hint_playback(MOUNTING_BOMB_SIREN, 0);
        stage2=1;
//        while(1){//TRAP CITYYYYY
//            Serial.print("TRAP ");
//        }   
    }


    if(countdown <= 0 && stage2==1) //booooooom!!!!
    {
        hint_playback(ALPHA_TEAM_WIN_EXPLOSION, 0);
        END = 1;       
    }
    else if(bravo_near==1 && stage2==1)    //bravo team unmounted
    {
        hint_playback(BRAVO_TEAM_WIN, 0);
        END = 1;
    }

        
//  if(countdown==5){
//    Wire.beginTransmission(3); //開始傳送，指定給從端3號
//    Wire.write("a");
//    Wire.endTransmission(); //結束通訊 
//    countdown++;
//  }else if(countdown==10){
//    Wire.beginTransmission(3); //開始傳送，指定給從端3號
//    Wire.write("b");
//    Wire.endTransmission(); //結束通訊 
//    countdown = 0;
//  }
  
  if(END==1)
  {
    TimeDisp[0] = 0x00;
    TimeDisp[1] = 0x00;
    TimeDisp[2] = 0x00;
    TimeDisp[3] = 0x00;
        tm1637.display(TimeDisp);
    while(1);
    }

} 
int hint_playback(int mode, int priority)
{
  int iii = 1;//fucking magic number
  if(priority < 5)
    iii=3;
  
//    if(isPlaying == FALSE || nowPlayingpPiority > priority) //not playing or lower priority
//    {
//        play_mp3(mode);
//        nowPlayingpPiority = priority;
//    }
    Serial.print("mode: ");
//    Serial.print(mode);


    Wire.beginTransmission(3); //開始傳送，指定給從端3號
    

while(iii--)
{
    switch(mode) {
      case 0:
        Serial.print("START");   Wire.write("1");
        break;      
      case 1:
         Serial.print("ALPHA_TEAM_LOSE_EXPLOSION");  Wire.write("2");
        break;
      case 2:
          Serial.print("ALPHA_TEAM_WIN_EXPLOSION"); Wire.write("3");
        break;
      case 3:
         Serial.print("BRAVO_TEAM_WIN");  Wire.write("4");
        break;
      case 4:
         Serial.print("MOUNTING_BOMB_SIREN"); Wire.write("5");
        break;
      case 5:
         Serial.print("BOMB_MOUNTED_SIREN");  Wire.write("6");
        break;

      case 6:
         Serial.print("TIKTOK");  Wire.write("7");
        break;
    case 7:
         Serial.print("GROUNDED_SIREN");  Wire.write("8");
        break;
      default:
        Serial.println("Invalid number");
    }
    delay(10); //fucking magic number
}


 
    Wire.endTransmission(); //結束通訊 

    
    Serial.print(" ,priority: ");
    Serial.println(priority);
    return 0;
}

void TimingISR2() {
  countdown--;
}

void TimingISR() {//0.5s
  halfsecond --;

  
    if(END==1){
     return;
    }
    
Serial.println(countdown);
//Serial.println(END);





beeptoggle = (~beeptoggle);
if(beeptoggle)
digitalWrite(9,HIGH);
  else
  digitalWrite(9,LOW);

Update = ON;

      if (halfsecond == 0) {
    countdown--;

    
    if(countdown<=0){
      END = 1;
    }

    
           second --;

 
        if (second == 0) {
            minute --;
            second = 60;
        }
    
             halfsecond = 2;
    }
    ClockPoint = (~ClockPoint) & 0x01;
}
int TimeUpdate(void) {
    if (ClockPoint) {
        tm1637.point(POINT_ON);
    } else {
        tm1637.point(POINT_OFF);
    }
    TimeDisp[0] = minute / 10;
    TimeDisp[1] = minute % 10;
    TimeDisp[2] = second / 10;
    TimeDisp[3] = second % 10;
    Update = OFF;

    if(minute<=0)
      return 1;//timeout
    else
      return 0;
}

int IRTimerUpdate(int time_input) {

  int _min, _sec;
  
    tm1637.point(POINT_OFF);
    _min = time_input / 60;
    _sec = time_input % 60;

    Serial.print("time: ");
    Serial.print(_min);
    Serial.print(":");
    Serial.println(_sec);
    
    TimeDisp[0] = _min / 10;
    TimeDisp[1] = _min % 10;
    TimeDisp[2] = _sec / 10;
    TimeDisp[3] = _sec % 10;
    Update = OFF;

    return 0;
}
int ir_input_mapping(int input)
{
  int mapped = -1;
//25,69,70,71,68,64,67,7,21,9
//Serial.println(input);
switch(input) {
      case 25:
        mapped = 0;
        break;      
      case 69:
        mapped = 1;
        break;   
      case 70:
        mapped = 2;
        break;   
      case 71:
        mapped = 3;
        break;   
      case 68:
        mapped = 4;
        break;   
      case 64:
        mapped = 5;
        break;   
      case 67:
        mapped = 6;
        break;   
      case 7:
        mapped = 7;
        break;   
      case 21:
        mapped = 8;
        break;   
      case 9:
        mapped = 9;
        break;                                                           
      default:
        mapped = -1;
    }

      
  return mapped;
  }
/**
 * UID serial print
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
