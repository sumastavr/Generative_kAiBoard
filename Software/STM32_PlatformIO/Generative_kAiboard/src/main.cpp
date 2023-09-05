/*

BSD 3-Clause License

Copyright (c) 2023, Pamungkas Sumasta (www.sumasta.tech)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <Arduino.h>
#include <IWatchdog.h>
#include <Wire.h>
#include <SPI.h>

#include "GPIO.h"
#include "LCD.h"
#include "EEPROM.h"
#include "Coprocessor.h"
#include "utils.h"
#include "NFC.h"
#include "ToF.h"
#include "ethernetSSL.h"
#include "openAI.h"
#include "keyboard.h"
#include "time.h"

#define STATE_BASIC_KEYBOARD  0
#define STATE_CHAT_GPT_QUERY  1
#define STATE_CHAT_GPT_STREAM 2
#define STATE_DEVICE_LOCKED   99
#define STATE_DEVICE_SLEEP    88

byte STATE_TRACKER = 0;

#include "logic.h"

void setup() {

  Serial.begin(115200); 
  IWatchdog.begin(30000000);
  checkWatchDog();
  initLCDSerial();
  initBLE();
  initEEPROM();
  initEthernetDHCP(2000);
  initGPIO();
  initTypeCounter();
  initVariables();
  STATE_TRACKER=readCurrentState();
  Serial.print("Current State: ");
  Serial.println(STATE_TRACKER);
  sendNTPpacket(TIME_SERVER);
}


void loop() {

  IWatchdog.reload();
  monitorKeycount();

  int inputSpecialKeys=scanSpecialKeys();
  char input=scanKeyboard();

  if (inputSpecialKeys==SP_KEY_A){
    lockdevice();
  }else if(inputSpecialKeys==SP_KEY_B){
    sleepDevice();
  }

  switch(STATE_TRACKER){

    case STATE_BASIC_KEYBOARD:

          monitorClock();
          
          if(inputSpecialKeys==SP_KEY_C && currentSpKeyState) toChatGPTMode();
        
          if(input!=NULL){
            if(currentKeyState){
              pressChar(input);
            }else{
              releaseChar(input);
            }
            delay(20);
          }

          switch(inputSpecialKeys){
            case SP_KEY_F:
              if(currentSpKeyState){
                pressChar(CT);
              }else{
                releaseChar(CT);
              }
              break; 
            case SP_KEY_E:
              if(currentSpKeyState){
                pressChar(AL);
              }else{
                releaseChar(AL);
              }
              break;
            case SP_KEY_D:
              if(currentSpKeyState){
                pressChar(SP);
              }else{
                releaseChar(SP);
              }
              break;     
          }

          break;

    case STATE_CHAT_GPT_QUERY:

        if(inputSpecialKeys==SP_KEY_C && currentSpKeyState) toKeyboardMode();

        if(currentKeyState){
          if(input==BS){
            delTextLCD(INPUT_KBD,1);
            delay(100);
          }else if(input==ES){ // Escape key
            playVideo(VID_DISENGAGE,100);
            STATE_TRACKER=2;
            safeCurrentState(STATE_TRACKER);
            buzzMotor(2,250);
            sendTextLCD(STATUS_BAR, "Disengage the gpt interlock");
          }else if(input==EN){
            buzzMotor(2,250);
            openAI_chat(getTextLCD(INPUT_KBD,0));
          }else{
            if(isPrintableKey(input)){      
              if(currentKeyState){    
                appendTextLCD(INPUT_KBD,String(input));
                delay(50);
              }
            }
          }
        }

        if(inputSpecialKeys<6){
          switch(inputSpecialKeys){
          case 5: 
                  gotoPage("main_2");
                  delay(500);
                  changeLightMode(LED_MODE_THREESINE);
                  playVideo(VID_OUTRO,100);
                  break;
          case 3: clearTextLCD(INPUT_KBD);
                  delay(200);
                  break;  
          }
        }

        break;
    
    case STATE_CHAT_GPT_STREAM:

        if(digitalRead(PIN_GPT_STATUS)==LOW){
          changeLightMode(LED_MODE_SLANTBAR);
          buzzMotor(5,100);
          streamGPTResults(getTextLCD(OUTPUT_GPT,0));
          STATE_TRACKER=STATE_CHAT_GPT_QUERY;
          changeLightMode(LED_MODE_SIDERAIN);
          safeCurrentState(STATE_TRACKER);
          sendTextLCD(STATUS_BAR,"results sent");
          buzzMotor(5,100);
        }

        break;

    case STATE_DEVICE_LOCKED:
        monitorClock();
        if (!NFCInitialized){
          initNFC();
        }else{
          if(scanNFC()==true){  
            playVideo(VID_UNLOCKED,100);
            STATE_TRACKER=STATE_BASIC_KEYBOARD;
            safeCurrentState(STATE_TRACKER);
            sendTextLCD(STATUS_BAR,"keyboard mode");
            changeLightMode(LED_MODE_PLASMA);
            clearTextLCD(INPUT_KBD);
            clearTextLCD(OUTPUT_GPT);
            buzzMotor(1,500);
          }
        }

        if(input!=NULL || inputSpecialKeys<6){
          playVideo(VID_LOCK,100);
        }

        break;

    case STATE_DEVICE_SLEEP:

        if(!isToFinitialized){
          initTof();
        }else{
          if(scanToF()==true){
            wakeDevice();
          }
        }

        break;
  }
}



