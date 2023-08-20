#include <Ethernet.h>
#include <EthernetUdp.h>
#include "HardwareSerial.h"

/* Network */
void print_network_info(void);

void setup() {
  //STM32F429ZI's Serial port changed from defaulat Serial Port
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);
  Serial3.begin(9600);

  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  if (Ethernet.begin(mac) == 0) {
    Serial3.println("Failed to configure Ethernet using DHCP");
    while (true);
  }

  print_network_info();
}

void loop() {

  switch (Ethernet.maintain()) {
    case 1:
      Serial.println("Error: renewed fail");
      break;

    case 2:
      Serial.println("Renewed success");
      print_network_info();
      break;

    case 3:
      Serial.println("Error: rebind fail");
      break;

    case 4:
      Serial.println("Rebind success");
      print_network_info();
      break;

    default:
      break;
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
  
}