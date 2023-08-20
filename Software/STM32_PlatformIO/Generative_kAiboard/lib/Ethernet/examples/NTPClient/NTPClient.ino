#include <TimeLib.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "HardwareSerial.h"

/* Network */
#define NTP_PACKET_SIZE 48

/* Time */
#define SEVENTY_YEARS (2208988800UL)
#define TIME_SERVER   "time.nist.gov"

unsigned long highWord;
unsigned long lowWord;
unsigned long secsSince1900;
unsigned long epoch;

/* Network */
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 111); 
unsigned int localPort = 5000;      

byte packetBuffer[NTP_PACKET_SIZE]; 
EthernetUDP Udp;

void sendNTPpacket(const char * address);
void print_network_info(void);

void setup() {

  Serial3.setRx(PC11);
  Serial3.setTx(PC10);
  Serial3.begin(9600);
  Ethernet.begin(mac, ip);
  print_network_info();
  Udp.begin(localPort);
}

void loop() {
  sendNTPpacket(TIME_SERVER); 

  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE); 

    highWord = word(packetBuffer[40], packetBuffer[41]);
    lowWord = word(packetBuffer[42], packetBuffer[43]); 
    secsSince1900 = highWord << 16 | lowWord;
    epoch = secsSince1900 - SEVENTY_YEARS;

    setTime(epoch);   
    Serial3.printf("[%d/%d/%d %d:%d:%d] GMT \r\n", day(), month(), year(),  hour(), minute(), second());
  }

  delay(10000);
}

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {

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
  Serial.print(Ethernet.localIP());
  Serial.println("\r\n-------------------------------------------------");
}