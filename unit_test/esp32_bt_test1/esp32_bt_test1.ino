#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino



BluetoothSerial ESP_BT; //Object for Bluetooth


int incoming;


void setup() {

  Serial.begin(115200); //Start Serial monitor in 9600

  ESP_BT.begin("ESP32_LED_Control"); //Name of your Bluetooth Signal

  Serial.println("Bluetooth Device is Ready to Pair");


}


void loop() {

  

  if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {

    incoming = ESP_BT.read(); //Read what we recevive 

    Serial.print("Received:"); Serial.println(incoming);

  }

  delay(20);

}
