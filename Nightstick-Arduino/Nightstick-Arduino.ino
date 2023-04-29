/*
  IDE used Arduino 1.8.19
  Board: Seeed Studio XIAO nRF52840 BLE Sense
  Core: Seeed Studio nRF52 Boards v1.1.1 (Not the mbed -enabled core!)

  Board specs:
  Procesor: Nordic nRF52840 - ARM® Cortex®-M4 with FPU run up to 64 MHz
  Wireless:     Bluetooth 5.0/NFC/Zigbee
  Flash:  1 MB flash and 256 kB RAM
  Ext. flash  2 MB QSPI flash
  Board RAM size 256 kB
  Interface / Pins     1xUART, 1xIIC, 1xSPI, 1xNFC, 1xSWD / (11xGPIO(PWM), 6xADC)
  Circuit operating voltage: 3.3V@200mA
  Charging current: 50mA/100mA (internal) / 1A external
  Input voltage (VIN): 5V
  Standby power consumption:  <5μA


  
  Button voltage divider level is max 3v3 regulated (not battery voltage) 
  
  Can't programm the XIAO nRf52?

  when XIAO nrf is stuck and can't be programmed - double tap reset btn while powered via USB-C
  xiao nrf should appear as flash storage in windows when sucessfull

  Dependencies:
  - FastLed (Version3.5 ) only used for the color, math & timing functions because hardware otuput isnt possible. The nRF52 architecture is not fully supported)
        https://github.com/FastLED/FastLED/
        
  - Adafruit NeoPixel (Version 1.11.0) needed for Hardware LED data output
        https://github.com/adafruit/Adafruit_NeoPixel
        
  - Bluefruit (Version 0.21.0 ) Adafruit Bluefruit nRF52 Libraries - Bluetooth Low Energy library
      already included in XIAO nRF52 core librarys  ( https://github.com/adafruit/Adafruit_nRF52_Arduino )
      
  - SdFat - Adafruit Fork (Version 2.2.1)
        https://github.com/adafruit/SdFat
        
  - Arduino_LSM6DS3 (Inertial measurement unit library Arduino_LSM6DS3 v1.0.2)
        https://www.arduino.cc/reference/en/libraries/arduino_lsm6ds3/

  - MahonyAHRS (Version 1.1)  implementation of MahonyAHRS algorithm to smooth and improve the gyro and accleration readings
    https://github.com/PaulStoffregen/MahonyAHRS
  
  - I2Cdevlib-MPU6050 (Version 1.0.0) We only need the helper_3dmath.h file for easy vector handling
      https://github.com/jrowberg/i2cdevlib
      
## Credits

This code is written by Marlon Graeber aka "Loaded Dice" 

## License

This Nightstick Arduino Code is free software: you can redistribute it and/or  modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
The Nightstick code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.en.html) for more details.
You should have received a copy of the GNU Lesser General Public License along with this code.  If not, see [this](https://www.gnu.org/licenses/)


To do:

  New Featues to implement:
    
    - PDM Microphone --> soundlevel detection or FFT + overlay animations
    - make compatible android app and / or lilygo smartwatch code to communicate via BLE with the stick, change settings, lookup battery status....
  
  Improvements & next steps:
  
    - complete input detection & hander routine
        - long button tap (button 1 or 3) -> change folder
        - short button tap  (button 1 or 3) -> change bitmap
        - short button tap (center button) cycle through  battery state display mode / tilt stick - brightness control mode 
        - long button tap  (center button) activate bluetooth (play stick animation) (very long tap = BT/BLE pairing mode)
    - errors should result in error codes and corresponding messages --> char* getErrMsg(error_code) & internal led blink codes (system tab)
      also add boolean to choose if the error should blink forever (Stop the script) or is one time
      (error messages can get stored in a progmem char array later)
      also create an MAINPATH/System/error_log.txt file with  millis() timestamp and error message
      also on bootup search for the error_log.txt and print to serial via msg() and msgln()
    - config struct should control controls main stick states (switch variables like  BRIGHTNESS to cfg.bright and so on)
    - on startup read all folders and bitmap files in MAINPATH/BMPs/ and for each folder create a text file here: MAINPATH/System/BMPs/folder_name.txt 
      each text doument contains a list of all bitmap files that are loadable (right size). so the document serves like a lookup table what file/folder is next/pervious (sdFat fgets())
      create/clear MAINPATH/System/BMPs folder on startup
    - Store contents of the Readme.txt file in progmem and write it to sd card if the file is not present or file size is different
    - when SD card is empty create folder structre and Files: MAINPATH, MAINPATH/BMPs, MAINPATH/System, MAINPATH/readme.txt, MAINPATH/config.csv (config done!) ...
    - implement the use of colorpalettes from FastLed
    - implement battery voltage read function & check values with laboratory power supply
    - implement battery low animation 
    - in Led2Pixel() uint16_t ledPixelPos[NUM_LEDS][n]; where n=0 --> bmp x pos n=1 --> bmp y pos for each led
      currently ledPixelPos is rounded to thethe next whole pixel - instead of rounding directly multiply x&y floats by 10 an then rounding the value
      use the extra precision to implement a blending function toblend with the surrounding pixels (smoother transition)
    - Don't output data to stick like a simple line of LEDs. Introduce a virtual_NUM_LEDS (use the 4 led strips in the flower head as one)--> Look at LEDs mirrorStick()  
    - load and test the trail bitmaps ( 1 degree per pixel )
    - when bmp file & folder --> "-" or "" i n config file (no file or folder selected) load 1st .bmp from first folder when bmp mode is activated.
    - implemet deep sleep for µC or idle animation when not played (make this selectable in config )

    - test lowest voltage where leds are still working
      consider using charge / discharge board instead of charge only

  next version main PCB:
  
    - add button resisor array onto the board.
    - add circuit to switch LEDs completly off (no standby current) 
      (maybe other battery charging board? --> charge/discharge with enable function?)
    - pushbutton on off switch circuit
    - add (electrolytic) capacitor to main pcb
    - make a mini push button pcb (curved / buttons pointing down)
    - put a connector terminal on the bottom - like ZH 1.5 mm (4 & 6 pin; pins) (90 degree angled connector)
        (Btn1,Btn2,Btn3,Gnd) (VBat,DO_Led,Gnd) (VBat, DO_Led2, Gnd)(switch1,switch2)
  
  3D case design improvements
    - improved battery holder
 
               -------------------
 ------=======|  NIGHTSTICK CODE  |=======-------
               -------------------
   ---=={ Current Setup for this sketch }==--- 

        Pinout:
                      ______
                  ___| USBC |___
        (A0) D0  |O  |______|  O|  5V
        (A1) D1  |O|----------|O|  GND
        (A2) D2  |O|   XIAO   |O|  3V3
        (A3) D3  |O| nRF52840 |O|  D10 (A10) MOSI
    SDA (A4) D4  |O|   BLE    |O|  D9  (A9)  MISO
    SCL (A5) D5  |O|  SENSE   |O|  D8  (A8)  SCK
    Tx  (A6) D6  |O|__________|O|  D7  (A7)  Rx
                  --------------

       
      |------|------|------|--------------|
      | ⁪Pin  | GPIO | Pin  | Function     |
      |------|------|------|--------------|
      | D0   |  2   ⁪| A0   | LED Data  ⁪   |
      | D1   ⁪|  3   ⁪| A1   | SD Card CS   |
      | D2   ⁪|  4   ⁪| A2   | Buttons In   |
      | D3   ⁪|  5   ⁪| A3   |              |
      | D4   ⁪|  6   ⁪| SDA  |              |
      | D5   ⁪|  7   ⁪| SCL  |              |
      | D6   ⁪| 21   ⁪| Tx   |              |
      | --   ⁪| --   ⁪| ---- | ------------ |
      | D10  ⁪| 10   ⁪| MOSI | SD Card      |
      | D9   ⁪|  9   ⁪| MISO | SD Card      |
      | D8   ⁪|  8   ⁪| SCK  | SD Card      |
      | D7   ⁪| 20   ⁪| Rx   |              |
      |------|------|------|--------------|

                  
   Main thoughts on the code structure:
   
     includes, definitions, Global variables & structs  are in the header file "Nightstick.h"
     Written in that order for each tab page section separated
     each tab gets its own setup function on the beginning of the page named setup_tabName() 
     and if routines need to run within the loop  i created a main_tabName() function right below the setup_tabName() function
     when a specific timing is needed for the main_ functions, a template function from the fastLED library is used (EVERY_N_MILLIS(n){})
     with the delay time defined in Nightstick.h
     
*/

#include "Nightstick.h"
//--------------------------------------------------------------------------------------------------------------------- Setup
void setup(){ 
  //do not use Serial.print() or Serial.println() instead use msg() or msgln() to implement a debug on/off message mode later
  Serial.begin(115200);

  setup_System(); // begin with system setup to enable error blink codes with the internal board leds
  setup_SD();     // boot sequence stops when SD card is removed (safety reason)
  setup_Config(); // find and load config.csv from SD card or write new if not found
  setup_Inputs();
  setup_Batt();
  setup_FILTER_IMU();
  setup_BLE_COM();
  setup_LEDs();

  memset(pixelBuff,0,sizeof(pixelBuff));  // Filling  the buffer to see at compile time what space is used and whats left - can be removed later
}

void loop(){ // all main functions have timining structures integrated
  main_FILTER_IMU();
  main_LEDs();
  main_BLE_COM();

}
