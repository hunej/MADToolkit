
#include <IRremote.h>
int RECV_PIN = 2;//IR

IRrecv irrecv(RECV_PIN);
decode_results results;
int ir_result;
int ir_timer;



void setup()
{
  Serial.begin(115200);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn();
  Serial.println("Enabled IRin");
}

void loop() {
// receive countdown setting from ir controller
  while (IrReceiver.decodedIRData.command != 0x1C) // 0x1C is OK button
  {
                Serial.println(IrReceiver.decodedIRData.command, HEX);

    if (IrReceiver.decode())
    {
            Serial.println(IrReceiver.decodedIRData.command, HEX);
      ir_result = IrReceiver.decodedIRData.command;



      IrReceiver.resume();
    }
  }
  
  delay(100);
}
