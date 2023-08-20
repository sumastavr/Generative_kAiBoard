/* Copyright 2018 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>
#include "Ethernet.h"
//#include "utility/w5100.h" //TODO: path Check
#include "w5100.h"

#if ARDUINO >= 156 && !defined(ARDUINO_ARCH_PIC32)
extern void yield(void);
#else
#define yield()
#endif

//socket.h START
#define SOCKET                uint8_t  ///< SOCKET type define for legacy driver

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

#define SOCK_ERROR            0
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)     ///< Socket is not initialized or SIPR is Zero IP address when Sn_MR_TCP
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG           (SOCK_ERROR - 10)    ///< Invalid argument.
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)    ///< Invalid IP address
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)    ///< Data length is zero or greater than buffer max size.
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.

#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.

#define SF_TCP_ALIGN		     0x02			   ///< Valid only \ref Sn_MR_TCP and W5300, refer to \ref Sn_MR_ALIGN
#define SF_IO_NONBLOCK           0x01              ///< Socket nonblock io mode. It used parameter in \ref socket().


/*
 * UDP & MACRAW Packet Infomation
 */
#define PACK_FIRST               0x80              ///< In Non-TCP packet, It indicates to start receiving a packet. (When W5300, This flag can be applied)
#define PACK_REMAINED            0x01              ///< In Non-TCP packet, It indicates to remain a packet to be received. (When W5300, This flag can be applied)
#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet. (When W5300, This flag can be applied)
//A20150601 : For Integrating with W5300
#define PACK_FIFOBYTE            0x02              ///< Valid only W5300, It indicate to have read already the Sn_RX_FIFOR.

//socket.h  END

// TODO: randomize this when not using DHCP, but how?
static uint16_t local_port = 49152;  // 49152 to 65535


//W5300 TODO: Erase
//USed SocketPeek() --> UDP
typedef struct {
	uint16_t RX_RSR; // Number of bytes received
	uint16_t RX_RD;  // Address to read
	uint16_t TX_FSR; // Free space ready for transmit
	uint8_t RX_inc;  // how much have we advanced RX_RD
} socketstate_t;
//W5300 TODO: add sock_remained_size, sock_is_sending, sock_any_port

static socketstate_t state[MAX_SOCK_NUM];


static uint32_t getSockTX_FSR(uint8_t s);
static uint16_t getSockRX_RSR(uint8_t s);
static void write_data(uint8_t s, uint16_t offset, const uint8_t *data, uint16_t len);
static void read_data(uint8_t s,uint8_t *dst, uint16_t len);  //W5300
//static void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len);


/*****************************************/
/*          Socket management            */
/*****************************************/


void EthernetClass::socketPortRand(uint16_t n)
{
	n &= 0x3FFF;
	local_port ^= n;
}

//W5300, ioLibrary's Socket Func
uint8_t EthernetClass::socketBegin(uint8_t protocol, uint16_t port)
{
	uint8_t s, stat, status[MAX_SOCK_NUM], chip, maxindex=MAX_SOCK_NUM;
#if 0  //W5300 ioLibrary socket() START
	switch(protocol)
	{
      case Sn_MR_TCP :
         {

            uint32_t taddr;
            W5300.getSIPR((uint8_t*)&taddr);
            if(taddr == 0) return SOCKERR_SOCKINIT;
	    break;
         }
      case Sn_MR_UDP :
      case Sn_MR_MACRAW :
	  case Sn_MR_IPRAW :
         break;
   #if ( _WIZCHIP_ < 5200 )
      case Sn_MR_PPPoE :
         break;
   #endif
      default :
         return SOCKERR_SOCKMODE;
	}
#endif  //W5300 ioLibrary socket() END
//ARDUINO codes START to read socket START
	// first check hardware compatibility
	chip = W5300.getChipId();
	if (!chip) return MAX_SOCK_NUM; // immediate error if no hardware detected

	for (s=0; s < maxindex; s++) {
		status[s] = W5300.getSnSR(s);
		//W5300, Debug
	
		if (status[s] == SnSR::CLOSED) goto makesocket;  //W5300 Debug status[s] is 0x01 Wrong Values
	}
	for (s=0; s < maxindex; s++) {
		stat = status[s];
		if (stat == SnSR::LAST_ACK) goto closemakesocket;
		if (stat == SnSR::TIME_WAIT) goto closemakesocket;
		if (stat == SnSR::FIN_WAIT) goto closemakesocket;
		if (stat == SnSR::CLOSING) goto closemakesocket;
	}
//ARDUINO codes START to read socket END
closemakesocket:
	W5300.execCmdSn(s, Sn_CR_CLOSE);  //ARDUINO: Sock_CLOSE

makesocket:
	EthernetServer::server_port[s] = 0;
	uint8_t flag = 0x00; //Temp for ioLibrary,  SF_IO_NONBLOCK or Sn_MR_MULTI
	W5300.setSnMR(s, ((uint16_t)(protocol | (flag & 0xF0))) | (((uint16_t)(flag & 0x02)) << 7) );
	W5300.setSnIR(s, 0xFF);

//W5300 ioLibrary START
	if(!port)
	{
	   port = W5300.sock_any_port++;
	   if(W5300.sock_any_port == 0xFFF0) W5300.sock_any_port = SOCK_ANY_PORT_NUM;
	}
	W5300.setSnPORT(s,port);
	W5300.execCmdSn(s,Sn_CR_OPEN);

	//A20150401 : For release the previous sock_io_mode
	W5300.sock_io_mode &= ~(1 <<s);
	W5300.sock_io_mode |= ((flag & SF_IO_NONBLOCK) << s);
	W5300.sock_is_sending &= ~(1<<s);
	W5300.sock_remained_size[s] = 0;
	W5300.sock_pack_info[s] = PACK_COMPLETED;

	while(W5300.getSnSR(s) == SOCK_CLOSED); //TODO: Temp. blocked, check if Blocking code
//W5300 ioLibrary END

	return (int8_t)s;
}


//TODO: W5300 Impl
// multicast version to set fields before open  thd
uint8_t EthernetClass::socketBeginMulticast(uint8_t protocol, IPAddress ip, uint16_t port)
{
	uint8_t s, status[MAX_SOCK_NUM], chip, maxindex=MAX_SOCK_NUM;

	// first check hardware compatibility
	chip = W5300.getChipId();
	if (!chip) return MAX_SOCK_NUM; // immediate error if no hardware detected
	//Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);

	// look at all the hardware sockets, use any that are closed (unused)
	for (s=0; s < maxindex; s++) {
		status[s] = W5300.getSnSR(s);
		if (status[s] == SnSR::CLOSED) goto makesocket;
	}
	//Serial.printf("W5000socket step2\n");
	// as a last resort, forcibly close any already closing
	for (s=0; s < maxindex; s++) {
		uint8_t stat = status[s];
		if (stat == SnSR::LAST_ACK) goto closemakesocket;
		if (stat == SnSR::TIME_WAIT) goto closemakesocket;
		if (stat == SnSR::FIN_WAIT) goto closemakesocket;
		if (stat == SnSR::CLOSING) goto closemakesocket;
	}
#if 0 //check CLOSE WAIT to socket close
	Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
	return MAX_SOCK_NUM; // all sockets are in use
closemakesocket:
	//Serial.printf("W5000socket close\n");
	W5300.execCmdSn(s, Sn_CR_CLOSE);  //ARDUINO: Sock_CLOSE
makesocket:
	//Serial.printf("W5000socket %d\n", s);
	EthernetServer::server_port[s] = 0;
	delayMicroseconds(250); // TODO: is this needed??
	W5300.setSnMR(s, protocol);
	W5300.setSnIR(s, 0xFF);
	if (port > 0) {
		W5300.setSnPORT(s, port);
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		W5300.setSnPORT(s, local_port);
	}
	// Calculate MAC address from Multicast IP Address
	byte mac[] = {  0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };
	mac[3] = ip[1] & 0x7F;
	mac[4] = ip[2];
	mac[5] = ip[3];
	W5300.setSnDIPR(s, ip.raw_address());   //239.255.0.1
	W5300.setSnDPORT(s, port);
	W5300.setSnDHAR(s, mac);
	W5300.execCmdSn(s, Sn_CR_OPEN);  //ARDUINO Sock_OPEN
#if 0  //ARDUINO code
	state[s].RX_RSR = 0;
	state[s].RX_RD  = W5300.getSnRX_RD(s); //TODO: W5300 Not Used
	state[s].RX_inc = 0;
	state[s].TX_FSR = 0;
#endif
	//Serial.printf("W5000 socket prot=%d, RX_RD=%d\n", W5100.readSnMR(s), state[s].RX_RD);
	return s;
}


// Return the socket's status
//
uint8_t EthernetClass::socketStatus(uint8_t s)
{
	uint8_t status = W5300.getSnSR(s);
	//Serial.printf("W5300 socket status =0x%02X\n", status);
	return status;
}


// Immediately close.  If a TCP connection is established, the
// remote host is left unaware we closed.
//
void EthernetClass::socketClose(uint8_t s)
{
   if( ((W5300.getSnMR(s)& 0x0F) == Sn_MR_TCP) && (W5300.getSnTX_FSR(s) != W5300.getSnTxMAX(s)) )
   {
      uint8_t destip[4] = {0, 0, 0, 1};

      W5300.setSnMR(s,Sn_MR_UDP);
      W5300.setSnPORTR(s, 0x3000);
      W5300.setSnCR(s,Sn_CR_OPEN);
      while(W5300.getSnCR(s) != 0);
      while(W5300.getSnSR(s) != SOCK_UDP);

    //TODO: check sendto is needed
    //sendto(s,destip,1,destip,0x3000); // send the dummy data to an unknown destination(0.0.0.1).
   }

	W5300.execCmdSn(s, Sn_CR_CLOSE); //ARDUINO value: Sock_CLOSE

	/* clear all interrupt of the socket. */
	W5300.setSnIR(s, 0xFF);

	//A20150401 : Release the sock_io_mode of socket n.
	W5300.sock_io_mode &= ~(1<<s);

	W5300.sock_is_sending &= ~(1<<s);
	W5300.sock_remained_size[s] = 0;
	W5300.sock_pack_info[s] = 0;
	while(W5300.getSnSR(s) != SOCK_CLOSED);  //TODO: W5300, check Sn_SR and Sn_CR
}


// Place the socket in listening (server) mode
//
uint8_t EthernetClass::socketListen(uint8_t s)
{
	if (W5300.getSnSR(s) != SnSR::INIT) {
		//Serial.printf("EthernetClass::socketListen(), readSnSR is INIT(13)=0x%02X\r\n", W5300.getSnSR(s));
		return 0;
	}

	W5300.execCmdSn(s, Sock_LISTEN);
	while(W5300.getSnSR(s) != SOCK_LISTEN)
	{
			socketClose(s);
			return SOCKERR_SOCKCLOSED;
	}
	return SOCK_OK;
}


// establish a TCP connection in Active (client) mode.
//
uint8_t EthernetClass::socketConnect(uint8_t s, uint8_t * addr, uint16_t port)
{
	// set destination IP
	W5300.setSnDIPR(s,addr);
	W5300.setSnDPORT(s, port);
	W5300.execCmdSn(s, Sn_CR_CONNECT);

	if(W5300.sock_io_mode & (1<<s))
	{
		return SOCK_BUSY;
	}

	while(W5300.getSnSR(s) != SOCK_ESTABLISHED)
	{
		if (W5300.getSnIR(s) & Sn_IR_TIMEOUT)
		{
			W5300.setSnIR(s, Sn_IR_TIMEOUT);
			return SOCKERR_TIMEOUT;
		}
		if (W5300.getSnSR(s) == SOCK_CLOSED)
		{
			return SOCKERR_SOCKCLOSED;
		}
	}
	return SOCK_OK;
}


// Gracefully disconnect a TCP connection.
//
uint8_t EthernetClass::socketDisconnect(uint8_t s)
{
	W5300.execCmdSn(s, Sock_DISCON);  //Sn_CR_DISCON

	W5300.sock_is_sending &= ~(1<<s);
	if(W5300.sock_io_mode & (1<<s)) return SOCK_BUSY;
	while(W5300.getSnSR(s) != SOCK_CLOSED)
	{
		if(W5300.getSnIR(s) & Sn_IR_TIMEOUT)
		{
			socketClose(s);
			return SOCKERR_TIMEOUT;
		}
	}
	return SOCK_OK;
}


/*****************************************/
/*    Socket Data Receive Functions      */
/*****************************************/

//TODO: Debug, W5300 Modified
static uint16_t getSockRX_RSR(uint8_t s)
{
  uint16_t rx_size = W5300.getSnRX_RSR(s);
  return rx_size;
}

//static void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len)
static void read_data(uint8_t s, uint8_t *buf, uint16_t len)
{
  uint16_t rd = 0;
  uint32_t i = 0;

  if(len == 0) return;
  for(i = 0; i < len; i++)
  {
     if((i & 0x01)==0)
     {
        rd = W5300.getSnRX_FIFOR(s); //TODO: Check Data recv, Addr: 0x230 + socketOffset(N * 40)
        //Serial.printf("READ:: socketRecv(): read_data --> W5300.getSnRX_FIFOR(%d) = %d, 0x%04X, \n", s, rd, rd);
        buf[i] = (uint8_t)(rd >> 8);
     }
     else  buf[i] = (uint8_t)rd;  // For checking the memory access violation
     //Serial.printf("READ:: socketRecv(): read_data --> buf[%d] = %d, \n", i, buf[i]);
  }
  W5300.sock_remained_byte[s] = (uint8_t)rd; // back up the remaind fifo byte.
}


// Receive data.  Returns size, or -1 for no data, or 0 if connection closed
int EthernetClass::socketRecv(uint8_t sn, uint8_t *buf, int16_t len)
{
	uint8_t  tmp = 0;
	uint32_t recvsize = 0;
	uint16_t mr = 0;
	uint8_t head[2];

	memset(buf, 0x00, len);

	recvsize = W5300.getSnRxMAX(sn);
	if(recvsize < len) len = recvsize;

	if(W5300.sock_remained_size[sn] == 0)
	{
		while(1)
		{
			recvsize = getSockRX_RSR(sn);
			tmp = W5300.getSnSR(sn); // == 0x22 SOCK_UDP
			//Serial.printf("Line: %d, socketRecv(%d): recvsize =%d, status = %d\n", __LINE__,  sn, recvsize, tmp);

			if (tmp != SOCK_ESTABLISHED)
			{
				if(tmp == SOCK_CLOSE_WAIT) {
					if(recvsize != 0) break;
					// FSR= Free space ready for transmit
					else if(W5300.getSnTX_FSR(sn) == W5300.getSnTxMAX(sn)) {
						socketClose(sn);
						//Serial.printf("socketRecv(): Free space is not ready for transmit(%d) \n", getSockTX_FSR(sn));
						return SOCKERR_SOCKSTATUS;
					}
				}
				else {
					Serial.print("socketRecv() --> invalid status  \n");
					socketClose(sn);
						return SOCKERR_SOCKSTATUS;
				}
			}
			if((W5300.sock_io_mode & (1<<sn)) && (recvsize == 0)) {
				Serial3.print("socketRecv() --> sock busy \n");
				return SOCK_BUSY;
			}
			if(recvsize != 0) break;
		};
	}
	if((W5300.sock_remained_size[sn] == 0) || (W5300.getSnMR(sn) & Sn_MR_ALIGN))
	{
		mr = W5300.getMR();
		if((W5300.getSnMR(sn) & Sn_MR_ALIGN)==0)
		{
			read_data(sn,head,2);
			if(mr & MR_FS) {
				recvsize = (((uint16_t)head[1]) << 8) | ((uint16_t)head[0]);
			}
			else {
				recvsize = (((uint16_t)head[0]) << 8) | ((uint16_t)head[1]);
			}
			W5300.sock_pack_info[sn] = PACK_FIRST;
		}
		W5300.sock_remained_size[sn] = recvsize;
	}
	if(len > W5300.sock_remained_size[sn]) len = W5300.sock_remained_size[sn];

	recvsize = len;
	//Serial.printf("EthernetClass::socketRecv(), (received size) = %d \n", len);

	if(W5300.sock_pack_info[sn] & PACK_FIFOBYTE) //PACK_FIFOBYTE indicates to have read already the Sn_RX_FOFOR
	{
		*buf = W5300.sock_remained_byte[sn];
		buf++;
		W5300.sock_pack_info[sn] &= ~(PACK_FIFOBYTE);
		recvsize -= 1;
		W5300.sock_remained_size[sn] -= 1;
		//Serial.printf("EthernetClass::socketRecv(), PACK_FIFOBYTE(received size) = %d \n", recvsize);
	}

	if(recvsize != 0)
	{
		read_data(sn, buf, recvsize);
		//W5300.execCmdSn(sn, Sn_CR_RECV);
			W5300.setSnCR(sn,Sn_CR_RECV);
			while(W5300.getSnCR(sn));
	}
	W5300.sock_remained_size[sn] -= recvsize;
	if(W5300.sock_remained_size[sn] != 0)
	{
		W5300.sock_pack_info[sn] |= PACK_REMAINED;
		if(recvsize & 0x1) W5300.sock_pack_info[sn] |= PACK_FIFOBYTE;
	}
	else W5300.sock_pack_info[sn] = PACK_COMPLETED;
	if(W5300.getSnMR(sn) & Sn_MR_ALIGN) {
		W5300.sock_remained_size[sn] = 0;
	}
	//Serial.printf("socketRecv, len = %d\n", len);
	return len;
}


int EthernetClass::socketRecvUDP(uint8_t sn, uint8_t *buf, int16_t len) //socck
{
	uint16_t mr;
	uint16_t mr1;

	uint8_t  head[8];
	uint16_t pack_len=0;

	mr1 = W5300.getMR();
	mr = W5300.getSnMR(sn);

	if(W5300.sock_remained_size[sn] == 0){
		while(1){
			//pack_len = getSn_RX_RSR(sn); // org
			//pack_len = W5300.getSnRX_RSR(sn); // ?? why?
			pack_len = getSockRX_RSR(sn);
			if(W5300.getSnSR(sn) == SOCK_CLOSED) return SOCKERR_SOCKCLOSED;
			if( (W5300.sock_io_mode & (1<<sn)) && (pack_len == 0) ) return SOCK_BUSY;
			if(pack_len != 0) break;
		}
	}

    //parse ip address  and port number.
	if(W5300.sock_remained_size[sn] == 0)
	{
		//Serial.printf("sock_remained_size[%d], Read Head(ip, port, remained\n", sn);
		//read_data(sn, head, 8);  //Origin
		read_data(sn, buf, 8);
		//W5300.setSnCR(sn,Sn_CR_RECV);
		//while(W5300.getSnCR(sn));
		// read peer's IP address, port number & packet length

		if(mr1 & MR_FS)  /**< FIFO swap bit of \ref MR. Swap MSB & LSB of \ref Sn_TX_FIFOR & Sn_RX_FIFOR (0 : No swap, 1 : Swap) */
		{

			W5300.sock_remained_size[sn] = buf[7];

			W5300.sock_remained_size[sn] = (W5300.sock_remained_size[sn] << 8) + buf[6];
		}
		else
		{

			W5300.sock_remained_size[sn] = buf[6];

			W5300.sock_remained_size[sn] = (W5300.sock_remained_size[sn] << 8) + buf[7];
		}
		W5300.sock_pack_info[sn] = PACK_FIRST;

		return W5300.sock_remained_size[sn];
	}
	//HEADER_END

	if(len < W5300.sock_remained_size[sn]) pack_len = len;
	else pack_len = W5300.sock_remained_size[sn];
	len = pack_len;
	//Serial.printf("%d: socketRecvUDP(), recv len is %d \r\n ", __LINE__, len);
#if 0
	if(W5300.sock_pack_info[sn] & PACK_FIFOBYTE){
		*buf++ = W5300.sock_remained_byte[sn];
		pack_len -= 1;
		W5300.sock_remained_size[sn] -= 1;
		W5300.sock_pack_info[sn] &= ~PACK_FIFOBYTE;
	}
#endif

    //READ
	if(len != 0)
	{
		read_data(sn, buf, pack_len); // data copy.
		//Serial3.printf("recv Data is [%s]", buf);
		W5300.setSnCR(sn,Sn_CR_RECV);
		/* wait to process the command... */
		while(W5300.getSnCR(sn)) ;
	}

	W5300.sock_remained_size[sn] -= pack_len;
	if(W5300.sock_remained_size[sn] != 0)
	{
		W5300.sock_pack_info[sn] |= PACK_REMAINED;
		//printf("sock_pack_info");
		if(pack_len & 0x01) W5300.sock_pack_info[sn] |= PACK_FIFOBYTE;
	}
	else W5300.sock_pack_info[sn] = PACK_COMPLETED;

	pack_len = len;

	return (int32_t)pack_len;
}


uint32_t EthernetClass::socketRecvAvailable(uint8_t s)
{
	uint32_t rsr = getSockRX_RSR(s);
	//Serial.printf("EthernetClass::socketRecvAvailable(), getSockRX_RSR(%d) = %d \n", s, rsr);
	return rsr;
}

// get the first byte in the receive queue (no checking)
//
uint8_t EthernetClass::socketPeek(uint8_t s)
{
	uint16_t b;
	uint16_t ptr = state[s].RX_RD;
	W5300.read((ptr & W5300.SMASK) + W5300.RBASE(s), &b, 1);
	return b;
}


/*****************************************/
/*    Socket Data Transmit Functions     */
/*****************************************/

static uint32_t getSockTX_FSR(uint8_t s)
{
  uint32_t val, prev;

  prev = W5300.getSnTX_FSR(s);
  while (1) {
    val = W5300.getSnTX_FSR(s);
    if (val == prev) {
      state[s].TX_FSR = val;
      return val;
    }
    prev = val;
  }
}


static void setsockTX_WRSR(uint8_t s, uint16_t txwrs)
{
  W5300.setSnTX_WRSR(s, txwrs);
}


static void write_data(uint8_t s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
  uint32_t i = 0;
  if(len == 0)  return;

  for(i = 0; i < len ; i += 2)
     W5300.setSnTX_FIFOR(s, (((uint16_t)data[i]) << 8) | (((uint16_t)data[i+1]) & 0x00FF));
}


/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t EthernetClass::socketSend(uint8_t sn, const uint8_t * buf, uint16_t len)
{
//ioLibrary
  uint8_t tmp=0;
  uint16_t freesize=0;

  //CHECK_SOCKNUM();
  //CHECK_SOCKMODE(Sn_MR_TCP);
  //CHECK_SOCKDATA();
  tmp = W5300.getSnSR(sn);
  if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) return SOCKERR_SOCKSTATUS;
  if( W5300.sock_is_sending & (1<<sn) )
  {
     tmp = W5300.getSnIR(sn);
     if(tmp & Sn_IR_SENDOK)
     {
        W5300.setSnIR(sn, Sn_IR_SENDOK);
        W5300.sock_is_sending &= ~(1<<sn);
     }
     else if(tmp & Sn_IR_TIMEOUT)
     {
        socketClose(sn);
        return SOCKERR_TIMEOUT;
     }
     else return SOCK_BUSY;
  }
  freesize = W5300.getSnTxMAX(sn);
  if (len > freesize) len = freesize; // check size not to exceed MAX size.
  while(1)
  {
     freesize = W5300.getSnTX_FSR(sn);
     tmp = W5300.getSnSR(sn);
     if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
     {
        socketClose(sn);
        return SOCKERR_SOCKSTATUS;
     }
     if( (W5300.sock_io_mode & (1<<sn)) && (len > freesize) ) return SOCK_BUSY;
     if(len <= freesize) break;
  }
  write_data(sn, 0, (uint8_t *)buf, len);

  W5300.setSnTX_WRSR(sn,len);
  W5300.execCmdSn(sn,Sn_CR_SEND);
	//setSn_CR(sn,Sn_CR_SEND);
	/* wait to process the command... */
	//while(getSn_CR(sn));

  W5300.sock_is_sending |= (1 << sn);
  //M20150409 : Explicit Type Casting
  //return len;
  return (int32_t)len;
}



uint16_t EthernetClass::socketSendAvailable(uint8_t s)
{
	uint8_t status=0;
	uint16_t freesize=0;
	freesize = getSockTX_FSR(s);
	status = W5300.getSnSR(s);
	if ((status == SnSR::ESTABLISHED) || (status == SnSR::CLOSE_WAIT)) {
		return freesize;
	}
	return 0;
}

//W5300, Called by UDP
uint16_t EthernetClass::socketBufferData(uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len)
{

	uint16_t ret =0;
	uint32_t txfree = W5300.getSnTX_FSR(s);
	if (len > txfree) {
		ret = txfree; // check size not to exceed MAX size.
	} else {
		ret = len;
	}
	write_data(s, offset, buf, ret);
	return ret;
}

bool EthernetClass::socketStartUDP(uint8_t s, uint8_t* addr, uint16_t port)
{
	if ( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
	  ((port == 0x00)) ) {
		return false;
	}
	W5300.setSnDIPR(s, addr);
	W5300.setSnDPORT(s, port);
	return true;
}

bool EthernetClass::socketSendUDP(uint8_t s)
{
	//W5300 only START
	W5300.setSnTX_WRSR(s, W5300.udp_send_packet_len);  //Opt udp_send_packet_len for Class type
	W5300.execCmdSn(s, Sock_SEND); //Sn_CR_SEND

	/* +2008.01 bj */
	while ( (W5300.getSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) {
		if (W5300.getSnIR(s) & SnIR::TIMEOUT) {
			/* +2008.01 [bj]: clear interrupt */
			W5300.setSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
			return false;
		}
		yield();
	}

	/* +2008.01 bj */
	W5300.setSnIR(s, SnIR::SEND_OK);
	W5300.udp_send_packet_len = 0;

	/* Sent ok */
	return true;
}

