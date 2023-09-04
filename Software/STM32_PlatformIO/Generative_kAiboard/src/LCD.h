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

HardwareSerial LCD(USART6);

#define INPUT_KBD       "slt0.txt"
#define OUTPUT_GPT      "slt1.txt"
#define GPT_MODEL       "select1.txt"
#define GPT_ROLE        "select0.txt"
#define STATUS_BAR      "t8.txt"
#define INTERNET_STATE  "t2.txt"

#define SL_RANDOM       "h2.val"
#define SL_CREATIVE     "h1.val"
#define SL_TOKEN        "h0.val"

#define VID_INTRO       "v0"
#define VID_CURSOR      "v1"
#define VID_DISENGAGE   "v2"
#define VID_TO_STREAM   "v3"
#define VID_UNLOCKED    "v4"
#define VID_LOCK        "v5"

#define VID_GOTO_SLEEP  "v6"
#define VID_GOTO_WAKE   "v7"
#define VID_OUTRO       "v8"

#define CLOCK_LCD       "t10.txt"
#define CLOCK_PARA      "t10"
#define DATE_LCD        "t11.txt"
#define DATE_PARA       "t11"
#define SEC_BAR_PARA    "j1"
#define SEC_BAR         "j1.val"

#define GPT_BAR_PARA    "j0"
#define GPT_BAR         "j0.val"

//#define DEBUG_LCD

#define TIMEOUT_WAIT_LCD    100

String vidCurrentlyPlaying=VID_INTRO;

void initLCDSerial(){
    LCD.setTx(LCD_TX_PIN);
    LCD.setRx(LCD_RX_PIN);
    LCD.begin(115200);
}

void sendTermination(){
    LCD.write(0xFF);
    LCD.write(0xFF);
    LCD.write(0xFF);
}

void sendPayload(String payload){
    LCD.print(payload);
    #ifdef DEBUG_LCD
        Serial.println(payload);
    #endif
}

void sendTextLCD(String target, String content){
    String payload=target;
    payload+="=";
    payload+=(char)34;
    payload+=content;
    payload+=(char)34;
    sendPayload(payload);
    sendTermination();
    delay(10);
}

void appendTextLCD(String target, String content){
    String payload=target;
    payload+="+=";
    payload+=(char)34;
    payload+=content;
    payload+=(char)34;
    sendPayload(payload);
    sendTermination();
}

void delTextLCD(String target, int delCount){
    String payload=target;
    payload+="-=";
    payload+=delCount;
    sendPayload(payload);
    sendTermination();
}

void clearTextLCD(String target){
    String payload=target;
    payload+="=";
    payload+=(char)34;
    payload+="";
    payload+=(char)34;
    sendPayload(payload);
    sendTermination();
}

String expectLCDStream(int timeout){

    long counter=millis();
    String reply;
    while(timeout+counter>millis()){
        if (LCD.available()){
            reply+=(char)LCD.read();
        }
    }

    // sometimes end termination is received on the lcd buffer somehow, we filter it
    String termination="";
    termination+=(char)26;
    termination+=(char)0xFF;
    termination+=(char)0xFF;
    termination+=(char)0xFF;
    reply.replace(termination," ");

    /*
    Serial.println("DEBUG READ");
    for (int i=0;i<reply.length();i++){
        Serial.write(reply.charAt(i));
        Serial.print(" ");
        Serial.println((byte)reply.charAt(i));
    }
    */

    return reply;
}

int expectLCDStreamInt(int timeout){

    long counter=millis();
    int reply;
    while(timeout+counter>millis()){
        if (LCD.available()){
            reply=LCD.read();
        }
    }
    return reply;
}

String getTextLCD(String target, int charCount){
    String payload="prints ";
    payload+=target;
    payload+=",";
    payload+=String(charCount);
    sendPayload(payload);
    sendTermination();
    return expectLCDStream(TIMEOUT_WAIT_LCD);
}

void makeSelectionLCD(String target,int index){
    target.replace(".txt",".val");
    String payload=target;
    payload+="=";
    payload+=String(index);
    sendPayload(payload);
    sendTermination();
}

int getValueLCD(String target){
    String payload="prints ";
    payload+=target;
    payload+=",1";
    sendPayload(payload);
    sendTermination();
    return expectLCDStreamInt(TIMEOUT_WAIT_LCD);
}

void setLCDVolume(int volume){
    if(volume<=100){
        String payload="volume=";
        payload+=String(volume);
        sendPayload(payload);
        sendTermination();
    }
}

void pauseVideo(String video){
    // play
    String payload=video;
    payload+=".en=2";
    sendPayload(payload);
    sendTermination();
    delay(10);
}

void stopCurrentVideo(){

    // play
    String payload=vidCurrentlyPlaying;
    payload+=".en=0";
    sendPayload(payload);
    sendTermination();
    delay(10);

    // set visible
    payload="vis ";
    payload+=vidCurrentlyPlaying;
    payload+=",0";
    sendPayload(payload);
    sendTermination();
    delay(10);

}

void playVideo(String video,int volume){
    
    setLCDVolume(volume);

    // set invisible currently play if any
    String payload="vis ";
    payload+=vidCurrentlyPlaying;
    payload+=",0";
    sendPayload(payload);
    sendTermination();
    delay(10);

    vidCurrentlyPlaying=video;

    // set visible
    payload="vis ";
    payload+=video;
    payload+=",1";
    sendPayload(payload);
    sendTermination();
    delay(10);

    // play
    payload=video;
    payload+=".en=1";
    sendPayload(payload);
    sendTermination();
    delay(10);

}

void sleepLCD(){
    stopCurrentVideo();

    String payload="dim=0";
    sendPayload(payload);
    sendTermination();
    delay(10);

    payload="sleep=1";
    sendPayload(payload);
    sendTermination();
}

void wakeLCD(){
    String payload="sleep=0";
    sendPayload(payload);
    sendTermination();
    delay(10);

    payload="dim=100";
    sendPayload(payload);
    sendTermination();
    delay(10);

    
}

void gotoPage(String page){
    String payload="page ";
    payload+=page;
    sendPayload(payload);
    sendTermination();
    delay(3);
}

void hideObjectLCD(String name){
    String payload="vis ";
    payload+=name;
    payload+=",0";
    sendPayload(payload);
    sendTermination();
    delay(3);
}

void showObjectLCD(String name){
    String payload="vis ";
    payload+=name;
    payload+=",1";
    sendPayload(payload);
    sendTermination();
    delay(3);
}

void setProgressBar(String name, int state){
    if(state<=100){
        String payload=name;
        payload+="=";
        payload+=String(state);
        sendPayload(payload);
        sendTermination();
        delay(3);
    }
}

  /*
  Serial.print("1: ");
  uint8_t rep=getValueLCD(SL_RANDOM);
  Serial.println(rep);
  Serial.println();

  Serial.print("2: ");
  rep=getValueLCD(SL_CREATIVE);
  Serial.println(rep);
  Serial.println();

  Serial.print("3: ");
  rep=getValueLCD(SL_TOKEN);
  Serial.println(rep);
  Serial.println();
  */

