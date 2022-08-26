//https://sites.google.com/view/rayarduino/touch-sensor

int touchSensor = 3;    //觸摸感測器 SIG 連接 Arduino pin 12
int sensorValue = 0;     //設定觸摸感測器感測值為 sensorValue

void setup() {
  Serial.begin(115200);            //設定序列螢幕和電腦之間資料傳輸速率為 9600 bits 每秒
  pinMode(touchSensor, INPUT);   //Arduino 讀入聲音感測器感測值
}

void loop() {
  sensorValue = digitalRead(touchSensor);             //Arduino 數位讀入觸摸感測器感測值. 並存入 sensorValue 
                                                      //感測器被觸摸時讀值為HIGH (1)，感測器不被觸摸時讀值為 LOW(0)
  Serial.println(sensorValue);                        //印出感測值於序列螢幕
  if(digitalRead(touchSensor)==HIGH){                 //Arduino 數位讀入感測器的感測值，//當感測器被觸摸時  邏輯變數 lift 為假
    //
  }
  
  delay(100);
}
