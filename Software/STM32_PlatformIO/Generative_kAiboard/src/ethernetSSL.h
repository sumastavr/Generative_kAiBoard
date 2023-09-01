/*

BSD 3-Clause License

Copyright (c) 2023, Pamungkas Sumasta (www.sumasta.tech)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <Arduino.h>

#include <Ethernet.h>
#include <SSLClient.h>
#include "trust_anchors.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
const int rand_ADC_pin = PF6;

// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
EthernetClient base_client;
SSLClient client(base_client, TAs, (size_t)TAs_NUM, rand_ADC_pin);

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

bool initEthernetDHCP(int timeout){
   if (Ethernet.begin(mac,timeout,timeout) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");  
      sendTextLCD(INTERNET_STATE,"ERROR: NO INTERNET");
      return false;
   }else{
      String state="Connected  IP:";
      state+=String(Ethernet.localIP()[0]);
      state+=":";
      state+=String(Ethernet.localIP()[1]);
      state+=":";
      state+=String(Ethernet.localIP()[2]);
      state+=":";
      state+=String(Ethernet.localIP()[3]);

      if(isPOEATDetected()){
         state+="  POE:802.3AT";
      }else{
         state+="  POE:802.3AF";
      }
      
      Serial.println(state);
      sendTextLCD(INTERNET_STATE,state);
      print_network_info();
      return true;
   }
}