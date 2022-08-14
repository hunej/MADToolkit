#include <TimerOne.h>
#include "TM1637.h"


#define DELTA_THR 120
#define DELTA_THR_HALF 60

#define CLK 8 // TM1637 CLK
#define DIO 9 // TM1637 DIO


#define ISR_PERIOD 100//ms


volatile int tlast;
volatile int tdelta;
int ammo;
volatile int led_lifecycle;
int flicker; 
int qsec;
int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};

TM1637 tm1637(CLK, DIO);
#define RELAY_ON 1      // Define relay on pin state
#define RELAY_OFF 0     // Define relay off pin state
 
#define Relay  2       // Arduino pin where the relay connects



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  tm1637.set();
  tm1637.init();
  tm1637.point(POINT_ON);
  tm1637.display(TimeDisp);
  
  Timer1.initialize(100000);
  Timer1.attachInterrupt(TimingISR);
  
  tlast = millis();
  ammo = 0;

  pinMode(Relay,OUTPUT);
}


void loop() {  
  while(digitalRead(7)==0);

  

  //triggered now
  tdelta = millis() - tlast;
  tlast = millis();
  //Serial.println(tdelta);
  
  ammo++;
  TimeDisp[2] = ammo / 10;
  TimeDisp[3] = ammo % 10;
  tm1637.display(TimeDisp);
  tm1637.point(POINT_OFF);
  digitalWrite(4, HIGH);
  digitalWrite(Relay, RELAY_ON);
  
  if(tdelta>DELTA_THR)//semi
  {
    led_lifecycle = 3000;//3sec
    flicker = 0;  
  }
  else//full
  {
    led_lifecycle = 1000;//1sec
    flicker = 1;  
  }
  //digitalWrite(4, HIGH);
  delay(DELTA_THR_HALF);
}




int relay_toggle = 0;
void TimingISR()
{ // 250ms
  //halfsecond--;

   
  qsec++;
  
  if(qsec == 4)
  {
      qsec = 0;
  }

  if(led_lifecycle>0)
  {
    led_lifecycle -= ISR_PERIOD;
    digitalWrite(4, HIGH);
    digitalWrite(Relay, RELAY_ON);
//    if(flicker && qsec > 2)
//      digitalWrite(4, LOW);
//    else
//      digitalWrite(4, HIGH);
  }
  else
  {
    led_lifecycle = 0;  
    digitalWrite(4, LOW);
    digitalWrite(Relay, RELAY_OFF);
  }

}
