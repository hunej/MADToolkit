# MADSDK: Milsig Arduino DartSoft Development Kit

## Summary
This repository contains the Milsig Arduino DartSoft Development Kit, which can can be built as a dartsoft or airsoft task props box. 

###There are three versions of MADSDK: 
* Duo-Uno: Two Arduino Uno Board are used as master board and slave board, which are communicated via I2C
* Mega: Single Arduino Mega Board with full functionality
* Nano/Single-Uno: Single Arduino Nano/Uno Board with limited functionality

## Description
### Duo-Uno: Two Arduino Uno Board
The master board uses firmware interrupt for internal timer to trigger events. It also needs some peripherals such as LED Display(TM1637), Active Piezo Buzzer, RFID Sensor(RC522 for card sensing, optional), IR Remote Control(for timer setup, optional), Photoresistor(Optional), Potentiometer(B10k for Photoresistor threshold controlling, optional), RGB Led Light Module(Optional).

The audio playback event is sent via I2C to trigger slave board's MP3 audio decoder(VS1053B). An AUX line and external speaker is needed. 

User can use master board alone without slave board if MP3 playback function is not required. There are lots of unused pins on slave board can be used as custom function implementation.

### Mega: Single Arduino Mega Board
TODO...

### Nano/Single-Uno: Single Arduino Nano/Uno Board
TODO...


## Library
### Duo-Uno Master
* IRremote by shirriff
* TimerOne.h
* TM1637.h
* MFRC522
### Duo-Uno Slave
* SdFat_exFat by Bill Greiman
* vs1053_sdFat by mpflaga

### Mega
* TODO...

### Nano
* TODO...


## Updates 
### 2022/07/08
* First Commit
* Add a Potentiometer for Photoresistor threshold controlling
* Add mounting stage timer compensation
* File path correction
* Timer input refinement
* add external reset button to trigger reset pin
### 2022/07/09
* add pin information
### 2022/07/10
* change p5 to p7 for pwm pin that rgb module needed
* add rgb led light module
* remove button 7~9's functionality on IR controller
* mp3 audio content rework, add coffin dance music


## TODO : 
* STOP-CMD function implementation for I2C mp3 playback
* skippable instruction audios
* code optimization, add comment, indent correction
* change Photoresistor to PING Sensor
* add ENABLE_SLAVE_BOARD mode flag for master board used alone
* magic number elimination
* demo box installation tutorial
* 11.1v lipo for two Arduino Uno board powerining issue
* Single Arduino Mega Board ver, w/ DFPlayer mp3 codec and internal speaker demobox implementation
* Single Arduino Nano/Uno Board ver, w/o audio playback demobox implementation
