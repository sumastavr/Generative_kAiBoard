#include <Ethernet.h>
#include <EthernetUdp.h>
#include "HardwareSerial.h"

/* Network */
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 104);

unsigned int localPort = 8888;      // local port to listen on

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
EthernetUDP Udp;
void print_network_info(void);

void setup() {
  //STM32F429ZI's Serial port changed from default Serial Port
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);
  Serial3.begin(9600);

  Ethernet.begin(mac, ip);

  // start UDP
  print_network_info();
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
    if (packetSize) {

    Serial3.print("\r\n[From ");
    Serial.print(Udp.remoteIP());
    Serial.printf(": %d]: \r\n", Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println(packetBuffer);

    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(packetBuffer);
    Udp.endPacket();
  }
  delay(10);
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
  Serial.printf(": %d\r\n", localPort);
  Serial.println("-------------------------------------------------");
}