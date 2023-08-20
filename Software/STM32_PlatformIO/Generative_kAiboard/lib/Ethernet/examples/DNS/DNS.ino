#include <Arduino.h>
#include <Ethernet.h>
#include "Dns.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  DNSClient dns;
	IPAddress dns_ip;
  char dns_domain[] = "www.wiznet.io";

  //STM32F429ZI's Serial port changed from default Serial Port
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);
  Serial3.begin(9600);
 
  Ethernet.begin(mac);

	dns.begin(Ethernet.dnsServerIP());
	dns.getHostByName(dns_domain, dns_ip);

  Serial.println("\r\n-------------------------------------------------");
  Serial.printf("Target domain : %s \r\nIP of target domain :", dns_domain);
  Serial.print(dns_ip);
  Serial.println("\r\n-------------------------------------------------");
  
}
void loop() {
  ;
}