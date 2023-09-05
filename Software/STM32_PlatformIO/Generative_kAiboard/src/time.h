
#include <Arduino.h>
#include <EthernetUdp.h>
#include <TimeLib.h>

/* Network */
#define NTP_PACKET_SIZE 48

/* Time */
#define SEVENTY_YEARS (2208988800UL)
//#define TIME_SERVER   "time.nist.gov"
#define TIME_SERVER   "pool.ntp.org"


unsigned long highWord;
unsigned long lowWord;
unsigned long secsSince1900;
unsigned long epoch;

const int timeZone = 2; // CET

unsigned int localPort = 123;      

byte packetBuffer[NTP_PACKET_SIZE]; 
EthernetUDP Udp;

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {

  Udp.begin(localPort);

  //delay(1000);

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  
  Udp.beginPacket(address, 123); 
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();

  long startCounter=millis();
  bool replied=false;

  while(millis()-startCounter<2000 && !replied){
    if (Udp.parsePacket()) {
        Udp.read(packetBuffer, NTP_PACKET_SIZE); 

        highWord = word(packetBuffer[40], packetBuffer[41]);
        lowWord = word(packetBuffer[42], packetBuffer[43]); 
        secsSince1900 = highWord << 16 | lowWord;
        epoch = secsSince1900 - SEVENTY_YEARS + timeZone * SECS_PER_HOUR;

        setTime(epoch);   
        Serial.printf("[%d/%d/%d %d:%d:%d] CET \r\n", day(), month(), year(),  hour(), minute(), second());
        replied=true;
    }
  }
}

void printTime(){
    Serial.printf("[%d/%d/%d %d:%d:%d] CET \r\n", day(), month(), year(),  hour(), minute(), second());
}

String getSecondsString(){
    String sec=String(second());
    if(second()<10){
        sec="0"+sec;
    }
    return sec;
}

String getMinuteString(){
    String min=String(minute());
    if(minute()<10){
        min="0"+min;
    }
    return min;
}

String getHourString(){
    String hours=String(hour());
    if(hour()<10){
        hours="0"+hours;
    }
    return hours;
}

String getClockString(){
    String clockString=getHourString()+":"+getMinuteString();
    return clockString;
}

String getNameofDayString(){
    String nameDays[]={" ","Sunday","Monday","Tuesday","Wednesday", "Thursday", "Friday", "Saturday"};
    return nameDays[weekday()];
}

String getCompleteDateString(){
    String dateComplete=getNameofDayString();
    dateComplete+="  "+String(day())+"/"+String(month())+"/"+String(year());
    return dateComplete;
}

int getSecondsScaled(){
    return map(second(),0,59,0,100);
}
