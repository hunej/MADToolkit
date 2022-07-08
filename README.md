# Milsig Dartsoft Arduino Toolkit

## Summary
This repository contains the Arduino code which can can be built as a dartsoft or airsoft task props box. Two Arduino Uno Board are used as master board and slave board,  which are communicated via i2c. 

The master board contains internal timer(firmware interrupt), rfid sensor(RC522), buzzer with internal oscillator, photoresistor, IR Remote Control. 

The audio playback event is sent via i2c to trigger slave board's mp3 audio decoder(VS1053B). An AUX line and external speaker is needed. 

User can use master board alone without slave board if mp3 playback function is not required.


## Updates 

### 2022/07/08
* First Commit


## TODO : 
1. timer input refinement
2. add 5000ms delay to mounting stage
3. mp3 audio content rework, add coffin dance music
4. code optimization, add comment, indent correction
5. add pin info documents
6. add ENABLE_SLAVE_BOARD mode flag for master board used alone
7. magic number elimination
8. photoresistor threshold tuning
9. demo box installation tutorial
10. 11.1v lipo for two arduino uno board powerining issue
