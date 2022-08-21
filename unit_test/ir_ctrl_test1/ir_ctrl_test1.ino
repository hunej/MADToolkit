
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
  while (1) // 0x1C is OK button
  {             
   //The function decode(&results)) is deprecated and may not work as expected! 
   //Just use decode() without a parameter and IrReceiver.decodedIRData.<fieldname> .
 
    if (IrReceiver.decode())
    {
      
      ir_result = IrReceiver.decodedIRData.command;
      if(ir_result!=0)
        Serial.println(ir_result);


      IrReceiver.resume();
    }
  }
  
  delay(100);
}
