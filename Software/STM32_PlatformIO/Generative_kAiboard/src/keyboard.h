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
#include "GPIO.h"
#include "keyboard_map.h"

const uint8_t MUX_CTRL_PORTS[] = {PORT_MUX_CTRL_S0, PORT_MUX_CTRL_S1, PORT_MUX_CTRL_S2, PORT_MUX_CTRL_S3};
const uint8_t MUX_READ_PINS[] = {PIN_KBD_L_MUX_1,PIN_KBD_L_MUX_2,PIN_KBD_R_MUX_1,PIN_KBD_R_MUX_2};

boolean keyStates[16][4];
boolean prevKeyStates[16][4];
boolean specialKeys[6];
boolean prevSpecialKeys[6];

const uint8_t scanIteration=16;

uint32_t clickCounter=0;
long timerSaveKeycount=millis();
long intervalSaveKeycount=1800000;

bool currentKeyState=false;
bool currentSpKeyState=false;

void initVariables(){
  for (int i=0;i<16;i++){
    for (int x=0;x<4;x++){
      keyStates[i][x]=1;
      prevKeyStates[i][x]=1;
    }
  }
  for (int i=0;i<6;i++){
    specialKeys[i]=1;
    prevSpecialKeys[i]=1;
  }
}

void setTypeCounterBrightness(int value){
    analogWrite(PORT_7S_ENABLE,value);
}

void displayTypeCounter(uint32_t clickCount){
  
  int numberArray[10];

  digitalWrite(PORT_7S_STCP,LOW);
  for (int i=0;i<10;i++){
    numberArray[i]=clickCount%10; 
    clickCount=clickCount/10;
    shiftOut(PORT_7S_DS, PORT_7S_SHCP, MSBFIRST, digits_7s[numberArray[i]]);
  }
  digitalWrite(PORT_7S_STCP,HIGH);

}

char scanKeyboard(){

  // Scan through all possible MUX combination from S0 to S3
  for (uint8_t i=0;i<scanIteration;i++){

    // Write each of the MUX control based on the bit inputs
    for (uint8_t x=0; x<sizeof(MUX_CTRL_PORTS); x++){
      digitalWrite(MUX_CTRL_PORTS[x],bitRead(i,x));
    }

    for (uint8_t z=0;z<sizeof(MUX_READ_PINS);z++){

      bool pinState=digitalRead(MUX_READ_PINS[z]);
      keyStates[i][z]=pinState;
      currentKeyState=false;

        if(pinState!=prevKeyStates[i][z]){
          prevKeyStates[i][z]=keyStates[i][z];

          if(pinState){
            Serial.print("R: ");
            Serial.println(caseMux[i][z]);
            currentKeyState=false;
            prevKeyStates[i][z]=keyStates[i][z];
            return caseMux[i][z];
          }else{

            //Serial.print(" STATE: ");
            //Serial.print(i);
            //Serial.print(" MUX: ");
            //Serial.println(z);
            //delay(200);

            Serial.print("P: ");
            Serial.println(caseMux[i][z]);
            currentKeyState=true;
            prevKeyStates[i][z]=keyStates[i][z];
            displayTypeCounter(++clickCounter);
            return caseMux[i][z];
          }      
        }

      prevKeyStates[i][z]=keyStates[i][z];
    }
  }
  return NULL;
}

int scanSpecialKeys(){

  specialKeys[0]=digitalRead(PIN_KBD_L_SPK_1);
  specialKeys[1]=digitalRead(PIN_KBD_L_SPK_2);
  specialKeys[2]=digitalRead(PIN_KBD_L_SPK_3);
  specialKeys[3]=digitalRead(PIN_KBD_R_SPK_1);
  specialKeys[4]=digitalRead(PIN_KBD_R_SPK_2);
  specialKeys[5]=digitalRead(PIN_KBD_R_SPK_3);

  currentSpKeyState=false;

  for (int i=0;i<6;i++){
    if (specialKeys[i]!=prevSpecialKeys[i]){
      if(specialKeys[i]==LOW){
        currentSpKeyState=true;
        displayTypeCounter(++clickCounter);
      }else{
        currentSpKeyState=false;
      }
      prevSpecialKeys[i]=specialKeys[i];
      return i;
    }
  }

  for (int i=0;i<6;i++){
    prevSpecialKeys[i]=specialKeys[i];
  }

  return 99;
}

bool isPrintableKey(char in){
  bool printable=true;
  for (int i=0;i<nonPrintableTotal;i++){
    if (nonPrintable[i]==in){
      printable=false;
      return printable;
    }
  }
  return printable;
}

void printDebugKeyboard(){
    
  Serial.println(digitalRead(MUX_READ_PINS[0]));
  Serial.println(digitalRead(MUX_READ_PINS[1]));
  Serial.println(digitalRead(MUX_READ_PINS[2]));
  Serial.println(digitalRead(MUX_READ_PINS[3]));

  Serial.println(digitalRead(PIN_KBD_L_SPK_1));
  Serial.println(digitalRead(PIN_KBD_L_SPK_2));
  Serial.println(digitalRead(PIN_KBD_L_SPK_3));
  
  Serial.println(digitalRead(PIN_KBD_R_SPK_1));
  Serial.println(digitalRead(PIN_KBD_R_SPK_2));
  Serial.println(digitalRead(PIN_KBD_R_SPK_3));
  
  Serial.println();

}

void initTypeCounter(){
  clickCounter=readKeyCountEEP();
  setTypeCounterBrightness(40);
  displayTypeCounter(clickCounter);
}

