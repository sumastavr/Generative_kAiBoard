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

#include "time.h"
#include "GPIO.h"
#include "LCD.h"
#include "EEPROM.h"
#include "Coprocessor.h"
#include "utils.h"
#include "NFC.h"
#include "ToF.h"
#include "ethernetSSL.h"
#include "prompt.h"
#include "openAI.h"
#include "keyboard.h"

byte STATE_TRACKER = 0;
const String stateName[] = {"Keyboard Mode", "Generative Mode", "Streaming Mode", "Keyboard is Locked", "Sleep mode"};

#define STATE_BASIC_KEYBOARD  0
#define STATE_CHAT_GPT_QUERY  1
#define STATE_CHAT_GPT_STREAM 2
#define STATE_DEVICE_LOCKED   3
#define STATE_DEVICE_SLEEP    4

#include "logic.h"

void setup() {

  Serial.begin(115200);
  checkWatchDog();
  initWatchDog(30);

  initLCDSerial();
  initBLE();
  initEEPROM();
  initEthernetDHCP(4000);
  initGPIO();
  initTypeCounter();
  initVariables();

  STATE_TRACKER=readCurrentState();
  initStatusBarLCD(stateName[STATE_TRACKER]);
  initInternetClock(TIME_SERVER);

  display_mallinfo();
  
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

          if(inputSpecialKeys==SP_KEY_C && currentSpKeyState) toChatGPTMode();
          processKey(input);
          processSpKeyKBD(inputSpecialKeys);
          monitorClock();
          break;

    case STATE_CHAT_GPT_QUERY:

        if(inputSpecialKeys==SP_KEY_C && currentSpKeyState) toKeyboardMode();
        processSpKeyGPT(inputSpecialKeys);

        if(currentKeyState){
          if(input==BS){            // Backspace Key
            delTextLCD(INPUT_KBD,1);
            delay(100);
          }else if(input==ES){      // Escape key
            STATE_TRACKER=2;
            safeCurrentState(STATE_TRACKER);
            buzzMotor(2,250);
            sendTextLCD(STATUS_BAR, "Disengage the gpt interlock");
          }else if(input==EN){      // Enter Key
            buzzMotor(2,250);
            String processedPrompt=processPromptArray(getTextLCD(INPUT_KBD,0));
            openAI_chat_Stream(processedPrompt);
          }else{
            processKeyGPT(input);
          }
        }

        break;
    
    case STATE_CHAT_GPT_STREAM:

        streamingProcess();
        break;

    case STATE_DEVICE_LOCKED:
        
        monitorClock();
        monitorKBDLock(input,inputSpecialKeys);
        
        if (!NFCInitialized){
          initNFC();
        }else{
          checkNFCTag();
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



