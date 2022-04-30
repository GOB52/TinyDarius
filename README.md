# TinyDarius

[Japanese/日本語](./README.ja.md)  

Work in progress. Now only stage A.  
It also serves as [dogfooding](https://en.wikipedia.org/wiki/Eating_your_own_dog_food) for my libraries (goblib,goblib_m5s).

<ins>**Due to rights issues, image and sound resource data are not included.**</ins>  
For guidelines on creating your own resources [here](HowToMakeResources.md)

## Overview
Boss rush from arcade game "DARIUS".  
We are not running it on an emulator, but on our own code.  
Therefore, the behavior is different from the real thing.  
I have nothing but respect for the people and company that created "DARIUS".  

## Support hardware
M5Stack Basic,Gray  

### Require
M5Stack Faces + GB Face

## How to build

You can build on [ArduinoIDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) with [Visual Studio Code](https://code.visualstudio.com/).  
Please refer to the respective Webpage for instructions on how to build each environment.  

### Require
[M5Stack](https://github.com/m5stack/M5Stack) 0.4.0  
[LovyanGFX](https://github.com/lovyan03/LovyanGFX) 0.4.17 (support v0,v1)  
[SdFat](https://github.com/greiman/SdFat) 2.1.2  
[goblib](https://github.com/GOB52/goblib) 0.1.0  
[goblib_m5s](https://github.com/GOB52/goblib_m5s) 0.1.0

### platform.ini for PlatformIO

#### Settings

|Item | Settings|
|:---|:---|
|platform | espressif32@3.5.0 |
|board | m5stack-core-esp32 or m5stack-gray|

#### Build env

|env|description|remarks|
|:---|:---|:---|
|master|Master build with LovyanGFX v0| No debugging features|
|master\_v1|Master build with LovyanGFX v1| No debugging features|
|release|Release build with LovyanGFX v0| Debugging features available|
|release\_v1|Release build with LovyanGFX v1| Debugging features available|
|debug|Debug build with LovyanGFX v0|Debugging features available|
|debug|Debug build with LovyanGFX v1|Debugging features available|

Recommend building with master or master\_v1  
I shared libraries between ArduinoIDE and PlatformIO, so use [lib\_extra\_dirs](https://docs.platformio.org/en/latest/projectconf/section_env_library.html#lib-extra-dirs) and installed libraries (exclude M5Stack) by ArduinoIDE.  
Please edit platform.ini according to your environment.


### TinyDarius.ino for ArduinoIDE

TinyDarius.ino is empty file. setup() and loop() in ./src/main.cpp.  
The build will be equivalent to release in platform.ini.  
If you want to change building env,then you need edit platform.txt   
Please refer to the env description to set the options.


#### Settings

BoardManager **M5Stack version 1.0.9**  

[Menu] - [Tool]  

|Menu|Settings|
|:---|:---|
|Board|M5Stack-Core-ESP32|
|Flash Frequency|80|
|Flash Mode|QIO|
|Partation Scheme|Default|
|Core Debug|any |

## How to play

|Button|Description|
|:---|:---|
|Select|Insert coin|
|Start|Start game if credits exists|
|Cross|Moving my ship|
|A| Shot(Software rapid shot)|

1. Please inert coin.(Push select)
1. Push start and start game.
1. Can't move until enemies appear.(Please wait)
1. Defeat the boss in time. (The boss will escape when the time is up)
1. Choose next stage by ship position.(Be careful for obstacles)
1. Game clear when you defeat all boss.


## Scheduled for implementation
(Maybe if I have time I will make an effort to do so to the extent possible)  

- Add more boss.
- Add time bonus on round clear.
- Add more SFX.
- Save/load  score ranking.
- Add missile and power up for ship... hmm, May or may not be implemented.


## Special thanks

**[@KojiSaito](https://twitter.com/kojisaito)** Influenced by his programming for M5Stack, I also bought M5Stack. Thanks to he has shown me the path to M5Stack development.  
**[@Lovyan03](https://github.com/lovyan03)** He is making many cool and useful libraries. Thanks to LovaynGFX I was able to create a game for M5Stack.  

Thanks also to the other people who gave me all kinds of advice during the development.  

Have a happy coding :)

