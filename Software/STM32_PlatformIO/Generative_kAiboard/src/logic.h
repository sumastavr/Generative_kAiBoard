
#include <Arduino.h>

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
  sendTextLCD(STATUS_BAR,"keyboard is locked");
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
  sendTextLCD(STATUS_BAR,"keyboard mode");
  changeLightMode(LED_MODE_PLASMA);
  clearTextLCD(INPUT_KBD);
  clearTextLCD(OUTPUT_GPT);
  buzzMotor(1,500);
}

void toKeyboardMode(){
  STATE_TRACKER=STATE_BASIC_KEYBOARD;
  safeCurrentState(STATE_TRACKER);
  sendTextLCD(STATUS_BAR,"keyboard mode");
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
  sendTextLCD(STATUS_BAR,"CHAT GPT MODE");
  sendTextLCD(INPUT_KBD,"ASK ME ANYTHING...");
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