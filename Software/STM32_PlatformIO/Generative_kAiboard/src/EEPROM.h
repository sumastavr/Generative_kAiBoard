
#include "I2C_24LC1025.h"

I2C_24LC1025 ee(0x50);

#define EEP_ADD_KEYCOUNTER  1000
#define EEP_CURRENT_STATE_MODE  2000
#define EEP_GPT_RESULT    3000

uint32_t start, stop;

void initEEPROM(){
  if (!ee.begin()){
    Serial.println("EEPROM not found...");
  }
}

bool safeCurrentState(byte state){
  bool success=true;
  if(ee.readByte(EEP_CURRENT_STATE_MODE)!=state){
    if(!ee.writeByteVerify(EEP_CURRENT_STATE_MODE,state)) success=false; 
  }
  return success;
}

byte readCurrentState(){
    return ee.readByte(EEP_CURRENT_STATE_MODE);
}

bool safeKeycountEEP(uint32_t counter){
  byte *ptr;
  ptr = (byte*) &counter;
  byte m0 = *ptr;
  bool success=true;
  
  if(ee.readByte(EEP_ADD_KEYCOUNTER)!=m0){
    if(!ee.writeByteVerify(EEP_ADD_KEYCOUNTER,m0)) success=false; 
  }

  ptr++;
  m0 = *ptr;
  if(ee.readByte(EEP_ADD_KEYCOUNTER+1)!=m0){
    if(!ee.writeByteVerify(EEP_ADD_KEYCOUNTER+1,m0)) success=false; 
  }

  ptr++;
  m0 = *ptr;
  if(ee.readByte(EEP_ADD_KEYCOUNTER+2)!=m0){
    if(!ee.writeByteVerify(EEP_ADD_KEYCOUNTER+2,m0)) success=false; 
  }

  ptr++;
  m0 = *ptr;
  if(ee.readByte(EEP_ADD_KEYCOUNTER+3)!=m0){
    if(!ee.writeByteVerify(EEP_ADD_KEYCOUNTER+3,m0)) success=false; 
  }

  return success;
}

uint32_t readKeyCountEEP(){

  uint32_t temp = ee.readByte(EEP_ADD_KEYCOUNTER+3);
  temp = temp << 8; // shift left
  temp = temp | ee.readByte(EEP_ADD_KEYCOUNTER+2); // or result with byte 1
  temp = temp << 8; // shift left
  temp = temp | ee.readByte(EEP_ADD_KEYCOUNTER+1); // or result with byte 2
  temp = temp << 8; // shift left
  temp = temp | ee.readByte(EEP_ADD_KEYCOUNTER); // or result with byte 3

  return temp;

}

void dump(uint32_t from, uint32_t to)
{
  for (uint32_t i = from; i < to; i++)  // I2C_DEVICESIZE_24LC1025
  {
    volatile int x = ee.readByte(i);
    char buffer[24];
    if (i % 16 == 0)
    {
      char buffer[24];
      Serial.print('\n');
#if defined (ESP8266) || defined(ESP32)
      sprintf(buffer, "%08X\t", i);   //  ESP cast (long unsigned int)
#else
      sprintf(buffer, "%08lX\t", i);  //  AVR needs lX
#endif
      Serial.print(buffer);
    }
    sprintf(buffer, "%02X\t", x);
    Serial.print(buffer);
  }
  Serial.println();
}



void test(uint32_t offset)
{
  Serial.println();

  for (int i = 0; i < 10; i++)
  {
    ee.writeByte(offset + i, i);
    Serial.println(offset + i);
    delay(10);
  }

  for (int i = 0; i < 10; i++)
  {
    if (i % 10 == 0) Serial.print('\n');
    Serial.print(ee.readByte(offset + i));
    Serial.print('\t');
    delay(10);
  }
  Serial.print('\n');
}