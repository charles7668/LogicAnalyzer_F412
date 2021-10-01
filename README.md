# LogicAnalyzer_F412
simple logic analyzer with Nucleo-F412ZG

## System Requirements
1. STM32CubeMX (this project use version 6.3.0)
2. STM32CubeIDE or other IDE can build cmake (this project use CLion)

## GPIO setting
1. channel 1 -> PD14 (nucleo D10)
2. channel 2 -> PD15 (nucleo D9)
3. channel 3 -> PC11 (nucleo D46)
4. channel 4 -> PC12 (nucleo D47)
5. channel 5 -> PA2  (nucleo A8)
6. channel 6 -> PA3  (nucleo A0)
7. channel 7 -> PA4  (nucleo D24)
8. channel 8 -> PA5  (nucleo D13)

## How to use
use serial port tool connect to ST-Link virtual comport   
port setting : (115200 ,N ,8 ,1) 

## command
command start with ascii "start" string and two 4 bytes data  
total 5 + 4 + 4 = 13 bytes

the first 4 bytes data is interval with unit micro seconds  

the second 4 bytes data is number of sample (max is 200k) 

data order is reverse per byte  
ex : 1000 =  03E8 , send data will be  E8 03 00 00