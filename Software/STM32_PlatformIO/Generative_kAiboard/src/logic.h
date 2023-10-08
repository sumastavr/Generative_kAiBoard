
#include <Arduino.h>

const String genModeInfo[]={"::::Informative mode selected::In this mode you can ask me any questions, I'll automatically embed real-time information and your personal user statistic automatically. I'm also keeping the history of the conversation until you reboot::the reply will be streamed directly",
                            "::::Creative mode selected::in this mode you can ask me to generate some paragraphs / contents, i'll imitate your style, your word vibes based on the local historical knowledge and statistics while you are in keyboard mode::the reply will be streamed directly",
                            "::::Suggestive mode selected::in this mode you can use me as a normal keyboard, I'll provide seamless recommendations and tune your sentence as you type along::the reply will be provided after a . (dot and space) is detected",
                            "::::Illustrative mode selected::ask me to generate any sketches, drawings or illustrations, I'll automatically take into account your preferences, real-time statistics and personal flavour::the sketched will be generated and new browser will be opened automatically",
                            "::::Educative mode selected::Tell me a specific domain and I will ask you a quiz consists of 10 questions, add more specific details as you wish::one question will be provided at a time and you can answer with a multiple choice selection",
                            };

const String promptBoxInfo[]={"Ask me anything",
                              "Ask me to generate any contents",
                              "Go ahead and type, I'll tune your sentence",
                              "Ask me to sketch any drawings",
                              "Give me a topic, I will give you an educative quiz"
                              };

bool isGenerativeModeChanged(){
  LCD.setTimeout(300);
  if(LCD.available()>0){
    if(LCD.find("mode")){
      while(LCD.available()) LCD.read();
      delay(1000);
      return true;
    }
  }
  return false;
}

void monitorGenerativeModeSelection(){
  if(isGenerativeModeChanged()){
    STATE_GENERATIVE_TRACKER=getValueLCD("select1.val");
    Serial.print("Mode Now: ");
    Serial.println(generativeStateName[STATE_GENERATIVE_TRACKER]);
    
    String NXTspacer="";
    NXTspacer+=(char)0x0D;
    NXTspacer+=(char)0x0A;
    String toDisplay=genModeInfo[STATE_GENERATIVE_TRACKER];
    toDisplay.replace(":",NXTspacer);

    changeAlignmentText(OUTPUT_GPT,1);
    sendTextLCD(OUTPUT_GPT,toDisplay);

    sendTextLCD(INPUT_KBD,promptBoxInfo[STATE_GENERATIVE_TRACKER]);

  }

}

void monitorKeycount(){
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
  //playVideo(VID_LOCK,100);
  STATE_TRACKER=STATE_DEVICE_LOCKED;
  safeCurrentState(STATE_TRACKER);
  changeLightMode(LED_MODE_PLASMA);
  sendTextLCD(STATUS_BAR,stateName[STATE_TRACKER]);
}

void sleepDevice(){
  STATE_TRACKER=STATE_DEVICE_SLEEP;
  safeCurrentState(STATE_TRACKER);
  analogWrite(PORT_LOGO_PWM_R,255);
  analogWrite(PORT_7S_ENABLE,255);

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
  sendTextLCD(STATUS_BAR,stateName[STATE_TRACKER]);
  changeLightMode(LED_MODE_PLASMA);
  clearTextLCD(INPUT_KBD);
  clearTextLCD(OUTPUT_GPT);
  buzzMotor(1,500);
}

void toKeyboardMode(){
  STATE_TRACKER=STATE_BASIC_KEYBOARD;
  safeCurrentState(STATE_TRACKER);
  sendTextLCD(STATUS_BAR,stateName[STATE_TRACKER]);
  stopCurrentVideo();
  hideObjectLCD(VID_OUTRO);
  changeLightMode(LED_MODE_PLASMA);
  clearTextLCD(INPUT_KBD);
  clearTextLCD(OUTPUT_GPT);
  showObjectLCD(CLOCK_PARA);
  showObjectLCD(DATE_PARA);
  showObjectLCD(SEC_BAR_PARA);
  sendTextLCD(CLOCK_LCD,getClockString());
  sendTextLCD(DATE_LCD,getCompleteDateString());
  setProgressBar(SEC_BAR,getSecondsScaled());
  hideObjectLCD(GPT_BAR_PARA);
  buzzMotor(1,500);
}

void toChatGPTMode(){
  STATE_TRACKER=STATE_CHAT_GPT_QUERY;
  safeCurrentState(STATE_TRACKER);
  changeLightMode(LED_MODE_SIDERAIN);
  sendTextLCD(STATUS_BAR,stateName[STATE_TRACKER]);
  changeAlignmentText(INPUT_KBD,1);
  sendTextLCD(INPUT_KBD,"ASK ME ANYTHING");
  hideObjectLCD(CLOCK_PARA);
  hideObjectLCD(DATE_PARA);
  hideObjectLCD(SEC_BAR_PARA);
  playVideo(VID_OUTRO,0);
  buzzMotor(2,250);
}

long clockRefreshCounter=millis();

void monitorClock(){
  if(millis()-clockRefreshCounter>1000){
    setProgressBar(SEC_BAR,getSecondsScaled());
    if(second()<=1){
      sendTextLCD(CLOCK_LCD,getClockString());
      sendTextLCD(DATE_LCD,getCompleteDateString());
    }
    clockRefreshCounter=millis();
  }
}

void streamingProcess(){
  if(digitalRead(PIN_GPT_STATUS)==LOW){
    changeLightMode(LED_MODE_SLANTBAR);
    buzzMotor(5,100);
    streamGPTResults(getTextLCD(OUTPUT_GPT,0));
    delay(2000);
    STATE_TRACKER=STATE_CHAT_GPT_QUERY;
    changeLightMode(LED_MODE_SIDERAIN);
    safeCurrentState(STATE_TRACKER);
    sendTextLCD(STATUS_BAR,"results sent");
    buzzMotor(5,100);
  }
}

void checkNFCTag(){
  if(scanNFC()==true){  
    playVideo(VID_UNLOCKED,100);
    STATE_TRACKER=STATE_BASIC_KEYBOARD;
    safeCurrentState(STATE_TRACKER);
    sendTextLCD(STATUS_BAR,stateName[STATE_TRACKER]);
    changeLightMode(LED_MODE_PLASMA);
    clearTextLCD(INPUT_KBD);
    clearTextLCD(OUTPUT_GPT);
    buzzMotor(1,500);
  }
}

void monitorKBDLock(char in, char inSp){
  if(in!=NULL || inSp<6){
    playVideo(VID_LOCK,100);
  }
}