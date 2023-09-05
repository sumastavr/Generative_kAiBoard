
#include <Arduino.h>
#include <FastLED.h>
#include <BleKeyboard.h>
#include <EEPROM.h>

#define LEDCHANGEHEADER     212
#define LEDCHANGEFOOTER     213

#define LEDSLEEPMODEHEADER  215
#define LEDSLEEPMODEFOOTER  216

#define KBDPRESSCODE        201
#define KBDRELEASECODE      202

SET_LOOP_TASK_STACK_SIZE(16*1024);

// RGB Shades data output to LEDs is on pin 5
#define LED_PIN  13

// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 255
#define STARTBRIGHTNESS 25

// Cycle time (milliseconds between pattern changes)
#define cycleTime 7000

// Hue time (milliseconds between hue increments)
#define hueTime 30

// Time after changing settings before settings are saved to EEPROM
#define EEPROMDELAY 2000

#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"

// list of functions that will be displayed
functionList effectList[] = { threeDee,
                              plasma,
                              threeSine,
                              confetti,
                              rider,
                              glitter,
                              slantBars,
                              colorFill,
                              sideRain,
                            };


const byte numEffects = (sizeof(effectList)/sizeof(effectList[0]))-1;
//const byte numEffects=4;

BleKeyboard bleKeyboard;

#define PIN_JYS_L_Y   32
#define PIN_JYS_L_X   15
#define PIN_JYS_L_BUT 4

#define PIN_JYS_R_Y   14
#define PIN_JYS_R_X   12
#define PIN_JYS_R_BUT 35

#define PIN_SLD_L     27
#define PIN_SLD_R     26

#define PORT_LED_SLD_L  23
#define PORT_LED_SLD_R  22

void setup() {
  pinMode(21,OUTPUT);
  digitalWrite(21,LOW);

  pinMode(PIN_JYS_L_Y,INPUT);
  pinMode(PIN_JYS_L_X,INPUT);
  pinMode(PIN_JYS_L_BUT,INPUT_PULLUP);
  
  pinMode(PIN_JYS_R_Y,INPUT);
  pinMode(PIN_JYS_R_X,INPUT);
  pinMode(PIN_JYS_R_BUT,INPUT_PULLUP);

  pinMode(PIN_SLD_L,INPUT);
  pinMode(PIN_SLD_R,INPUT);

  pinMode(PORT_LED_SLD_L,OUTPUT);
  pinMode(PORT_LED_SLD_R,OUTPUT);
  
  Serial.begin(115200);
  Serial2.begin(115200);
  
  //FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  //FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);

  Serial.println(ESP.getFreeHeap());
  bleKeyboard.setName("Generative kAiboard");
  bleKeyboard.begin();

}

long readADCTimer=millis();

long readBrightnessSetTimer=millis();
int intervalReadBrightness=100;

void checkBrightness();

int lastMappedBrightness=0;
int currentMappedBrightness=0;

bool sleepMode=false;

void checkBrightness(){

  currentMappedBrightness=map(analogRead(PIN_SLD_L),0,4095,0,MAXBRIGHTNESS);
  if(currentMappedBrightness!=lastMappedBrightness && !sleepMode){
    #ifdef FULL_DEBUG
      Serial.println("New Brightness: ");
      Serial.println(currentMappedBrightness);
    #endif
    FastLED.setBrightness(scale8(currentMappedBrightness,MAXBRIGHTNESS));
  }
  lastMappedBrightness=currentMappedBrightness;

}

bool toggleLEDSlider=false;

long timerToggleLED=millis();
int intervalTimerToggleLED=500;

void toggleSliderLED(){

  toggleLEDSlider=!toggleLEDSlider;
  digitalWrite(PORT_LED_SLD_R,toggleLEDSlider);
  digitalWrite(PORT_LED_SLD_L,!toggleLEDSlider);  

}

long readModeLightTimer=millis();
int intervalReadModeLightTimer=0;
byte currentLightModeSelection=0;
byte lastLightModeSelection=0;

void checkModeLight(){
  currentLightModeSelection=map(analogRead(PIN_SLD_R),0,4095,0,numEffects);
  if(currentLightModeSelection!=lastLightModeSelection){
    effectInit = false;
    currentEffect=currentLightModeSelection;
  }
  lastLightModeSelection=currentLightModeSelection;
}

long readLeftJoystickTimer=millis();
int readLeftJoystickInterval=250;
const int thresholdHigh=3500;
const int thresholdLow=1000;

void checkLeftJoystick(){
  int xValue=analogRead(PIN_JYS_L_X);
  int yValue=analogRead(PIN_JYS_L_Y);
  bool button=digitalRead(PIN_JYS_L_BUT);

  if(yValue>thresholdHigh){
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
  }else if(yValue<thresholdLow){
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
  }

  if(xValue>thresholdHigh){
    bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
    delay(500);
  }else if(xValue<thresholdLow){
    bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
    delay(500);
  }

  if(button==LOW){
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    delay(500);
  }
  
}



void loop() {

  if(millis()-timerToggleLED>intervalTimerToggleLED){
    toggleSliderLED();
    timerToggleLED=millis();
  }

  if(millis()-readBrightnessSetTimer>intervalReadBrightness){
    checkBrightness();
    readBrightnessSetTimer=millis();
  }

  if(millis()-readModeLightTimer>intervalReadModeLightTimer){
    checkModeLight();
    readModeLightTimer=millis();
  }

  if(millis()-readLeftJoystickTimer>readLeftJoystickInterval){
    checkLeftJoystick();
    readLeftJoystickTimer=millis();
  }


  
  currentMillis = millis(); // save the current timer value

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true && currentLightModeSelection==0) {
    cycleMillis = currentMillis;
    effectInit = false;
    if(++currentEffect>numEffects)currentEffect=0;
  }

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    effectList[currentEffect](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);

  if(!sleepMode){
    FastLED.show(); // send the contents of the led memory to the LEDs
    FastLED.delay(5);
  }

  if(bleKeyboard.isConnected()) {
    while(Serial2.available()>0){
      byte in=Serial2.read();
      if(in==LEDCHANGEHEADER){
        Serial.println("LED Change");
        delay(5);
        byte lightMode=Serial2.read();
        if (lightMode>=49 && lightMode <=57){
          lightMode-=48;
          Serial.println(lightMode);
          effectInit = false;
          currentEffect=lightMode;
          Serial2.flush();
        }
      }else if(in==LEDSLEEPMODEHEADER){
        Serial.println("SLEEP MODE Command");
        delay(5);
        byte enableLED=Serial2.read();
        if (enableLED>=48 && enableLED <=50){
          //enableLED-=48;
          Serial.println(enableLED);
          if(enableLED==49){
            FastLED.setBrightness(0);
            FastLED.show(); 
            sleepMode=true;
          }else if(enableLED==48){
            FastLED.setBrightness(scale8(currentMappedBrightness,MAXBRIGHTNESS));
            sleepMode=false;
          }
          Serial2.flush();
        }
      }else if(in==KBDPRESSCODE){
        delay(5);
        byte key=Serial2.read();
        bleKeyboard.press(char(key));
        Serial.print("PRESS: "); 
        Serial.println(char(key));
      }else if(in==KBDRELEASECODE){
        delay(5);
        byte key=Serial2.read();
        bleKeyboard.release(char(key));
        Serial.print("RELEASE: "); 
        Serial.println(char(key));
      }else{
        Serial.write(in);
        bleKeyboard.print(char(in));
        //bleKeyboard.write(in);
      }
      
    }
  }
}

/*
  if(millis()-readADCTimer>500){

    Serial.print(analogRead(PIN_JYS_L_Y));
    Serial.print('\t');
    Serial.print(analogRead(PIN_JYS_L_X));
    Serial.print('\t');
    
    Serial.print(analogRead(PIN_JYS_R_Y));
    Serial.print('\t');
    Serial.print(analogRead(PIN_JYS_R_X));
    Serial.print('\t');

    Serial.print(analogRead(PIN_SLD_L));
    Serial.print('\t');
    Serial.print(analogRead(PIN_SLD_R));
    Serial.print('\t');

    Serial.print(digitalRead(PIN_JYS_L_BUT));
    Serial.print('\t');
    Serial.print(digitalRead(PIN_JYS_R_BUT));
    Serial.println('\t');
    
    int brightness=map(analogRead(PIN_SLD_L),0,4095,0,255);
    FastLED.setBrightness(brightness);

    toggle=!toggle;

    digitalWrite(PORT_LED_SLD_R,toggle);
    digitalWrite(PORT_LED_SLD_L,!toggle);
    
    readADCTimer=millis();

  }
  */
