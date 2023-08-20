#include <Arduino.h>
#include "Ethernet.h"
#include "HardwareSerial.h"

/* Network */
#define SERVER_PORT 5000
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 11, 111); 

EthernetServer server(SERVER_PORT);
void print_network_info(void);

void setup() {
  //STM32F429ZI's Serial port changed from default Serial Port
  Serial3.setRx(PC11);
  Serial3.setTx(PC10);  
  Serial3.begin(9600);

  Ethernet.begin(mac, ip);
  print_network_info();
  
  // start the server
  server.begin();
  Serial3.print("server is at ");
  Serial3.println(Ethernet.localIP());
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial3.println("new client");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial3.write(c);
  
          if (c == '\n') {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); 
          client.println("Refresh: 5");  
          client.println();
          client.println("<!DOCTYPE HTML>");

          client.println("<html>");
          client.print("Hello World \r\n");
          client.println("</html>");
          break;
        }
      }
    }

    delay(1);
    client.stop();
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
  Serial.print(Ethernet.localIP());
  Serial.printf(": %d\r\n", SERVER_PORT);
  Serial.println("-------------------------------------------------");
}