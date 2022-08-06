#include <TimerOne.h>
#include "TM1637.h"

#define CLK 2 // TM1637 CLK
#define DIO 3 // TM1637 DIO

#define BUZZER_PIN 13
#define BUTTON_ADD_PIN 5
#define BUTTON_SUB_PIN 6
#define BUTTON_START_PIN 7

#define ISR_PERIOD 10//ms

#define ON 1
#define OFF 0
TM1637 tm1637(CLK, DIO);

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
unsigned char Update;



unsigned long timer_10ms;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello");
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT); // BUZZER
  beep_short(3);

  // Button
  pinMode(BUTTON_ADD_PIN, INPUT);
  pinMode(BUTTON_SUB_PIN, INPUT);
  pinMode(BUTTON_START_PIN, INPUT);
  
  // TM1637
  tm1637.set();
  tm1637.init();
  tm1637.point(POINT_ON);
  tm1637.display(TimeDisp);
  




  while(digitalRead(BUTTON_START_PIN) == HIGH)
  {
    
  }
  Serial.println("Start!");

  Timer1.initialize(100000);//10ms isr
  Timer1.attachInterrupt(TimingISR);

  timer_10ms = 0;  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Update == ON)
  {
    TimeUpdate(timer_10ms);
    tm1637.display(TimeDisp);
//    Serial.println(digitalRead(BUTTON_ADD_PIN));
  }
//  Serial.println(digitalRead(BUTTON_ADD_PIN));
  delay(1000);
}


void TimingISR()
{ //10ms
  
  
  timer_10ms++;
  if(timer_10ms%10==0)
    Update = ON;
  

}

int TimeUpdate(int time_input)
{
  int second, msecond;
  if (ClockPoint)
  {
    tm1637.point(POINT_ON);
  }
  else
  {
    tm1637.point(POINT_OFF);
  }

  second = time_input / 10;
  msecond = time_input % 10;

  TimeDisp[0] = second / 10;
  TimeDisp[1] = second % 10;
  TimeDisp[2] = msecond / 10;
  TimeDisp[3] = msecond % 10;
  Update = OFF;

  return 0;
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
