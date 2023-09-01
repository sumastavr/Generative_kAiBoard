
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