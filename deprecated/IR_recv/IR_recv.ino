/*
本程式為紅外線遙控接收的範例

首先要安裝IRremote by shirriff的函式庫

紅外線套件賣場：
https://goods.ruten.com.tw/item/show?21707292633563

傑森創工賣場：https://goo.gl/EWoPQ4

傑森創工粉絲團：
https://www.facebook.com/jasonshow
 */

#include <IRremote.h>
//設定紅外線接收頭的PIN腳
#define RECV_PIN 2//int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);

//接收訊號後會把結果存在results
decode_results results;

void setup()
{
  Serial.begin(115200);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // 開始接收訊號！
  Serial.println("Enabled IRin");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(IrReceiver.decodedIRData.command); //接收訊號，以16進位型式輸出到監控視窗
    
    if(results.value == 0xFF38C7){  //如果按下遙控器的特定鍵，就顯示訊息！
      Serial.println("Turn On!");
    }
    
    irrecv.resume(); // 接著接收下一個訊號
  }
  delay(100);
}
