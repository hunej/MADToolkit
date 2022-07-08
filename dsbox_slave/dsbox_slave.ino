#include <Wire.h>
#define led 13

volatile byte x1, x2, x3, x4;
volatile int y1, y2;
bool flag1 = false;

char trackName[] = "0001_START.mp3";

char t0001[] = "0001_START.mp3";
char t0002[] = "0002_ALPHA_TEAM_LOSE_EXPLOSION.mp3";
char t0003[] = "0003_ALPHA_TEAM_WIN_EXPLOSION.mp3";
char t0004[] = "0004_BRAVO_TEAM_WIN.mp3";
char t0005[] = "0005_MOUNTING_BOMB_SIREN.mp3";
char t0006[] = "0006_BOMB_MOUNTED_SIREN.mp3";
char t0007[] = "0007_TIKTOK_XX.mp3";
char t0008[] = "0008_GROUNDED_SIREN.mp3";
char t0009[] = "0009_WELCOME";

const char *flist[20];
int tid_priority[] = {-1, 0, 0, 0, 0, 2, 3, 5, 5, 0};
int nowPlaying_tid = 0;
int toPlay_tid = 0;

#include <SPI.h>
#include "SdFat.h"            //http://librarymanager/All#SdFat_exFat by Bill Greiman. We currently use v2.1.2
const int pin_microSD_CS = 9; // Default pin for the SparkFun MP3 Shield
SdFat sd;

#include "vs1053_SdFat.h" //http://librarymanager/All#vs1053_sdFat by mpflaga

vs1053 MP3player;

state_m vs1053State;

void setup()
{

  flist[1] = t0001;
  flist[2] = t0002;
  flist[3] = t0003;
  flist[4] = t0004;
  flist[5] = t0005;
  flist[6] = t0006;
  flist[7] = t0007;
  flist[8] = t0008;
  flist[9] = t0009;

  Wire.begin(3);                // I2C位址與連線設定，這裡設定為3號
  Wire.onReceive(receiveEvent); //當收到I2C訊號時，啟動的動作
  Serial.begin(115200);         // Serial輸出鮑率為9600
  Serial.println("MP3 Testing");

  if (sd.begin(pin_microSD_CS) == false)
  {
    Serial.println(F("SD init failed. Is card present? Formatted? Freezing..."));
    while (1)
      ;
  }
  MP3player.begin();
  MP3player.setVolume(40, 40); //(Left, Right) 40 is pretty good for ear buds

  //  MP3player.playMP3(t0001);

  ////  while(MP3player.getState() == 5);
  //  delay(5000);
}
char c;
int c_tmp;
void loop()
{

  while (1)
  {
    Serial.println(c);
    c_tmp = c - 48;
    Serial.println(c_tmp);
    if (c_tmp < 1 || c_tmp > 20) // pass if no valid i2c msg got
      continue;
    else if (c_tmp == 67 && nowPlaying_tid == 8)
    {
      //    Serial.println(c_tmp);
      MP3player.stopTrack();
    }

    vs1053State = MP3player.getState();
    //  Serial.println(vs1053State);
    if (vs1053State == 5) // playing & got i2c msg
    {
      toPlay_tid = c_tmp;
    }
    else // idle & got i2c msg
    {
      nowPlaying_tid = 0;
      toPlay_tid = c_tmp;
    }

    if (nowPlaying_tid == 0 || tid_priority[toPlay_tid] <= tid_priority[nowPlaying_tid] || toPlay_tid != nowPlaying_tid))// not playing or higher priority sended or grounded recovered
    {
      nowPlaying_tid = toPlay_tid;
      c = '0';
      Serial.print("play: ");
      Serial.println(flist[toPlay_tid]);
      MP3player.stopTrack();
      MP3player.playMP3(flist[toPlay_tid]);
    }

    //  Serial.println(c_tmp, DEC);//-48 is the offset

    // char t0001[] = "0001_START.mp3";
    // char t0002[] = "0002_ALPHA_TEAM_LOSE_EXPLOSION.mp3";
    // char t0003[] = "0003_ALPHA_TEAM_WIN_EXPLOSION.mp3";
    // char t0004[] = "0004_BRAVO_TEAM_WIN.mp3";
    // char t0005[] = "0005_MOUNTING_BOMB_SIREN.mp3";
    // char t0006[] = "0006_BOMB_MOUNTED_SIRENR.mp3";
    // char t0007[] = "0007_TIKTOK_XX.mp3";
    // char t0008[] = "0008_GROUNDED_SIREN.mp3";

    //  if(c == '1'){
    //    c = '0';
    //  Serial.print("aaa");
    //  MP3player.stopTrack();
    //  MP3player.playMP3(t0001);
    //}
    // else if(c == '2'){
    //  c = '0';
    //  MP3player.stopTrack();
    // MP3player.playMP3(t0002);
    // }
    // else if(c == '3'){
    //  c = '0';
    //  MP3player.stopTrack();
    // MP3player.playMP3(t0003);
    // }
    // else if(c == '4'){
    //  c = '0';
    //  MP3player.stopTrack();
    // MP3player.playMP3(t0004);
    // }
    // else if(c == '5'){
    //  c = '0';
    //  MP3player.stopTrack();
    // MP3player.playMP3(t0005);
    // }
    // else if(c == '6'){
    //  c = '0';
    ////  MP3player.stopTrack();
    //  if(vs1053State==4)
    //    MP3player.playMP3(t0006);
    // }
    //  else if(c == '7'){
    //  c = '0';
    ////  MP3player.stopTrack();
    //  if(vs1053State==4)
    //    MP3player.playMP3(t0007);
    // }
    // else if(c == '8'){
    //  c = '0';
    //  MP3player.stopTrack();
    // MP3player.playMP3(t0008);
    // }

    delay(100); //延遲0.1秒
  }
}
//自定義的接收後的啟動副程式
void receiveEvent(int numBytes)
{
  while (Wire.available())
  {                  //判斷Wire.available()有沒有訊號
    c = Wire.read(); //將傳入的訊號Wire.read()指定給字串C
    // Serial.print(c); //透過Serial印出字串C的內容
  }
  // if(c == 49){
  //   Serial.print("1");
  //   MP3player.playMP3(t0007);
  // }
  //  else{
  //   Serial.print("2");
  //  MP3player.playMP3(t0006);
  //  }
}
