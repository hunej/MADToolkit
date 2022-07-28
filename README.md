# MADSDK: Milsig Airsoft & Dartsoft Software Development Kit

## Summary
This repository contains the Milsig Airsoft & Dartsoft Software Development Kit, which can can be built as a dartsoft or airsoft task props box. 

### There are three versions of MADSDK: 
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

### iOS APP
TODO...

### Android APP
TODO...

## Getting Started
TODO...

## Library Dependency
### Duo-Uno Master
* IRremote by Armin Joachimsmeyer
* TimerOne by Paul Stoffregen
* Grove 4-Digital Display by Seeed Studio
* MFRC522 by GithubCommunity
### Duo-Uno Slave
* SdFat_exFat by Bill Greiman
* VS1053 for use with SdFat by Michael P. Flaga

### Mega
* TODO...

### Nano
* TODO...

### iOS APP
TODO...

### Android APP
TODO...

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
### 2022/07/12
* library dependency link correction
### 2022/07/29
* https://github.com/dniklaus/arduino-display-lcdkeypad testing
* https://github.com/fmalpartida/New-LiquidCrystal testing

## TODO
* skippable instruction audios
* domination game mode implememtation
* destruction/domination game mode integrated implementation
* change timer beep pattern when time is running out
* STOP-CMD function implementation for I2C mp3 playback
* code optimization, add comment, indent correction
* ~exchangeable Photoresistor/PING sensor API (deprecated)~
* ~add ENABLE_SLAVE_BOARD mode flag for master board used alone (deprecated)~
* magic number elimination
* getting started user guide & demo box installation tutorial videos
* 11.1v lipo for two Arduino Uno board powerining issue
* single Arduino Mega board ver demobox implementation, w/ DFPlayer mp3 codec and internal speaker
* single Arduino Nano/Uno board ver demobox implementation, w/o audio playback, Photoresistor, Potentiometer, RFID sensor, IR remote control, RGB light module
* iOS/Android APP implementation
* add Amazon link for component price reference

## Community
* TODO...

## Contributing
* We use GitHub issues to tracking issue requests and bugfix. If you encounter any bug or have any ideas, please create new issues on this repository
