/*

BSD 3-Clause License

Copyright (c) 2023, Pamungkas Sumasta (www.sumasta.tech)
malloc info Creation by Frederic Pillon

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
#include <malloc.h>

extern "C" char *sbrk(int i);
/* Use linker definition */
extern char _end;
extern char _sdata;
extern char _estack;
extern char _Min_Stack_Size;

static char *ramstart = &_sdata;
static char *ramend = &_estack;
static char *minSP = (char*)(ramend - &_Min_Stack_Size);

void buzzMotor(int repeat,int duration){
    for (int i=0;i<repeat;i++){
        digitalWrite(PORT_VIB_MOT_L,HIGH);
        digitalWrite(PORT_VIB_MOT_R,HIGH);
        delay(duration/2);
        digitalWrite(PORT_VIB_MOT_L,LOW);
        digitalWrite(PORT_VIB_MOT_R,LOW);
        delay(duration/2);
    }
}

void softwareResetNow(){
  NVIC_SystemReset();
}

bool isPOEATDetected(){
  if(digitalRead(PIN_POE_AT_DET)==LOW){
    return true;
  }else{
    return false;
  }
}

void initGPIO(){
  pinMode(PIN_POE_AT_DET,INPUT_PULLUP);

  pinMode(PORT_WS_LED_DATA,OUTPUT);
  pinMode(PORT_WS_LED_ENABLE,OUTPUT);

  pinMode(PORT_7S_DS,OUTPUT);
  pinMode(PORT_7S_SHCP,OUTPUT);
  pinMode(PORT_7S_STCP,OUTPUT);
  pinMode(PORT_7S_ENABLE,OUTPUT);

  pinMode(PORT_MUX_CTRL_S0,OUTPUT);
  pinMode(PORT_MUX_CTRL_S1,OUTPUT);
  pinMode(PORT_MUX_CTRL_S2,OUTPUT);
  pinMode(PORT_MUX_CTRL_S3,OUTPUT);

  pinMode(PIN_KBD_L_MUX_1,INPUT_PULLUP);
  pinMode(PIN_KBD_L_MUX_2,INPUT_PULLUP);
  pinMode(PIN_KBD_R_MUX_1,INPUT_PULLUP);
  pinMode(PIN_KBD_R_MUX_2,INPUT_PULLUP);

  pinMode(PIN_KBD_L_SPK_1,INPUT_PULLUP);
  pinMode(PIN_KBD_L_SPK_2,INPUT_PULLUP);
  pinMode(PIN_KBD_L_SPK_3,INPUT_PULLUP);

  pinMode(PIN_KBD_R_SPK_1,INPUT_PULLUP);
  pinMode(PIN_KBD_R_SPK_2,INPUT_PULLUP);
  pinMode(PIN_KBD_R_SPK_3,INPUT_PULLUP);

  pinMode(PORT_VIB_MOT_L,OUTPUT);
  pinMode(PORT_VIB_MOT_R,OUTPUT);

  pinMode(PORT_LOGO_PWM_R,OUTPUT);
  pinMode(PORT_LOGO_PWM_G,INPUT_FLOATING); // not used for now
  pinMode(PORT_LOGO_PWM_B,INPUT_FLOATING); // not used for now
  
  pinMode(PIN_GPT_STATUS,INPUT_PULLUP);

  digitalWrite(PORT_WS_LED_ENABLE,HIGH);
  digitalWrite(PORT_WS_LED_DATA,HIGH);
  analogWriteFrequency(1000);
  analogWrite(PORT_LOGO_PWM_R,200);
}

void checkWatchDog(){
  if (IWatchdog.isReset(true)) Serial.println("Watchdog Triggered!");
}

void display_mallinfo(void){

  char *heapend = (char*)sbrk(0);
  char * stack_ptr = (char*)__get_MSP();
  struct mallinfo mi = mallinfo();

  Serial.print("Total non-mmapped bytes (arena):       ");
  Serial.println(mi.arena);
  Serial.print("# of free chunks (ordblks):            ");
  Serial.println(mi.ordblks);
  Serial.print("# of free fastbin blocks (smblks):     ");
  Serial.println(mi.smblks);
  Serial.print("# of mapped regions (hblks):           ");
  Serial.println(mi.hblks);
  Serial.print("Bytes in mapped regions (hblkhd):      ");
  Serial.println(mi.hblkhd);
  Serial.print("Max. total allocated space (usmblks):  ");
  Serial.println(mi.usmblks);
  Serial.print("Free bytes held in fastbins (fsmblks): ");
  Serial.println(mi.fsmblks);
  Serial.print("Total allocated space (uordblks):      ");
  Serial.println(mi.uordblks);
  Serial.print("Total free space (fordblks):           ");
  Serial.println(mi.fordblks);
  Serial.print("Topmost releasable block (keepcost):   ");
  Serial.println(mi.keepcost);

  Serial.print("RAM Start at:       0x");
  Serial.println((unsigned long)ramstart, HEX);
  Serial.print("Data/Bss end at:    0x");
  Serial.println((unsigned long)&_end, HEX);
  Serial.print("Heap end at:        0x");
  Serial.println((unsigned long)heapend, HEX);
  Serial.print("Stack Ptr end at:   0x");
  Serial.println((unsigned long)stack_ptr, HEX);
  Serial.print("RAM End at:         0x");
  Serial.println((unsigned long)ramend, HEX);

  Serial.print("Heap RAM Used:      ");
  Serial.println(mi.uordblks);
  Serial.print("Program RAM Used:   ");
  Serial.println(&_end - ramstart);
  Serial.print("Stack RAM Used:     ");
  Serial.println(ramend - stack_ptr);
  Serial.print("Estimated Free RAM: ");
  Serial.println(((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks);
}

String getFreeRamString(){
  char *heapend = (char*)sbrk(0);
  char * stack_ptr = (char*)__get_MSP();
  struct mallinfo mi = mallinfo();
  return String((((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks));
}

