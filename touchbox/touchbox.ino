
// Header
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define
#define BUZZER_TIMER_PIN 7
#define BUZZER_VIOLATION_PIN 8

#define echoPinAlpha 9
#define trigPinAlpha 10
#define echoPinBravo 11
#define trigPinBravo 12

#define onTouchThr 400



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
void TimingISR();
void beep_short(int count, int pin);

// Global variables
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // LCD

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
  lcd.print("MADSDK");
  delay(1000);


  // Buzzer
  pinMode(BUZZER_TIMER_PIN, OUTPUT);
  pinMode(BUZZER_VIOLATION_PIN, OUTPUT);

  beep_short(3, BUZZER_TIMER_PIN);
  beep_short(3, BUZZER_VIOLATION_PIN);


  //SR04
  pinMode(trigPinAlpha, OUTPUT);
  pinMode(echoPinAlpha, INPUT);  
  pinMode(trigPinBravo, OUTPUT);
  pinMode(echoPinBravo, INPUT);   

  // ISR
  Timer1.initialize(500000); // 0.5s
  Timer1.attachInterrupt(TimingISR);  
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALPHA: ");
  lcd.print(alpha_sensing_time / 2);
  if (alpha_sensing_time % 2)
    lcd.print(".5s");
  else
    lcd.print(".0s");

  lcd.setCursor(0, 1);
  lcd.print("BRAVO: ");
  lcd.print(bravo_sensing_time / 2);
  if (bravo_sensing_time % 2)
    lcd.print(".5s");
  else
    lcd.print(".0s");



  digitalWrite(trigPinAlpha, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinAlpha, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinAlpha, LOW);

  pinMode(echoPinAlpha, INPUT);
  duration = pulseIn(echoPinAlpha, HIGH);

  if(duration<=onTouchThr)
  {
    onTouchAlpha = true;
    DEBUG_PRINT("alpha in! ");
  }
  else
  {
    DEBUG_PRINT("alpha out! ");
    onTouchAlpha = false;
  }

    
  DEBUG_PRINT(duration);
  DEBUG_PRINT(", ");

  digitalWrite(trigPinBravo, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinBravo, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinBravo, LOW);

  pinMode(echoPinBravo, INPUT);
  duration = pulseIn(echoPinBravo, HIGH);

  if(duration<=onTouchThr)
    onTouchBravo = true;
  else
    onTouchBravo = false;

    
  DEBUG_PRINTLN(duration);
}


void TimingISR()
{ // 0.5s


  halfsecond--;
  if (onTouchAlpha)
  {
    alpha_sensing_time++;
  }

  if (onTouchBravo)
  {
    bravo_sensing_time++;
  }  

  if (!(onTouchAlpha || onTouchBravo))
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
