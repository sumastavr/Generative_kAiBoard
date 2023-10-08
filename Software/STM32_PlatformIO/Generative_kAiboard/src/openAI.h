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
#include "openAIKey.h"

const char server[] = "api.openai.com";    // name address for Arduino (using DNS)

//ChatGPT
String role = "You are a helpful assistant.";
String model = "gpt-3.5-turbo";
String system_content = "{\"role\": \"system\", \"content\":\""+ role +"\"}";
String historical_messages = system_content;

String openAI_chat(String message) { 

  message.replace("\"","'");
  String user_content = "{\"role\": \"user\", \"content\":\""+ message+"\"}";
  historical_messages += ", "+user_content;
  String temperature = "\"temperature\":"+ String(0.8);
  String maxToken = "\"max_tokens\":"+ String(400);
  String frequency = "\"frequency_penalty\":"+ String(0.5);
  String user = "\"user\":\"Sumasta Generative Kaiboard\"";

  String request = "{\"model\":\""+model+"\",\"messages\":[" + historical_messages + "]," + temperature +","+ maxToken + ","+ frequency + ","+ user + "}";

  Serial.println(request);
  Serial.println(client.connected());

  if (client.connect(server, 443)) {
    
    IWatchdog.reload();
    sendTextLCD(STATUS_BAR,"GENERATING ANSWERS");
    clearTextLCD(OUTPUT_GPT);

    client.println("POST /v1/chat/completions HTTP/1.1");
    client.println("Connection: close"); 
    client.println("Host: api.openai.com");
    client.println("Authorization: Bearer " + openaiKey);
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();

    for (int i = 0; i < request.length(); i += 1024) {
      client.print(request.substring(i, i+1024));
    }
    
    String getResponse="";
    String Feedback="";
    bool state = false;
    int waitTime = 30000;   // timeout 30 seconds
    long startTime = millis();
    bool replied=false;

    showObjectLCD(GPT_BAR_PARA);
    byte progress=0;

    while ((startTime + waitTime) > millis() && !replied) {

      Serial.print("Generating: ");
      Serial.println(millis()-startTime);
      progress++;
      setProgressBar(GPT_BAR,progress);
      delay(300);
    
      while (client.available()>0 && !replied) {

          char c = client.read();
          Serial.print(c);
          
          if (state==true){ 
            getResponse += c;
          }

          if (c == '\n'){
            Feedback = "";
          }else if (c != '\r'){
            Feedback += c;
          }

          if (Feedback.indexOf("\",\"content\":\"")!=-1||Feedback.indexOf("\"content\": \"")!=-1){
            state=true;
          }else if (Feedback.indexOf("Bad Request")!=-1){
            sendTextLCD(STATUS_BAR,"Bad Request Please retry");
            client.stop();
            replied=true;
          }
            
          if (getResponse.indexOf("\"},")!=-1 && state==true) {
            state=false;
            getResponse = getResponse.substring(0,getResponse.length()-3);
            
            String spacer="";
            spacer+=(char)0x5C;
            spacer+=(char)0x6E;
            getResponse.replace(spacer," ");

            clearTextLCD(STATUS_BAR);
            sendTextLCD(OUTPUT_GPT,getResponse);
            IWatchdog.begin(10000);

            client.flush();
            client.stop();

          } else if (getResponse.indexOf("\"")!=-1 && c == '\n' && state==true) {
            state=false;
            getResponse = getResponse.substring(0,getResponse.length()-2);

            // search /n in the reply and replace it to /r for display
            String GPTspacer="";
            GPTspacer+=(char)0x5C;
            GPTspacer+=(char)0x6E;
            String NXTspacer="";
            NXTspacer+=(char)0x0D;
            NXTspacer+=(char)0x0A;
            String toDisplay=getResponse;
            toDisplay.replace(GPTspacer,NXTspacer);

            sendTextLCD(STATUS_BAR,"press ESCape to stream");
            sendTextLCD(OUTPUT_GPT,toDisplay);
            hideObjectLCD(GPT_BAR_PARA);
            buzzMotor(1,1000);
            replied=true;
            break;
          }    
      }

      if (getResponse.length()>0){
        client.stop();
        String assistant_content = "{\"role\": \"assistant\", \"content\":\""+ getResponse+"\"}";
        historical_messages += ", "+assistant_content;
        //Serial.println("reply: ");
        //Serial.println(getResponse);
        return getResponse;
      }

    }

    Serial.println(Feedback);
    return "error";

  }else{
    
    sendTextLCD(STATUS_BAR,"Can't connect to openai");
    return "Connection failed";  
  }
}

String openAI_chat_Stream(String message) { 

  message.replace("\"","'");
  String user_content = "{\"role\": \"user\", \"content\":\""+ message+"\"}";
  historical_messages += ", "+user_content;
  String temperature = "\"temperature\":"+ String(0.8);
  String maxToken = "\"max_tokens\":"+ String(800);
  String frequency = "\"frequency_penalty\":"+ String(0.5);
  String user = "\"user\":\"Sumasta Generative Kaiboard\"";
  String stream="\"stream\": true";

  String request = "{\"model\":\""+model+"\",\"messages\":[" + historical_messages + "]," + temperature +","+ maxToken + ","+ frequency + ","+ user + "," +stream + "}";

  if (client.connect(server, 443)) {
    
    IWatchdog.reload();
    sendTextLCD(STATUS_BAR,"GENERATING ANSWERS");
    clearTextLCD(OUTPUT_GPT);
    changeAlignmentText(OUTPUT_GPT,0);
    changeLightMode(LED_MODE_COLORFILL);

    client.println("POST /v1/chat/completions HTTP/1.1");
    client.println("Connection: close"); 
    client.println("Host: api.openai.com");
    client.println("Authorization: Bearer " + openaiKey);
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();

    for (int i = 0; i < request.length(); i += 1024) {
      client.print(request.substring(i, i+1024));
    }
    
    String getResponse="";
    String Feedback="";
    bool state = false;
    int waitTime = 30000;   // timeout 30 seconds
    long startTime = millis();
    bool replied=false;

    showObjectLCD(GPT_BAR_PARA);
    byte progress=0;
    long counterProgress=millis();

    int chunk=0;
    String completeAppendedReply="";

    stopCurrentVideo();
    hideObjectLCD(VID_OUTRO);

    while ((startTime + waitTime) > millis() && !replied) {

      if(millis()-counterProgress>300){
        setProgressBar(GPT_BAR,++progress);
        counterProgress=millis();
      }
    
      while (client.available()>0 && !replied) {

          if(millis()-counterProgress>300){
            setProgressBar(GPT_BAR,++progress);
            counterProgress=millis();
          }

          char c = client.read();
          //Serial.print(c);
          
          if (state==true){ 
            getResponse += c;
          }

          if (c == '\n'){
            Feedback = "";
          }else if (c != '\r'){
            Feedback += c;
          }

          if (Feedback.indexOf("\",\"content\":\"")!=-1 || Feedback.indexOf("\"content\": \"" )!=-1 || Feedback.indexOf("{\"content\":\"" )!=-1 || Feedback.indexOf("{\"content\": \"" )!=-1){
            state=true;
          }else if (Feedback.indexOf("Bad Request")!=-1){
            sendTextLCD(STATUS_BAR,"Bad Request Please retry");
            client.stop();
            changeLightMode(LED_MODE_SIDERAIN);
            replied=true;
            historical_messages="";
          }else if(Feedback.indexOf("[DONE]")!=-1){
            sendTextLCD(STATUS_BAR,"press ESCape to stream");
            hideObjectLCD(GPT_BAR_PARA);
            replied=true;
            client.stop();
            String assistant_content = "{\"role\": \"assistant\", \"content\":\""+ completeAppendedReply+"\"}";
            historical_messages += ", "+assistant_content;
            Serial.print("Hist Message Length: ");
            Serial.println(historical_messages.length());
            Serial.print("Free Ram: ");
            Serial.println(getFreeRamString());
            String systemState="FREE-RAM:"+getFreeRamString()+" b";
            systemState+="  GPTBUFFER:";
            systemState+=String(historical_messages.length())+" b";
            systemState+="  UPTIME:"+String(millis())+" ms";
            sendTextLCD(SYSTEM_STATE,systemState);
            buzzMotor(2,250);
            changeLightMode(LED_MODE_SIDERAIN);
            return completeAppendedReply;
            break;
          }
            
          if (getResponse.indexOf("\"},")!=-1 && state==true) {

            chunk++;
            state=false;
            getResponse = getResponse.substring(0,getResponse.length()-3);

            /*
            Serial.print("Chunk: ");
            Serial.print(chunk);
            Serial.print(" ");
            Serial.println(getResponse);
            */

            String GPTspacer="";
            GPTspacer+=(char)0x5C;
            GPTspacer+=(char)0x6E;
            String NXTspacer="";
            NXTspacer+=(char)0x0D;
            NXTspacer+=(char)0x0A;
            String toDisplay=getResponse;
            toDisplay.replace(GPTspacer,NXTspacer);

            appendTextLCD(OUTPUT_GPT,toDisplay);
            completeAppendedReply+=getResponse;
            getResponse="";
            Feedback="";

          } 
      }
    }

    Serial.println(Feedback);
    return "error";

  }else{
    
    sendTextLCD(STATUS_BAR,"Can't connect to openai");
    return "Connection failed";  
  }
}

