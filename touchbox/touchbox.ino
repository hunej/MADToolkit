
// Header
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define
#define BUZZER_TIMER_PIN 7
#define BUZZER_VIOLATION_PIN 2

#define PinAlpha 9
#define PinBravo 11

#define PinSwitch 4


//#define DEBUG

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
void TimingISR();
void beep_short(int count, int pin);

// Global variables
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // LCD

int countdown = 0;
bool isEND = false;
int second = 20;
int minute = 0;
int halfsecond = 2;
int sensing_time = 0;
int alpha_sensing_time = 0;
int bravo_sensing_time = 0;
long duration;


int beeptoggle = 0;
bool onTouchAlpha = false;
bool onTouchBravo = false;
bool onTouchSwitch = false;
  

void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ;          // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  
  // LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Milsig Taiwan");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("TouchBox");
  delay(1000);


  // Buzzer
  pinMode(BUZZER_TIMER_PIN, OUTPUT);
  pinMode(BUZZER_VIOLATION_PIN, OUTPUT);

  beep_short(3, BUZZER_TIMER_PIN);
  beep_short(3, BUZZER_VIOLATION_PIN);


  //touch
  pinMode(PinAlpha, INPUT);
  pinMode(PinBravo, INPUT);

  //switch
  pinMode(PinSwitch, INPUT);
  

  // ISR
  Timer1.initialize(500000); // 0.5s
  Timer1.attachInterrupt(TimingISR);  


  pinMode(A0, INPUT_PULLUP);//selector1
  pinMode(A1, INPUT_PULLUP);//selector2

  if (digitalRead(A0)==1 && digitalRead(A1)==1)//5min
    countdown = 300;
  else if (digitalRead(A0)==0 && digitalRead(A1)==1)//7min
    countdown = 420;
  else if (digitalRead(A0)==1 && digitalRead(A1)==0)//10min
    countdown = 600;
  else
    countdown = 720;
    
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(alpha_sensing_time / 2);
  if (alpha_sensing_time % 2)
    lcd.print(".5s");
  else
    lcd.print(".0s");

  
  lcd.print(" B:");
  lcd.print(bravo_sensing_time / 2);
  if (bravo_sensing_time % 2)
    lcd.print(".5s");
  else
    lcd.print(".0s");

  lcd.setCursor(0, 1);
  lcd.print("COUNTDOWN: ");
  lcd.print(countdown);
  lcd.print("s");

  

  if(digitalRead(PinAlpha)==HIGH)
  {
    onTouchAlpha = true;
    DEBUG_PRINTLN("alpha on!");
  }
  else
    onTouchAlpha = false;
  
  if(digitalRead(PinBravo)==HIGH)
  {
    onTouchBravo = true;
    DEBUG_PRINTLN("bravo on!");
  }
  else
    onTouchBravo = false;

  if(digitalRead(PinSwitch)==LOW)
  {
    onTouchSwitch = true;
    DEBUG_PRINTLN("switch on!");
  }
  else
    onTouchSwitch = false;

  
  delay(100);


  if(countdown<=0)
  {
    isEND = true;
    digitalWrite(BUZZER_VIOLATION_PIN, LOW);
    digitalWrite(BUZZER_TIMER_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("STOP!");

    while(1);  
  }
}


void TimingISR()
{ // 0.5s
  if(isEND)
    return;

  halfsecond--;
  if (onTouchAlpha)
  {
    alpha_sensing_time++;
  }

  if (onTouchBravo)
  {
    bravo_sensing_time++;
  }  

  if (!(onTouchAlpha || onTouchBravo) && onTouchSwitch)
  {
    digitalWrite(BUZZER_VIOLATION_PIN, HIGH);
  }
  else
  {
    digitalWrite(BUZZER_VIOLATION_PIN, LOW);
  }

    
  beeptoggle = (~beeptoggle);
  if (beeptoggle && (onTouchAlpha || onTouchBravo))
  {
    digitalWrite(BUZZER_TIMER_PIN, HIGH);
  }
  else
  {
    digitalWrite(BUZZER_TIMER_PIN, LOW);
  }

  if (halfsecond == 0)
  {
    halfsecond = 2;
    countdown--;
  }
  
}




void beep_short(int count, int pin)
{
  while (count--)
  {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }
  digitalWrite(pin, LOW);
}
