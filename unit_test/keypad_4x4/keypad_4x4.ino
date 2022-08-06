//4x4薄膜鍵盤範例程式

#include "Adafruit_Keypad.h"

const byte ROWS = 4; // 列數(橫的)
const byte COLS = 4; // 行數(直的)
//鍵盤上每一個按鍵的名稱
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'-','8','9','C'},
  {'+','0','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8}; //定義列的腳位
byte colPins[COLS] = {7, 6, 5, 4}; //定義行的腳位

//初始化鍵盤
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  customKeypad.begin();
  Serial.println("KeyPad Test...");
}

void loop() {
  // 開始偵測使用者的按鍵狀態
  customKeypad.tick();

  //判斷按了哪一個鍵
  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    Serial.print((char)e.bit.KEY);
    if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed"); 
    //按下的狀態是KEY_JUST_PRESSED
    else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");  
    //放開的狀態是KEY_JUST_RELEASED
  }

  delay(10);
}
