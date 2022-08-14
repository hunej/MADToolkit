#include <TimerOne.h>
#include "TM1637.h"


#define DELTA_THR 120
#define DELTA_THR_HALF 60

#define PULSE 7
#define CLK 8 // TM1637 CLK
#define DIO 9 // TM1637 DIO

int ammo;

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};

TM1637 tm1637(CLK, DIO);


void setup() {
//  Serial.begin(115200);

  tm1637.set();
  tm1637.init();
  tm1637.point(POINT_ON);
  tm1637.display(TimeDisp);
  
  ammo = 0;

}


void loop() {
  while(digitalRead(PULSE)==0);

  
  ammo++;
  TimeDisp[2] = ammo / 10;
  TimeDisp[3] = ammo % 10;
  tm1637.display(TimeDisp);
  tm1637.point(POINT_OFF);

  delay(DELTA_THR_HALF);
}
