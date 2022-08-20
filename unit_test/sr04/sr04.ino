//https://atceiling.blogspot.com/2017/03/arduino_28.html

int trigPin = 12;                  //Trig Pin
int echoPin = 11;                  //Echo Pin
long duration, cm, inches;
 
void setup() {
  Serial.begin (115200);             // Serial Port begin
  pinMode(trigPin, OUTPUT);        // 定義輸入及輸出 
  pinMode(echoPin, INPUT);
}
 
void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
 
  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
  inches = (duration/2) / 74; 

  Serial.print("Distance : ");  
  Serial.print(inches);
  Serial.print("in,   ");
  Serial.print(cm);
  Serial.print("cm,   ");
  Serial.print(duration);
  Serial.print("duration");
  
  Serial.println();
  
  
  delay(250);
}
