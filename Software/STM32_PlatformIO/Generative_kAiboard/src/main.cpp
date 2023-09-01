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

void lockdevice();
void wakeDevice();
void sleepDevice();

unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

#define STATE_BASIC_KEYBOARD  0
#define STATE_CHAT_GPT_QUERY  1
#define STATE_CHAT_GPT_STREAM 2
#define STATE_DEVICE_LOCKED   99
#define STATE_DEVICE_SLEEP    88

byte STATE_TRACKER = 0;

void setup() {

  Serial.begin(115200); 
  IWatchdog.begin(30000000);
  checkWatchDog();
  initLCDSerial();
  initBLE();
  initEEPROM();
  initGPIO();
  initEthernetDHCP(2000);

  delay(250);

  //safeCurrentState(0);
  clickCounter=readKeyCountEEP();
  STATE_TRACKER=readCurrentState();

  setTypeCounterBrightness(40);
  displayTypeCounter(clickCounter);

}

void loop() {

  IWatchdog.reload();

  int inputSpecialKeys=scanSpecialKeys();
  char input=scanKeyboard();

  switch(STATE_TRACKER){

    case STATE_BASIC_KEYBOARD:
          if(input!=NULL){
            sendChar(input);
            delay(100);
          }

          if(input==' ' && inputSpecialKeys==0){
            STATE_TRACKER=STATE_CHAT_GPT_QUERY;
            safeCurrentState(STATE_TRACKER);
            changeLightMode(LED_MODE_SIDERAIN);
            stopCurrentVideo();
            sendTextLCD(STATUS_BAR,"Chat GPT MODE");
            sendTextLCD(INPUT_KBD,"ask me anything...");
            buzzMotor(2,250);
          }

          break;

    case STATE_CHAT_GPT_QUERY:
        if(input!=NULL){
          if(input==8){
            delTextLCD(INPUT_KBD,1);
            delay(100);
          }else if(input==27){ // Escape key
            playVideo(VID_DISENGAGE,100);
            STATE_TRACKER=2;
            safeCurrentState(STATE_TRACKER);
            buzzMotor(2,250);
            sendTextLCD(STATUS_BAR, "Disengage the gpt interlock");
          }else if(input==13){
            buzzMotor(2,250);
            openAI_chat(getTextLCD(INPUT_KBD,0));
          }else{          
            appendTextLCD(INPUT_KBD,String(input));
            delay(100);
          }
        }

        if(inputSpecialKeys<6){
          switch(inputSpecialKeys){
          case 5: //playVideo(VID_TO_STREAM,100);
                  //delay(10000);
                  //playVideo(VID_CURSOR,100);
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

        if(input==' ' && inputSpecialKeys==0){
            playVideo(VID_INTRO,0);
            STATE_TRACKER=STATE_BASIC_KEYBOARD;
            safeCurrentState(STATE_TRACKER);
            sendTextLCD(STATUS_BAR,"keyboard mode");
            changeLightMode(LED_MODE_PLASMA);
            clearTextLCD(INPUT_KBD);
            clearTextLCD(OUTPUT_GPT);
            buzzMotor(1,500);
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

  if(millis()-timerSaveKeycount>intervalSaveKeycount){
    if(safeKeycountEEP(clickCounter)){
      Serial.println("Keycount Saved");
    }else{
      Serial.println("Saving Keycount FAILED");
    }
    timerSaveKeycount=millis();
  }

}

void lockdevice(){
  playVideo(VID_LOCK,100);
  STATE_TRACKER=STATE_DEVICE_LOCKED;
  safeCurrentState(STATE_TRACKER);
  changeLightMode(LED_MODE_PLASMA);
  sendTextLCD(STATUS_BAR,"device locked");
}

void sleepDevice(){
  STATE_TRACKER=STATE_DEVICE_SLEEP;
  safeCurrentState(STATE_TRACKER);
  analogWrite(PORT_LOGO_PWM_R,255);
  analogWrite(PORT_7S_ENABLE,255);

  gotoPage("main_2");
  delay(500);
  playVideo(VID_GOTO_SLEEP,100);
  delay(5500);
  sleepLCD();
  sleepModeLight(1);
}

void wakeDevice(){
  wakeLCD();
  delay(1000);
  sleepModeLight(0);
  playVideo(VID_GOTO_WAKE,100);
  analogWrite(PORT_LOGO_PWM_R,200);
  analogWrite(PORT_7S_ENABLE,40);
  STATE_TRACKER=STATE_BASIC_KEYBOARD;
  safeCurrentState(STATE_TRACKER);
  sendTextLCD(STATUS_BAR,"keyboard mode");
  changeLightMode(LED_MODE_PLASMA);
  clearTextLCD(INPUT_KBD);
  clearTextLCD(OUTPUT_GPT);
  buzzMotor(1,500);
}

