#include <Arduino.h>
#include <IWatchdog.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <Ethernet.h>
#include <SSLClient.h>
#include "trust_anchors.h"

#include <SparkFun_VL53L5CX_Library.h> 

#include "GPIO.h"
#include "keyboard_map.h"
#include "LCD.h"
#include "EEPROM.h"
#include "Coprocessor.h"
#include "utils.h"
#include "NFC.h"
#include "ToF.h"

String openaiKey = ""; // Change your api key here

const char server[] = "api.openai.com";    // name address for Arduino (using DNS)

// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
const int rand_ADC_pin = PF6;

// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
EthernetClient base_client;
SSLClient client(base_client, TAs, (size_t)TAs_NUM, rand_ADC_pin);

// put function declarations here:
int myFunction(int, int);

const uint8_t MUX_CTRL_PORTS[] = {PORT_MUX_CTRL_S0, PORT_MUX_CTRL_S1, PORT_MUX_CTRL_S2, PORT_MUX_CTRL_S3};
const uint8_t MUX_READ_PINS[] = {PIN_KBD_L_MUX_1,PIN_KBD_L_MUX_2,PIN_KBD_R_MUX_1,PIN_KBD_R_MUX_2};

boolean keyStates[16][4];
boolean prevKeyStates[16][4];
boolean specialKeys[6];
boolean prevSpecialKeys[6];

const uint8_t scanIteration=16;

bool keyPressed(int channel, int mux);
bool keyActive(int channel, int mux);
bool isPrintableKey(int channel, int mux);

char scanKeyboard();
int scanSpecialKeys();

void print_network_info(void);
String openAI_chat(String message);
void lockdevice();
void wakeDevice();
void sleepDevice();

unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

//ChatGPT
String role = "You are a helpful assistant.";
String model = "gpt-3.5-turbo";
String system_content = "{\"role\": \"system\", \"content\":\""+ role +"\"}";
String historical_messages = system_content;

uint32_t clickCounter=0;
long timerSaveKeycount=millis();
long intervalSaveKeycount=1800000;

#define STATE_BASIC_KEYBOARD  0
#define STATE_CHAT_GPT_QUERY  1
#define STATE_CHAT_GPT_STREAM 2
#define STATE_DEVICE_LOCKED   99
#define STATE_DEVICE_SLEEP    88

byte STATE_TRACKER = 0;

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

void setup() {

  Serial.begin(115200); // Default Serial to STlink and ChatGPT buffer
  initLCDSerial();
  initBLE();
  initEEPROM();

  delay(500);
  //safeCurrentState(0);
  clickCounter=readKeyCountEEP();
  STATE_TRACKER=readCurrentState();

  //sendTextLCD(INPUT_KBD,"Hallo Sumasta This is i am testing reading the input");
  //delay(100);
  //sendTextLCD(OUTPUT_GPT,"This is reply from the output gpt");
  //appendTextLCD(OUTPUT_GPT," Ha");
  //delTextLCD(OUTPUT_GPT,1);
  //clearTextLCD(OUTPUT_GPT);

  //makeSelectionLCD(GPT_MODEL,1);
  //makeSelectionLCD(GPT_ROLE,1);

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
  
  /*
  for (int i=0;i<rep.length();i++){
    Serial.print(rep.charAt(i));
    Serial.print(" : ");
    Serial.println(rep.charAt(i));
  }
  Serial.println();
  */

  //IWatchdog.begin(2000000);

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

  if (Ethernet.begin(mac,5000,5000) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");  
    sendTextLCD(INTERNET_STATE,"ERROR: NO INTERNET");
  }else{
    String state="Connected to internet: ";
    state+=String(Ethernet.localIP());
    sendTextLCD(INTERNET_STATE,"Connected  ip:192.168.1.202  poe:802.3at");
    //clearTextLCD(STATUS_BAR);
    print_network_info();
  }

  if (IWatchdog.isReset(true)) {
    Serial.println("WDG RESET");
  }

  IWatchdog.begin(30000000);

  #ifdef USE_NFC
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1); // halt
    }
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

    Serial.println("Waiting for an ISO14443A Card ...");
  #endif

  #ifdef USE_TOF

    Wire.begin();
    Wire.setClock(400000); //Sensor has max I2C freq of 400kHz 
    
    Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
    if (myImager.begin() == false)
    {
      Serial.println(F("Sensor not found - check your wiring. Freezing"));
      while (1) ;
    }
    
    myImager.setResolution(8*8); //Enable all 64 pads
    
    imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width

    myImager.startRanging();

  #endif

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

  //digitalWrite(PORT_LOGO_PWM_R,LOW);
  //digitalWrite(PORT_LOGO_PWM_G,HIGH);
  analogWrite(PORT_LOGO_PWM_R,200);
  //analogWrite(PORT_LOGO_PWM_G,100);
  //analogWrite(PORT_LOGO_PWM_B,1023);

  analogWrite(PORT_7S_ENABLE,40);

  displayTypeCounter(clickCounter);

}

long logger=millis();
int loopcounter=0;

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

String openAI_chat(String message) { 
  

  /*
  message.replace("\"","'");
  String user_content = "{\"role\": \"user\", \"content\":\""+ message+"\"}";
  historical_messages += ", "+user_content;
  String request = "{\"model\":\""+model+"\",\"messages\":[" + historical_messages + "]}";
  */

  //String message="you are beautiful";
  safeKeycountEEP(clickCounter);
  message.replace("\"","'");
  String user_content = "{\"role\": \"user\", \"content\":\""+ message+"\"}";
  historical_messages += ", "+user_content;
  String temperature = "\"temperature\":"+String(0.8);
  String maxToken = "\"max_tokens\":"+String(200);
  String frequency = "\"frequency_penalty\":"+String(0.5);
  String user = "\"user\":\"Sumasta Generative Kaiboard\"";

  String request = "{\"model\":\""+model+"\",\"messages\":[" + historical_messages + "]," + temperature +","+ maxToken + ","+ frequency + ","+ user + "}";
  Serial.println(request);


  if (client.connect(server, 443)) {

    sendTextLCD(STATUS_BAR,"I'm thinking...");
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
    int waitTime = 30000;   // timeout 20 seconds
    long startTime = millis();
    while ((startTime + waitTime) > millis()) {
      //delay(100);      
      while (client.available()>0 && (startTime + waitTime) > millis()) {
          IWatchdog.begin(2000000);
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
            IWatchdog.begin(10000);
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

          } else if (getResponse.indexOf("\"")!=-1 && c == '\n'&&state==true) {
            state=false;
            getResponse = getResponse.substring(0,getResponse.length()-2);

            String spacer="";
            spacer+=(char)0x5C;
            spacer+=(char)0x6E;
            getResponse.replace(spacer," ");

            sendTextLCD(STATUS_BAR,"press ESCape to stream");
            sendTextLCD(OUTPUT_GPT,getResponse);
            IWatchdog.begin(10000);

            /*
            for(int i=0;i<getResponse.length();i++){
              BLE.print(getResponse.charAt(i));
              delay(50);
            }
            */
            
            //return getResponse;
            //delay(500);
            //NVIC_SystemReset();
            client.flush();
            client.stop();
          }    

          //startTime = millis();
          //Serial.println(client.available());
          //delay(10);
      }

      if (getResponse.length()>0){
        client.stop();
        String assistant_content = "{\"role\": \"assistant\", \"content\":\""+ getResponse+"\"}";
        historical_messages += ", "+assistant_content;
        Serial.println("reply: ");
        Serial.println(getResponse);
        return getResponse;
      }
    }
    
    client.stop();
    Serial.println(Feedback);
    return "error";
  }else
    sendTextLCD(STATUS_BAR,"Can't connect to openai");
    return "Connection failed";  
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

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
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
      
      if (pinState==LOW){

        if(pinState!=prevKeyStates[i][z]){
          if(caseMux[i][z]==8){ // backspace
            prevKeyStates[i][z]=keyStates[i][z];
            return caseMux[i][z];
          }else{
            prevKeyStates[i][z]=keyStates[i][z];
            displayTypeCounter(++clickCounter);
            return caseMux[i][z];
          }
        }
        //Serial.print(caseMux[i][z]);
        //Serial.print(caseMux[i][z]);
        //Serial.print(" STATE: ");
        //Serial.print(i);
        //Serial.print(" MUX: ");
        //Serial.println(z);
        //delay(100);
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

  for (int i=0;i<6;i++){
    if (specialKeys[i]==LOW && specialKeys[i]!=prevSpecialKeys[i]) {
      //Serial.print("SP_K:");
      //Serial.println(i);
      if (specialKeys[4]==LOW && STATE_TRACKER!=STATE_DEVICE_LOCKED){
        lockdevice();
      }else if(specialKeys[2]==LOW && STATE_TRACKER!=STATE_DEVICE_SLEEP){
        sleepDevice();
      }
      
      
      return i;
    }
  }

  for (int i=0;i<6;i++){
    prevSpecialKeys[i]=specialKeys[i];
  }

  return 99;

}

void printKeys(){
  for (uint8_t i=0;i<scanIteration;i++){
    for (uint8_t x=0; x<4; x++){
      if (keyStates[i][x]==LOW){       
        if (isPrintableKey(i,x)){
          //Serial.print(caseMux[i][x]);
        }
      }
    }
  }
}

bool isPrintableKey(int channel, int mux){
  return true;
}

void print_network_info(void){
  byte print_mac[] ={0,};
  Serial.println("\r\n-------------------------------------------------");
  Serial.printf("MAC        : ");
  Ethernet.MACAddress(print_mac);
  for (byte i = 0; i < 6; i++) {
    Serial.print(print_mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
  Serial.printf("IP         : ");
  Serial.println(Ethernet.localIP());
  Serial.printf("Gateway IP : ");
  Serial.println(Ethernet.subnetMask());
  Serial.printf("DNS        : ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println("-------------------------------------------------");
  
}


/*
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
  */ 

  /*
  while(BLE.available()>0){
    Serial.write(BLE.read());
  }

  while(LCD.available()>0){
    Serial.write(LCD.read());
  }
  */
 /*
  if(digitalRead(PIN_GPT_STATUS)==LOW){
    Serial.println("GPT ENABLED");
    digitalWrite(PORT_VIB_MOT_L,HIGH);
    digitalWrite(PORT_VIB_MOT_R,HIGH);
    delay(500);
  }else{
    digitalWrite(PORT_VIB_MOT_L,LOW);
    digitalWrite(PORT_VIB_MOT_R,LOW);
  }
*/
