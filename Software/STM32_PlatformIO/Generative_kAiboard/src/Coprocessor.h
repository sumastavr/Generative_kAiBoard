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

void initBLE(){
    
  BLE.setTx(BLE_TX_PIN);
  BLE.setRx(BLE_RX_PIN);
  BLE.begin(115200);

}

void sendChar(char input){
    BLE.print(input);
}

void streamGPTResults(String answer){
    for(int i=0;i<answer.length();i++){
        IWatchdog.reload();
        BLE.print(answer.charAt(i));
        delTextLCD(OUTPUT_GPT,1);
        byte randomDelay=DELAY_STREAM+random(DELAY_STREAM_VAR);
        delay(randomDelay);
    }
}

void changeLightMode(int mode){

    String payload="";
    payload+=(char)LEDCHANGEHEADER;
    payload+=String(mode);
    payload+=(char)LEDCHANGEFOOTER;

    BLE.print(payload);

}

void sleepModeLight(int state){
    String payload="";
    payload+=(char)LEDSLEEPMODEHEADER;
    payload+=String(state);
    payload+=(char)LEDSLEEPMODEFOOTER;
    BLE.print(payload);
}