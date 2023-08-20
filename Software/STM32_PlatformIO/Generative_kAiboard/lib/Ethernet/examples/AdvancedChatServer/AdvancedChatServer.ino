#include <Arduino.h>
#include "Ethernet.h"
#include "HardwareSerial.h"

/* Network Info */
#define SERVER_PORT 5000
#define MAX_CLIENT  8

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 104);
IPAddress myDns(8, 8, 8, 8);
IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server(SERVER_PORT);
EthernetClient clients[MAX_CLIENT];

void print_network_info(void);

void setup() {
  //STM32F429ZI's Serial port changed from default Serial Port
  Serial3.setRx(PC11);
  Serial3.setTx(PC10); 
  Serial3.begin(9600);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);

   print_network_info();
   server.begin();
}

void loop() {
  EthernetClient newClient = server.accept();

  if (newClient) {  	
    for (byte i=0; i < MAX_CLIENT; i++) {
      if (!clients[i]) {
        Serial.printf("We have a new client # %d \r\n", i);
        newClient.printf("Hello, client number: %d\r\n", i);

        clients[i] = newClient;
        break;
      }
    }
  }

  for (byte i=0; i < MAX_CLIENT; i++) {
    if (clients[i] && clients[i].available() > 0) {
      byte buffer[80];
      int count = clients[i].read(buffer, 80);

      for (byte j=0; j < MAX_CLIENT; j++) {
        if (j != i && clients[j].connected()) {
          clients[j].write(buffer, count);
        }
      }
    }
  }

  for (byte i=0; i < MAX_CLIENT; i++) {
    if (clients[i] && !clients[i].connected()) {
      Serial.printf("disconnect client #%d \r\n", i);

      clients[i].stop();
    }
  }
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
  Serial.printf("Advanced Chat Server(port:%d) start ... \r\n", SERVER_PORT);
}
