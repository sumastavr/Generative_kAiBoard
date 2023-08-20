/*
 * Copyright 2018 Paul Stoffregen
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

// w5100.h contains private W5x00 hardware "driver" level definitions
// which are not meant to be exposed to other libraries or Arduino users

#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include <Arduino.h>
#include "w5300_define.h"
// Safe for all chips

// Safe for W5200 and W5500, but too fast for W5100
// Uncomment this if you know you'll never need W5100 support.
//  Higher SPI clock only results in faster transfer to hosts on a LAN
//  or with very low packet latency.  With ordinary internet latency,
//  the TCP window size & packet loss determine your overall speed.
//#define SPI_ETHERNET_SETTINGS SPISettings(30000000, MSBFIRST, SPI_MODE0)



class W5300Class {

public:
  static uint8_t chip;

  static uint16_t sock_any_port;
  static uint16_t sock_io_mode;
  static uint16_t sock_is_sending;
  uint16_t udp_send_packet_len = 0;  //W5300
  uint8_t sock_remained_byte[_WIZCHIP_SOCK_NUM_]; // set by wiz_recv_data()
  uint16_t sock_remained_size[_WIZCHIP_SOCK_NUM_];
  uint8_t  sock_pack_info[_WIZCHIP_SOCK_NUM_];

  static uint8_t init(void);
  void write_SnRX_SIZE(uint8_t sn, uint8_t size );
  void write_SnTX_SIZE(uint8_t sn, uint8_t size);
  void setup_Ethernet(void);
  void reset_soft(void);
  static uint16_t write(uint16_t addr, const uint16_t *buf, uint16_t len);
  static uint8_t write(uint16_t addr, uint16_t data) {
        //write(addr, &data, 1);
        _W5300_DATA(addr) = data;
        return 1;
    }

  static uint16_t read(uint16_t addr, uint16_t *buf, uint16_t len);
  static uint16_t read(uint16_t addr) {
    //uint16_t data;
    //read(addr, &data, 1);
    //return data;
    return _W5300_DATA(addr);
  }


  static void execCmdSn(uint8_t socket, uint8_t command);

  inline void setGatewayIp(const uint8_t * addr) { setGAR (addr); }
  inline void getGatewayIp(uint8_t * addr) { getGAR(addr); }

  inline void setSubnetMask(const uint8_t * addr) { setSUBR(addr); }
  inline void getSubnetMask(uint8_t * addr) { getSUBR(addr); }

  inline void setMACAddress(const uint8_t * addr) { setSHAR(addr); }
  inline void getMACAddress(uint8_t * addr) { getSHAR(addr); }

  inline void setIPAddress(const uint8_t * addr) { setSIPR(addr); }
  inline void getIPAddress(uint8_t * addr) { getSIPR(addr); }

  inline void setRetransmissionTime(uint16_t timeout) { setRTR(timeout); }
  inline void setRetransmissionCount(uint8_t retry) { setRCR(retry); }

  uint16_t getSnRX_RSR(uint8_t sn);
  uint32_t getSnTX_FSR(uint8_t sn);


  static W5300Linkstatus getLinkStatus();

//OLD codes
  //static uint8_t  chip;
  static uint8_t  CH_BASE_MSB;
  static uint8_t  ss_pin;
  static const uint16_t SSIZE = 2048;
  static const uint16_t SMASK = 0x07FF;


  static void setSS(uint8_t pin) {
    ss_pin = pin;
    //Serial.printf("setSS :-(%d)\n", ss_pin);
  }
//OLD codes End

#define set_MR(mr)         WIZCHIP_WRITE(MR,mr)
#define get_MR()           WIZCHIP_READ(MR)
#define get_IDR()          WIZCHIP_READ(IDR)

/***********************************
 * SOCKET Register Access Function *
 ***********************************/
#define setSn_MR(sn, mr)      WIZCHIP_WRITE(Sn_MR(sn),mr)
#define getSn_MR(sn)        	WIZCHIP_READ(Sn_MR(sn))
#define setSn_CR(sn, cr)      WIZCHIP_WRITE(Sn_CR(sn), ((uint16_t)cr) & 0x00FF)
#define getSn_CR(sn)          ((uint8_t)WIZCHIP_READ(Sn_CR(sn)))
#define setSn_IMR(sn, imr)    WIZCHIP_WRITE(Sn_IMR(sn), ((uint16_t)imr) & 0x00FF)
#define getSn_IMR(sn)         ((uint8_t)WIZCHIP_READ(Sn_IMR(sn)))
#define setSn_IR(sn, ir)      WIZCHIP_WRITE(Sn_IR(sn), ((uint16_t)ir) & 0x00FF)
#define getSn_IR(sn)          ((uint8_t)WIZCHIP_READ(Sn_IR(sn)))
#define getSn_SSR(sn)         ((uint8_t)WIZCHIP_READ(Sn_SR(sn)))
#define getSn_SR(sn)          getSn_SSR(sn)  ///< For compatible ioLibrary. Refer to getSn_SSR().
#define setSn_PORTR(sn, port) WIZCHIP_WRITE(Sn_PORTR(sn), port)
#define setSn_PORT(sn, port)  setSn_PORTR(sn, port)   ///< For compatible ioLibrary
#define getSn_PORTR(sn)       WIZCHIP_READ(Sn_PORTR(sn))
#define getSn_PORT(sn)        getSn_PORTR(sn)   ///< For compatible ioLibrary

#define setSn_DHAR(sn, dhar) { \
      WIZCHIP_WRITE(Sn_DHAR(sn),                       (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2), (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4), (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
   }

#define getSn_DHAR(sn, dhar) { \
      (dhar)[0] = (uint8_t)(WIZCHIP_READ(Sn_DHAR(sn)) >> 8); \
      (dhar)[1] = (uint8_t) WIZCHIP_READ(Sn_DHAR(sn)); \
      (dhar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2)) >> 8); \
      (dhar)[3] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2)); \
      (dhar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4)) >> 8); \
      (dhar)[5] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4)); \
   }

#define setSn_DPORTR(sn, dport)  WIZCHIP_WRITE(Sn_DPORTR(sn),dport)
#define setSn_DPORT(sn, dport)   setSn_DPORTR(sn,dport) ///< For compatible ioLibrary. Refer to @ref Sn_DPORTR.
#define getSn_DPORTR(sn)         WIZCHIP_READ(Sn_DPORTR(sn))
#define getSn_DPORT(sn)          getSn_DPORTR(sn)  ///< For compatible ioLibrary. Refer to @ref Sn_DPORTR.

#define setSn_DIPR(sn, dipr) { \
		WIZCHIP_WRITE(Sn_DIPR(sn),                       (((uint16_t)((dipr)[0])) << 8) + (((uint16_t)((dipr)[1])) & 0x00FF)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2), (((uint16_t)((dipr)[2])) << 8) + (((uint16_t)((dipr)[3])) & 0x00FF)); \
   }

#define getSn_DIPR(sn, dipr) { \
		(dipr)[0] = (uint8_t)(WIZCHIP_READ(Sn_DIPR(sn)) >> 8); \
		(dipr)[1] = (uint8_t) WIZCHIP_READ(Sn_DIPR(sn));  \
		(dipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2)) >> 8); \
		(dipr)[3] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2));  \
   }

#define setSn_MSSR(sn, mss)       WIZCHIP_WRITE(Sn_MSSR(sn), mss)
#define getSn_MSSR(sn)            WIZCHIP_READ(Sn_MSSR(sn))
#define setSn_KPALVTR(sn, kpalvt) WIZCHIP_WRITE(Sn_KPALVTR(sn), (WIZCHIP_READ(Sn_KPALVTR(sn)) & 0x00FF) | (((uint16_t)kpalvt)<<8))
#define getSn_KPALVTR(sn)     		((uint8_t)(WIZCHIP_READ(Sn_KPALVTR(sn)) >> 8))
#define setSn_PROTOR(sn, proto)   WIZCHIP_WRITE(Sn_PROTOR(sn),(WIZCHIP_READ(Sn_PROTOR(sn)) & 0xFF00) | (((uint16_t)proto) & 0x00FF))
#define setSn_PROTO(sn,proto)      setSn_PROTOR(sn,proto)  ///< For compatible ioLibrary
#define getSn_PROTOR(sn)          ((uint8_t)WIZCHIP_READ(Sn_PROTOR(sn)))
#define getSn_PROTO(sn)           getSn_PROTOR(sn)  ///< For compatible ioLibrary

#define  setSn_TX_WRSR(sn, txwrs) { \
      WIZCHIP_WRITE(Sn_TX_WRSR(sn),                       (uint16_t)(((uint32_t)txwrs) >> 16)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_TX_WRSR(sn),2), (uint16_t)txwrs); \
   }

#define  getSn_TX_WRSR(sn)           ( (((uint32_t)WIZCHIP_READ(Sn_TX_WRSR(sn))) << 16) + (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_WRSR(sn),1))) & 0x0000FFFF) )
#define  setSn_TX_FIFOR(sn, txfifo)  WIZCHIP_WRITE(Sn_TX_FIFOR(sn), txfifo);
#define getSn_RX_FIFOR(sn)           WIZCHIP_READ(Sn_RX_FIFOR(sn));
#define setSn_TOSR(sn, tos)          WIZCHIP_WRITE(Sn_TOS(sn), ((uint16_t)tos) & 0x00FF)
#define setSn_TOS(sn,tos)            setSn_TOSR(sn,tos)   ///< For compatible ioLibrar

#define getSn_TOSR(sn)               ((uint8_t)WIZCHIP_READ(Sn_TOSR(sn)))
#define getSn_TOS(sn)                getSn_TOSR(sn)    ///< For compatible ioLibrar

#define setSn_TTLR(sn, ttl)          WIZCHIP_WRITE(Sn_TTLR(sn), ((uint16_t)ttl) & 0x00FF)
#define setSn_TTL(sn,ttl)            setSn_TTLR(sn,ttl)   ///< For compatible ioLibrary

#define getSn_TTLR(sn)               ((uint8_t)WIZCHIP_READ(Sn_TTL(sn)))
#define getSn_TTL(sn)                getSn_TTLR(sn)    ///< For compatible ioLibrary

#define setSn_FRAGR(sn, frag)        WIZCHIP_WRITE(Sn_FRAGR(sn),  ((uint16_t)frag) & 0x00FF)
#define setSn_FRAG(sn,frag)          setSn_FRAGR(sn,flag)
#define getSn_FRAGR(sn)              (WIZCHIP_READ(Sn_FRAG(sn)))
#define getSn_FRAG(sn)               getSn_FRAGR(sn)


/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////
static void setRMSR(uint8_t sn,uint8_t rmsr);
static uint8_t getRMSR(uint8_t sn);
static void setTMSR(uint8_t sn,uint8_t tmsr);
static uint8_t getTMSR(uint8_t sn);

#define setSn_TXBUF_SIZE(sn, tmsr)   setTMSR(sn, tmsr) ///< For compatible ioLibrary
#define getSn_TXBUF_SIZE(sn)         getTMSR(sn) ///< For compatible ioLibrary
#define setSn_RXBUF_SIZE(sn,rmsr)    setRMSR(sn, rmsr)   ///< For compatible ioLibrary
#define getSn_RXBUF_SIZE(sn)         getRMSR(sn)    ///< For compatible ioLibrary

#define getSn_RxMAX(sn)         		(((uint32_t)getSn_RXBUF_SIZE(sn)) << 10)
#define getSn_TxMAX(sn)             (((uint32_t)getSn_TXBUF_SIZE(sn)) << 10)

//Not W5300 Remove Temp
#define getSn_RX_RD(sn)                  (sn)

//W5300 API End

private:


public:
  //TODO: Remove Temp Code
  static uint16_t RBASE(uint8_t socknum) {
  if (chip == 51) {
    return socknum * SSIZE + 0x6000;
  } else {
    return socknum * SSIZE + 0xC000;
  }
  }

  uint8_t getChipId(void) { return (getIDR() >> 8); }

  //Template W5300 class api for sockeet.cpp and ethernet.cpp
  static  void setMR(uint16_t mr)                         { set_MR(mr) ;}
  static  uint16_t getMR()                                { return get_MR() ;}
  static  uint16_t getIDR()                               { return get_IDR(); }
  static  void setSnPORT(uint8_t sn, uint16_t port)       { setSn_PORT(sn, port) ;}
  static  uint16_t getSnPORT(uint8_t sn)                  { return getSn_PORT(sn) ;}
  static  void getSnDIPR(uint8_t sn, uint8_t *dipr)       { getSn_DIPR(sn, dipr) ; }
  static  void setSnDIPR(uint8_t sn, uint8_t *dirp)       { setSn_DIPR(sn, dirp) ; }
  static  void setSnPORTR(uint8_t sn, uint16_t port)      { setSn_PORTR(sn, port);}
  static  void setSnDPORT(uint8_t sn, uint16_t dport)     { setSn_DPORT(sn, dport );}
  static  uint16_t getSnDPORT(uint8_t sn)                 { return getSn_DPORT(sn) ;}

  static  void setSnDHAR(uint8_t sn, uint8_t *  dhar)      { setSn_DHAR(sn, dhar) ;}

  static  void setSnMR(uint8_t sn, uint16_t mr)           { setSn_MR(sn, mr) ;}
  static  uint16_t getSnMR(uint8_t  sn)                   { return getSn_MR(sn) ;}
  static  uint16_t getSnIR(uint8_t sn)                    { return getSn_IR(sn) ;}
  static  void setSnIR(uint8_t sn, uint16_t sck_inst)     { setSn_IR(sn, sck_inst) ;}
  static  void setSnCR(uint8_t sn, uint16_t cr)           { setSn_CR(sn, cr) ;}
  static  uint8_t getSnCR(uint8_t sn)                     { return getSn_CR(sn)  ;}
  static  uint8_t getSnSR(uint8_t sn)                     { return getSn_SR(sn)  ;}

  static  void setSnTX_WRSR(uint8_t sn, uint16_t txwrs)        { setSn_TX_WRSR(sn, txwrs) ;}
  static  void setSnTX_FIFOR(uint8_t sn, uint16_t txfifo) { setSn_TX_FIFOR(sn, txfifo) ;}
  static uint32_t getSnRX_FIFOR(uint8_t sn)               { return getSn_RX_FIFOR(sn) ;}

  static uint32_t getSnTxMAX(uint8_t sn)                  {return getSn_TxMAX(sn) ;}
  static uint32_t getSnRxMAX(uint8_t sn)                  {return getSn_RxMAX(sn) ;}

  //Temp
  uint16_t getSnRX_RD(uint8_t sn)              {return getSn_RX_RD(sn) ;}

};//w5300 class

extern W5300Class W5300;
#endif

#ifndef UTIL_H
#define UTIL_H

#ifndef htons
// The host order of the Arduino platform is little endian.
// Sometimes it is desired to convert to big endian (or
// network order)

// Host to Network short
#define htons(x) ( (((x)&0xFF)<<8) | (((x)>>8)&0xFF) )

// Network to Host short
#define ntohs(x) htons(x)

// Host to Network long
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )

// Network to Host long
#define ntohl(x) htonl(x)

#endif // !defined(htons)

#endif
