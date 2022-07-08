# Milsig Dartsoft Arduino Toolkit

## Summary
This repository contains the Arduino code which can can be built as a dartsoft or airsoft task props box. Two Arduino Uno Board are used as master board and slave board,  which are communicated via I2C. 

The master board needs some peripherals such as LED Display(TM1637), Internal Timer(Firmware Interrupt), RFID Sensor(RC522), Buzzer with Internal Oscillator, Photoresistor, IR Remote Control, Potentiometer(B10k).

The audio playback event is sent via I2C to trigger slave board's MP3 audio decoder(VS1053B). An AUX line and external speaker is needed. 

User can use master board alone without slave board if MP3 playback function is not required.


## Updates 

### 2022/07/08
* First Commit
* Add a Potentiometer for Photoresistor threshold controlling
* Add mounting stage timer compensation
* File path correction
* Timer input refinement

## TODO : 
1. mp3 audio content rework, add coffin dance music
2. add pin info documents
3. code optimization, add comment, indent correction
4. add ENABLE_SLAVE_BOARD mode flag for master board used alone
5. magic number elimination
6. demo box installation tutorial
7. 11.1v lipo for two arduino uno board powerining issue
