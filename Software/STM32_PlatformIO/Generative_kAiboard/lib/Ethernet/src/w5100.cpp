/*
 * Copyright 2018 Paul Stoffregen
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <Arduino.h>
#include "Ethernet.h"
#include "w5100.h"
#include "GxIO_STM32F4_FMC.h"

/***************************************************/
/**            Default SS pin setting             **/
/***************************************************/


// If variant.h or other headers specifically define the
// default SS pin for Ethernet, use it.

#define SS_PIN_DEFAULT	PD7

#define WIZCHIP_RST PC8
#define WIZCHIP_INT PC9
#define WIZCHIP_CS1 PD7

// W5100 controller instance
//uint8_t  W5300Class::ss_pin = SS_PIN_DEFAULT;
//uint16_t W5300Class::SSIZE = 2048;
//uint16_t W5100Class::SMASK = 0x07FF;

W5300Class W5300;

uint8_t W5300Class::chip = 53;
uint16_t W5300Class::sock_io_mode = 0;
uint16_t W5300Class::sock_any_port = SOCK_ANY_PORT_NUM;
uint16_t W5300Class::sock_is_sending = 0;
//uint16_t W5300Class::SSIZE = 2048;
//uint16_t W5300Class::SMASK = 0x07FF;

uint8_t W5300Class::init(void)
{
	static bool initialized = false;
	uint8_t i;

	if (initialized) return 1;

	// Many Ethernet shields have a CAT811 or similar reset chip
	// connected to W5100 or W5200 chips.  The W5200 will not work at
	// all, and may even drive its MISO pin, until given an active low
	// reset pulse!  The CAT811 has a 240 ms typical pulse length, and
	// a 400 ms worst case maximum pulse length.  MAX811 has a worst
	// case maximum 560 ms pulse length.  This delay is meant to wait
	// until the reset pulse is ended.  If your hardware has a shorter
	// reset time, this can be edited or removed.

	//Serial.println("w5100 init");

	//initSS();
	//resetSS();

  digitalWrite(WIZCHIP_CS1, LOW);
  digitalWrite(WIZCHIP_RST, HIGH);

  //W5300 Setup
  pinMode(WIZCHIP_INT, INPUT);
  pinMode(WIZCHIP_RST, STM_MODE_OUTPUT_PP);
  pinMode(WIZCHIP_CS1, STM_MODE_OUTPUT_PP);
  pinMode(PB14, STM_MODE_OUTPUT_PP);
  //STM FMC Setup
  FMC.init();
  delay(50);

  digitalWrite(WIZCHIP_CS1, LOW);
  digitalWrite(WIZCHIP_RST, HIGH);

  delay(560);
	//SSIZE = 2048;
	uint8_t memsize[2][MAX_SOCK_NUM] = {{8, 8, 8, 8}, {32, 32, 32, 32}};
  //uint8_t memsize[2][MAX_SOCK_NUM] = {{8}, {8}};

	for (int8_t i=0; i<MAX_SOCK_NUM; i++) {
		W5300.write_SnTX_SIZE(i, memsize[0][i]);
		W5300.write_SnRX_SIZE(i, memsize[1][i]);
	}

	W5300.reset_soft();

	return 1; // successful init
}

W5300Linkstatus W5300Class::getLinkStatus()
{
	if (init())
    return LINK_ON;
  else
    return LINK_OFF;
#if 0
	if (!init()) return UNKNOWN;
	switch (chip) {
		case 53:
		//phystatus = readPSTATUS_W5300();              //TODO: W5300, Check Register Addr of PHYCFGR_W5300
		//phystatus = readPHYCFGR_W5300();
		Serial.println("W5300Class::getLinkStatus() \n");
		phystatus = 1;                                  //TODO: W5300, Temp Code:  Not implemented PHYCFGR
		if (phystatus & 0x01) return LINK_ON;
		return LINK_OFF;
	  default:
		return UNKNOWN;
	}
#endif
}

 void W5300Class::write_SnRX_SIZE(uint8_t sn, uint8_t size ){
	//W5300.setRMSR(sn, size);
	setRMSR(sn, size);
}
 void W5300Class::write_SnTX_SIZE(uint8_t sn, uint8_t size){
	//W5300.setTMSR(sn, size);
	setTMSR(sn, size);
}

void W5300Class::reset_soft(void){
  //Serial.println("W5300Class::reset_soft() \n");
  uint8_t gw[4], sn[4], sip[4];
  uint8_t mac[6];

  getSHAR(mac);
  getGAR(gw);
  getSUBR(sn);
  getSIPR(sip);
  setMR(MR_RST);
  getMR(); // for delay

  setSHAR(mac);
  setGAR(gw);
  setSUBR(sn);
  setSIPR(sip);

  chip = getIDR();
}

void W5300Class::setup_Ethernet(void){

	uint8_t  mac[6] = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56};
	uint8_t  ip[4] = {192, 168, 11, 111};
	uint8_t  sn[4] = {255, 255, 255, 0};
	uint8_t  gw[4] = {192, 168, 11, 1};
	W5300.setMACAddress(mac);
	W5300.setIPAddress(ip);
	W5300.setGatewayIp(gw);
	W5300.setSubnetMask(sn);
}

uint16_t W5300Class::getSnRX_RSR(uint8_t sn)
{
   uint32_t received_rx_size=0;
   uint32_t received_rx_size1=1;
   while(1)
   {
      received_rx_size = (((uint32_t)WIZCHIP_READ(Sn_RX_RSR(sn))) << 16) |
                         (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),2))) & 0x0000FFFF);
      if(received_rx_size == received_rx_size1) break;
      received_rx_size1 = received_rx_size;                                      // if first == sencond, Sn_RX_RSR value is valid.
   }                                                                             // save second value into first
   return received_rx_size + (uint32_t)((sock_pack_info[sn] & 0x02) ? 1 : 0);
}

uint32_t W5300Class::getSnTX_FSR(uint8_t sn)
{
   uint32_t free_tx_size=0;
   uint32_t free_tx_size1=1;
   while(1)
   {
      free_tx_size = (((uint32_t)WIZCHIP_READ(Sn_TX_FSR(sn))) << 16) |
                     (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),2))) & 0x0000FFFF);                           // read
      if(free_tx_size == free_tx_size1) break;  // if first == sencond, Sn_TX_FSR value is valid.
      free_tx_size1 = free_tx_size;             // save second value into first
   }
   return free_tx_size;

}

void W5300Class::execCmdSn(uint8_t s, uint8_t cmd)
{
	// Send command to socket

	W5300.setSnCR(s, cmd); //TODO: W5300 debug
	// Wait for command to complete
	delay(10); //W5300 Debug
	//Serial.printf("W5300 execCmdSn socket CMD(Sock Open:0x01, Listen:0x02)  =0x%02X\n", cmd);

	while (W5300.getSnCR(s)) ;
	//while (uint8_t temp = getSn_CR(s))
	//{
	//	Serial.printf("W5300 execCmdSn socket CMD( Close ), CMD = 0x%02X\n", temp);
	//}
}

uint16_t  W5300Class::write(uint16_t addr, const uint16_t *buf, uint16_t len)
{
  for (int i=0; i <len;i++){
    //write(addr, (uint16_t)buf[i]);
    _W5300_DATA(addr) =(uint16_t)buf[i];
  }
  return len;
}

uint16_t  W5300Class::read(uint16_t addr, uint16_t *buf, uint16_t len)
{
  for (int i=0; i <len;i++){
    buf[i]= _W5300_DATA(addr);
  }
  return len;
}

void  W5300Class::setRMSR(uint8_t sn,uint8_t rmsr)
{
  uint16_t rmem;
  //rmem = read(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)));
  rmem = WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)));
  if(sn & 0x01)  rmem = (rmem & 0xFF00) | (((uint16_t)rmsr ) & 0x00FF) ;
  else rmem =  (rmem & 0x00FF) | (((uint16_t)rmsr) << 8) ;
  //write(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)),rmem);
  WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)),rmem);
  //Serial3.printf("setRMSR: %x, mem: %x \r\n", rmem, rmem);
}

uint8_t  W5300Class::getRMSR(uint8_t sn)
{
  if(sn & 0x01)
  	return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE))) & 0x00FF);
  return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE))) >> 8);
}

void  W5300Class::setTMSR(uint8_t sn,uint8_t tmsr)
{
  uint16_t tmem;
  tmem = WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE)));
  if(sn & 0x01)  tmem = (tmem & 0xFF00) | (((uint16_t)tmsr ) & 0x00FF) ;
  else tmem =  (tmem & 0x00FF) | (((uint16_t)tmsr) << 8) ;
  WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE)),tmem);
}

uint8_t  W5300Class::getTMSR(uint8_t sn)
{
  if(sn & 0x01)
  	return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE))) & 0x00FF);
  return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE))) >> 8);
}
