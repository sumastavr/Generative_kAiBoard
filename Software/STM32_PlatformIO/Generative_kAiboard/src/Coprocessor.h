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

HardwareSerial BLE(USART2);

#define LEDCHANGEHEADER     212
#define LEDCHANGEFOOTER     213

#define LEDSLEEPMODEHEADER  215
#define LEDSLEEPMODEFOOTER  216

#define DELAY_STREAM        30
#define DELAY_STREAM_VAR    25

#define LED_MODE_PLASMA      1
#define LED_MODE_THREESINE   2
#define LED_MODE_CONFETTI    3
#define LED_MODE_RIDER       4
#define LED_MODE_GLITTER     5
#define LED_MODE_SLANTBAR    6
#define LED_MODE_COLORFILL   7
#define LED_MODE_SIDERAIN    8

#define STREAMSTARTCODE     221
#define STREAMDONECODE      222

void initBLE(){
    
  BLE.setTx(BLE_TX_PIN);
  BLE.setRx(BLE_RX_PIN);
  BLE.begin(115200);

}

void sendChar(char input){
    BLE.print(input);
}

#define KBDPRESSCODE        201
#define KBDRELEASECODE      202

void pressChar(char input){
    BLE.write(KBDPRESSCODE);
    BLE.print(input);
}

void releaseChar(char input){
    BLE.write(KBDRELEASECODE);
    BLE.print(input);
}

void streamGPTResults(String answer){
    String bufferDisplay=answer;
    BLE.write(STREAMSTARTCODE);
    delay(1000);
    for(int i=0;i<answer.length();i++){
        IWatchdog.reload();
        delTextLCD(OUTPUT_GPT,1);

        //BLE.print(answer.charAt(i));
        Serial.print(answer.charAt(i));
        
        delay(25);
    }
    BLE.write(STREAMDONECODE);
    delay(500);
}

void changeLightMode(byte mode){

    String payload="";
    payload+=(char)LEDCHANGEHEADER;
    payload+=String(mode);
    //payload+=(char)LEDCHANGEFOOTER;

    BLE.print(payload);

}

void sleepModeLight(byte state){
    String payload="";
    payload+=(char)LEDSLEEPMODEHEADER;
    payload+=String(state);
    //payload+=(char)LEDSLEEPMODEFOOTER;
    BLE.print(payload);
}