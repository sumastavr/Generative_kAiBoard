#ifndef	_W5300_DEFINE_H_
#define	_W5300_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _W5300_ 5300

#define _WIZCHIP_IO_BASE_				0x00000000	// for W5300
#define _W5300_IO_BASE_     _WIZCHIP_IO_BASE_

#define W5300_BANK_ADDR ((uint32_t)0x60000000)
#define _W5300_DATA(p) *(__IO uint16_t *)(W5300_BANK_ADDR + (p << 1))

#define _WIZCHIP_SN_BASE_  (0x0200)
#define _WIZCHIP_SN_SIZE_  (0x0040)

// #define _WIZCHIP_IO_TXBUF_ (0x4000) /* Internal Tx buffer address of the iinchip */
// #define _WIZCHIP_IO_RXBUF_ (0x6000) /* Internal Rx buffer address of the iinchip */
#define WIZCHIP_CREG_BLOCK      	      0x00   ///< Common register block
#define WIZCHIP_SREG_BLOCK(N)    	   (_WIZCHIP_SN_BASE_+ _WIZCHIP_SN_SIZE_*N) ///< Socket N register block
#define WIZCHIP_OFFSET_INC(ADDR, N)    (ADDR + N) ///< Increase offset address

#define RMS01R          (_W5300_IO_BASE_ + 0x28)
#define TMS01R          (_W5300_IO_BASE_ + 0x20)

//----------------------------- W5300 Common Registers -----------------------------
#define MR              (_WIZCHIP_IO_BASE_)             //Mode Register address(R/W); MR is used for S/W reset, ping block mode, PPPoE mode and etc.
#define IR              (_W5300_IO_BASE_ + 0x02)
#define _IMR_             (_W5300_IO_BASE_ + 0x04)
#define SHAR            (_W5300_IO_BASE_ + 0x08)            //Source MAC Register address(R/W)
#define GAR             (_W5300_IO_BASE_ + 0x10)            //Gateway IP Register address(R/W)
#define SUBR            (_W5300_IO_BASE_ + 0x14)            //Subnet mask Register address(R/W)
#define SIPR            (_W5300_IO_BASE_ + 0x18)            //Source IP Register address(R/W)
#define _RTR_          (_W5300_IO_BASE_ + 0x1C)             //Timeout register address( 1 is 100us )(R/W)
#define _RCR_           (_W5300_IO_BASE_ + 0x1E)            //Retry count register(R/W)
#define TMS01R          (_W5300_IO_BASE_ + 0x20)
#define TMS23R          (TMS01R + 2)                        ////TX memory size of \c SOCKET 1 & 2
#define TMS45R          (TMS01R + 4)
#define TMS67R          (TMS01R + 6)                        //TX memory size of \c SOCKET 6 & 7
#define TMSR0           TMS01R                              //TX memory size of \c SOCKET 0.
#define TMSR1           (TMSR0 + 1)                         //X memory size of \c SOCKET 1.
#define TMSR2           (TMSR0 + 2)                         //X memory size of \c SOCKET 2.
#define TMSR3           (TMSR0 + 3)
#define TMSR4           (TMSR0 + 4)
#define TMSR5           (TMSR0 + 5)
#define TMSR6           (TMSR0 + 6)
#define TMSR7           (TMSR0 + 7)                         //TX memory size of \c SOCKET 7.
#define RMS01R          (_W5300_IO_BASE_ + 0x28)
#define RMS23R          (RMS01R + 2)                        //RX memory size of \c SOCKET 2 & 3
#define RMS45R          (RMS01R + 4)
#define RMS67R          (RMS01R + 6)                        //RX memory size of \c SOCKET 6 & 7
#define RMSR0           RMS01R                              //RX memory size of \c SOCKET 0.
#define RMSR1           (RMSR0 + 1)                         //RX memory size of \c SOCKET 2.
#define RMSR2           (RMSR0 + 2)                         //RX memory size of \c SOCKET 2.
#define RMSR3           (RMSR0 + 3)
#define RMSR4           (RMSR0 + 4)
#define RMSR5           (RMSR0 + 5)
#define RMSR6           (RMSR0 + 6)                         //RX memory size of \c SOCKET 6.
#define RMSR7           (RMSR0 + 7)                         //RX memory size of \c SOCKET 7.
#define MTYPER          (_W5300_IO_BASE_ + 0x30)            //Memory Type Register; W5300’s 128Kbytes data memory (Internal TX/RX memory) is composed of 16 memory blocks of 8Kbytes.
#define PATR            (_W5300_IO_BASE_ + 0x32)            //@brief PPPoE Authentication Type register, - PAP  : 0xC023,  - CHAP : 0xC223
#define PTIMER          (_W5300_IO_BASE_ + 0x36)            //PPP Link Control Protocol Request Timer Register
#define PMAGICR         (_W5300_IO_BASE_ + 0x38)            //PPP LCP magic number register
#define PSIDR           (_W5300_IO_BASE_ + 0x3C)            //PPPoE session ID register
#define PDHAR           (_W5300_IO_BASE_ + 0x40)            //PPPoE destination hardware address register
#define UIPR            (_W5300_IO_BASE_ + 0x48)            //Unreachable port number register
#define UPORTR          (_W5300_IO_BASE_ + 0x4C)            ///Unreachable port number register
#define FMTUR           (_W5300_IO_BASE_ + 0x4E)
#define Pn_BRDYR(n)     (_W5300_IO_BASE_ + 0x60 + n*4)
#define Pn_BDPTHR(n)    (_W5300_IO_BASE_ + 0x60 + n*4 + 2)      //
#define IDR             (_W5300_IO_BASE_ + 0xFE)
#define VERSION        IDR


//----------------------------- W5300 SOCKET Registers -----------------------------
/**
 * @ingroup Socket_register_group_W5300
 * @brief Socket Mode register(R/W)
 * @details @ref Sn_MR configures the option or protocol type of Socket n.\n\n
 * Each bit of @ref Sn_MR defined as the following.
 * - @ref Sn_MR_ALIGN   : Alignment bit of Sn_MR, Only valid in \ref Sn_MR_TCP. (C0 : Include TCP PACK_INFO, 1 : Not include TCP PACK_INFO)
 * - @ref Sn_MR_MULTI	: Support UDP Multicasting
 * - @ref Sn_MR_MF	   : Enable MAC Filter (0 : Disable, 1 - Enable), When enabled, W5300 can receive only both own and broadcast packet.
 * - @ref Sn_MR_ND		: No Delayed Ack(TCP) flag
 * - @ref Sn_MR_IGMPv 	: IGMP version used <b>in UDP mulitcasting</b>. (0 : Version 2, 1 : Version 2)
 * - <b>PROTOCOL[3:0]</b>
 *
 * - @ref Sn_MR_PPPoE  : PPPoE
 * - @ref Sn_MR_MACRAW	: MAC LAYER RAW SOCK
 * - @ref Sn_MR_IPRAW  : IP LAYER RAW SOCK
 * - @ref Sn_MR_UDP		: UDP
 * - @ref Sn_MR_TCP		: TCP
 * - @ref Sn_MR_CLOSE	: Unused socket
 *  @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x00)
#define Sn_CR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x02)
#define Sn_IMR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x04)
#define Sn_IR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x06)
#define Sn_SSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x08)
#define Sn_SR(n)        Sn_SSR(n)   ///< For Compatible ioLibrary. Refer to @ref Sn_SSR(n)
#define Sn_PORTR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0A)
#define Sn_PORT(n)      Sn_PORTR(n)       ///< For compatible ioLibrary. Refer to @ref Sn_PORTR(n).
#define Sn_DHAR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0C)
#define Sn_DPORTR(n)    (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x12)
#define Sn_DPORT(n)     Sn_DPORTR(n)    ///< For compatible ioLibrary. Refer to \ref Sn_DPORTR.
#define Sn_DIPR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x14)
#define Sn_MSSR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x18)
#define Sn_KPALVTR(n)   (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1A)
#define Sn_PROTOR(n)    Sn_KPALVTR(n)
#define Sn_TOSR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1C)
#define Sn_TOS(n)       Sn_TOSR(n)  ///< For compatible ioLibrary. Refer to Sn_TOSR
#define Sn_TTLR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1E)
#define Sn_TTL(n)       Sn_TTLR(n)  ///< For compatible ioLibrary. Refer to Sn_TTLR

#define Sn_TX_WRSR(n)		(_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x20)
#define Sn_TX_FSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0024)
#define Sn_RX_RSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0028)
#define Sn_FRAGR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x002C)
#define Sn_FRAG(n)         Sn_FRAGR(n)
#define Sn_TX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x2E)
#define Sn_RX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x30)



/************************************/
/* The bit of MR regsiter defintion */
/************************************/
#define MR_DBW             (1 << 15)            /**< Data bus width bit of \ref MR. Read Only. (0 : 8Bit, 1 : 16Bit)*/
#define MR_MPF             (1 << 14)            /**< Mac layer pause frame bit of \ref MR. (0 : Disable, 1 : Enable)*/
#define MR_WDF(X)          ((X & 0x07) << 11)   /**< Write data fetch time bit of  \ref MR. Fetch Data from DATA bus after PLL_CLK * MR_WDF[2:0]*/
#define MR_RDH             (1 << 10)            /**< Read data hold time bit of \ref MR. Hold Data on DATA bus during 2 * PLL_CLK after CS high*/
#define MR_FS              (1 << 8)             /**< FIFO swap bit of \ref MR. Swap MSB & LSB of \ref Sn_TX_FIFOR & Sn_RX_FIFOR (0 : No swap, 1 : Swap) */
#define MR_RST             (1 << 7)             /**< S/W reset bit of \ref MR. (0 : Normal Operation,  1 : Reset (automatically clear after reset))*/
#define MR_MT              (1 << 5)             /**< Memory test bit of \ref MR. (0 : Normal, 1 : Internal Socket memory write & read Test)*/
#define MR_PB              (1 << 4)             /**< Ping block bit of \ref MR. (0 : Unblock, 1 : Block)*/
#define MR_PPPoE           (1 << 3)             /**< PPPoE bit of \ref MR. (0 : No use PPPoE, 1: Use PPPoE)*/
#define MR_DBS             (1 << 2)             /**< Data bus swap of \ref MR. Valid only 16bit mode (0 : No swap, 1 : Swap)*/
#define MR_IND             (1 << 0)             /**< Indirect mode bit of \ref MR. (0 : Direct mode, 1 : Indirect mode) */


/************************************/
/* The bit of IR regsiter definition */
/************************************/
#define IR_IPCF            (1 << 7)             /**< IP conflict bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_DPUR            (1 << 6)             /**< Destination port unreachable bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_PPPT            (1 << 5)             /**< PPPoE terminate bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_FMTU            (1 << 4)             /**< Fragment MTU bit of IR. To clear, Write the bit to '1'. */
#define IR_SnINT(n)        (0x01 << n)          /**< SOCKETn interrupt occurrence bit of \ref IR. To clear, Clear \ref Sn_IR*/

/*****************************************/
/* The bit of Pn_BRDYR regsiter definition*/
/*****************************************/
#define Pn_PEN             (1 << 7)             /**< PIN 'BRDYn' enable bit of Pn_BRDYR. */
#define Pn_MT              (1 << 6)             /**< PIN memory type bit of Pn_BRDYR. */
#define Pn_PPL             (1 << 5)             /**< PIN Polarity bit of Pn_BRDYR. */
#define Pn_SN(n)           ((n & 0x07) << 0)    /**< What socket to monitor. */


/***************************************/
/* The bit of Sn_MR regsiter definition */
/***************************************/
#define Sn_MR_ALIGN        (1 << 8)
#define Sn_MR_MULTI        (1 << 7)
#define Sn_MR_MF           (1 << 6)
#define Sn_MR_IGMPv        (1 << 5)
#define Sn_MR_MC           Sn_MR_IGMPv       ///< For compatible ioLibrary
#define Sn_MR_ND           (1 << 5)
#define Sn_MR_CLOSE        0x00
#define Sn_MR_TCP          0x01
#define Sn_MR_UDP          0x02                 /**< Protocol bits of \ref Sn_MR. */
#define Sn_MR_IPRAW        0x03                 /**< Protocol bits of \ref Sn_MR. */
#define Sn_MR_MACRAW       0x04
#define Sn_MR_PPPoE        0x05                 /**< Protocol bits of \ref Sn_MR. */

#define SOCK_STREAM        Sn_MR_TCP            /**< For Berkeley Socket API, Refer to @ref Sn_MR_TCP */
#define SOCK_DGRAM         Sn_MR_UDP            /**< For Berkeley Socket API, Refer to @ref Sn_MR_UDP */



/******************************/
/* The values of CR definition */
/******************************/
/**
 * @brief Initialize or open a socket
 * @details Socket n is initialized and opened according to the protocol selected in Sn_MR(P3:P0).
 * The table below shows the value of @ref Sn_SR corresponding to @ref Sn_MR.\n
 * <table>
 *   <tr>  <td>\b Sn_MR (P[3:0])</td> <td>\b Sn_SR</td>            		 </tr>
 *   <tr>  <td>Sn_MR_CLOSE  (000)</td> <td></td>         	   		 </tr>
 *   <tr>  <td>Sn_MR_TCP  (001)</td> <td>SOCK_INIT (0x13)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_UDP  (010)</td>  <td>SOCK_UDP (0x22)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_IPRAW  (010)</td>  <td>SOCK_IPRAW (0x32)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_MACRAW  (100)</td>  <td>SOCK_MACRAW (0x42)</td>  </tr>
 *   <tr>  <td>Sn_MR_PPPoE  (101)</td>  <td>SOCK_PPPoE (0x5F)</td>  </tr>
 * </table>
 */
#define Sn_CR_OPEN         0x01
#define Sn_CR_LISTEN       0x02
#define Sn_CR_CONNECT      0x04
#define Sn_CR_DISCON       0x08
#define Sn_CR_CLOSE        0x10
#define Sn_CR_SEND         0x20
#define Sn_CR_SEND_MAC     0x21
#define Sn_CR_SEND_KEEP    0x22
#define Sn_CR_RECV         0x40                 /**< RECV command value of \ref Sn_CR */
#define Sn_CR_PCON         0x23                 /**< PPPoE connection begins by transmitting PPPoE discovery packet. Refer to \ref Sn_CR */
#define Sn_CR_PDISCON      0x24                 /**< Closes PPPoE connection. Refer to \ref Sn_CR */
#define Sn_CR_PCR          0x25                 /**< In each phase, it transmits REQ message. Refer to \ref Sn_CR */
#define Sn_CR_PCN          0x26                 /**< In each phase, it transmits NAK message. Refer to \ref Sn_CR */
#define Sn_CR_PCJ          0x27                 /**< In each phase, it transmits REJECT message. Refer to \ref Sn_CR */


/*********************************/
/* The values of Sn_IR definition */
/*********************************/
#define Sn_IR_PRECV        0x80                 /**< It is set in the case that option data which is not supported is received. Refer to \ref Sn_IR */
#define Sn_IR_PFAIL        0x40                 /**< It is set in the case that PAP authentication is failed. Refer to \ref Sn_IR */
#define Sn_IR_PNEXT        0x20                 /**< It is set in the case that the phase is changed during PPPoE connection process. \ref Sn_IR */
#define Sn_IR_SENDOK       0x10                 /**< It is set when SEND command is completed. Refer to \ref Sn_IR */
#define Sn_IR_TIMEOUT      0x08                 /**< It is set when ARPTO or TCPTO is occured.  Refer to \ref Sn_IR */
#define Sn_IR_RECV         0x04                 /**< It is set whenever data is received from a peer.  Refer to \ref Sn_IR */
#define Sn_IR_DISCON       0x02                 /**< It is set when FIN or FIN/ACK packet is received from a peer.  Refer to \ref Sn_IR */
#define Sn_IR_CON          0x01                 /**< It is set one time when the connection is successful and then @ref Sn_SR is changed to @ref SOCK_ESTABLISHED. */

/**********************************/
/* The values of Sn_SSR definition */
/**********************************/
#define SOCK_CLOSED        0x00
#define SOCK_ARP           0x01                 /**< ARP-request is transmitted in order to acquire destination hardware address. */
#define SOCK_INIT          0x13
#define SOCK_LISTEN        0x14
#define SOCK_SYNSENT       0x15
#define SOCK_SYNRECV       0x16
#define SOCK_ESTABLISHED   0x17
#define SOCK_FIN_WAIT      0x18
#define SOCK_CLOSING       0x1A
#define SOCK_TIME_WAIT     0x1B
#define SOCK_CLOSE_WAIT    0x1C
#define SOCK_LAST_ACK      0x1D
#define SOCK_UDP           0x22
#define SOCK_IPRAW         0x32
#define SOCK_MACRAW        0x42                 /**< SOCKET0 is open as MACRAW mode. */
#define SOCK_PPPoE         0x5F                 /**< SOCKET0 is open as PPPoE mode. */

/* IP PROTOCOL */
#define IPPROTO_IP                   0        //< Dummy for IP
#define IPPROTO_ICMP                 1        //< Control message protocol
#define IPPROTO_IGMP                 2        //< Internet group management protocol
#define IPPROTO_GGP                  3        //< Gateway^2 (deprecated)
#define IPPROTO_TCP                  6        //< TCP
#define IPPROTO_PUP                  12       //< PUP
#define IPPROTO_UDP                  17       //< UDP
#define IPPROTO_IDP                  22       //< XNS idp
#define IPPROTO_ND                   77       //< UNOFFICIAL net disk protocol
#define IPPROTO_RAW                  255      //< Raw IP packet


//TODO: Erase
//#define Sn_TX_WRSR(n)	   (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x20)
//#define Sn_TX_FSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0024)
//#define Sn_RX_RSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0028)
//#define Sn_FRAGR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x002C)
//#define Sn_FRAG(n)         Sn_FRAGR(n)
//#define Sn_TX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x2E)
//#define Sn_RX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x30)


#define MAX_SOCK_NUM 4
#define WIZCHIP_WRITE write
#define WIZCHIP_READ read

//W5300(Only) ioLibrary Start
#define _WIZCHIP_SOCK_NUM_   8
#define SOCK_ANY_PORT_NUM  0xC000

#define  setIR(ir)        WIZCHIP_WRITE(IR, ir & 0xF0FF)
#define  getIR()          (WIZCHIP_READ(IR) & 0xF0FF)
#define  setIMR(imr)      WIZCHIP_WRITE(_IMR_, imr & 0xF0FF)
#define  getIMR()         (WIZCHIP_READ(_IMR_) & 0xF0FF)
#define  setSHAR(shar) { \
      WIZCHIP_WRITE(SHAR,                       (((uint16_t)((shar)[0])) << 8) + (((uint16_t)((shar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SHAR,2), (((uint16_t)((shar)[2])) << 8) + (((uint16_t)((shar)[3])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SHAR,4), (((uint16_t)((shar)[4])) << 8) + (((uint16_t)((shar)[5])) & 0x00FF)); \
   }

#define  getSHAR(shar) { \
		(shar)[0] = (uint8_t)(WIZCHIP_READ(SHAR) >> 8); \
		(shar)[1] = (uint8_t)(WIZCHIP_READ(SHAR));      \
		(shar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,2)) >> 8); \
		(shar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,2))); \
		(shar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,4)) >> 8); \
		(shar)[5] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,4))); \
   }

#define  setGAR(gar) { \
      WIZCHIP_WRITE(GAR,                       (((uint16_t)((gar)[0])) << 8) + (((uint16_t)((gar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(GAR,2), (((uint16_t)((gar)[2])) << 8) + (((uint16_t)((gar)[3])) & 0x00FF)); \
   }

#define  getGAR(gar) { \
		(gar)[0] = (uint8_t)(WIZCHIP_READ(GAR) >> 8); \
		(gar)[1] = (uint8_t)(WIZCHIP_READ(GAR));      \
		(gar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(GAR,2)) >> 8); \
		(gar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(GAR,2))); \
   }

#define  setSUBR(subr) { \
      WIZCHIP_WRITE(SUBR,                       (((uint16_t)((subr)[0])) << 8) + (((uint16_t)((subr)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SUBR,2), (((uint16_t)((subr)[2])) << 8) + (((uint16_t)((subr)[3])) & 0x00FF)); \
   }

#define  getSUBR(subr) { \
		(subr)[0] = (uint8_t)(WIZCHIP_READ(SUBR) >> 8); \
		(subr)[1] = (uint8_t)(WIZCHIP_READ(SUBR));      \
		(subr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SUBR,2)) >> 8); \
		(subr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SUBR,2))); \
   }

#define  setSIPR(sipr) { \
      WIZCHIP_WRITE(SIPR,                       (((uint16_t)((sipr)[0])) << 8) + (((uint16_t)((sipr)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SIPR,2), (((uint16_t)((sipr)[2])) << 8) + (((uint16_t)((sipr)[3])) & 0x00FF)); \
   }

#define  getSIPR(sipr) { \
		(sipr)[0] = (uint8_t)(WIZCHIP_READ(SIPR) >> 8); \
		(sipr)[1] = (uint8_t)(WIZCHIP_READ(SIPR));      \
	   (sipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SIPR,2)) >> 8); \
		(sipr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SIPR,2))); \
   }

#define  setRTR(rtr)     		WIZCHIP_WRITE(_RTR_, rtr)
#define  getRTR()       		WIZCHIP_READ(_RTR_)
#define  setRCR(rcr)  	  	WIZCHIP_WRITE(_RCR_, ((uint16_t)rcr)&0x00FF)
#define  getRCR()     	  	((uint8_t)(WIZCHIP_READ(_RCR_) & 0x00FF))
#define setMTYPER(mtype)    WIZCHIP_WRITE(MTYPER, mtype)
#define getMTYPER()         WIZCHIP_READ(MTYPER)
#define getPATR()           WIZCHIP_READ(PATR)
#define setPTIMER(ptimer)   WIZCHIP_WRITE(PTIMER, ((uint16_t)ptimer) & 0x00FF)
#define getPTIMER()     		((uint8_t)(WIZCHIP_READ(PTIMER) & 0x00FF))
#define setPMAGIC(pmagic) 	WIZCHIP_WRITE(PMAGIC, ((uint16_t)pmagic) & 0x00FF)
#define getPMAGIC()     		((uint8_t)(WIZCHIP_READ(PMAGIC) & 0x00FF))
#define getPSIDR()      		WIZCHIP_READ(PSIDR)

#define getPDHAR(pdhar) { \
		(pdhar)[0] = (uint8_t)(WIZCHIP_READ(PDHAR) >> 8); \
		(pdhar)[1] = (uint8_t)(WIZCHIP_READ(PDHAR));      \
		(pdhar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,2)) >> 8); \
		(pdhar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,2))); \
		(pdhar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,4)) >> 8); \
		(pdhar)[5] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,4))); \
   }

#define getUIPR(uipr) { \
		(uipr)[0] = (uint8_t)(WIZCHIP_READ(UIPR) >> 8); \
		(uipr)[1] = (uint8_t)(WIZCHIP_READ(UIPR));      \
		(uipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(UIPR,2)) >> 8); \
		(uipr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(UIPR,2))); \
   }

#define getUPORTR()       	WIZCHIP_READ(UPORTR)
#define getFMTUR()          WIZCHIP_READ(FMTUR)
#define getPn_BRDYR(p)     ((uint8_t)(WIZCHIP_READ(Pn_BRDYR(p)) & 0x00FF))
#define setPn_BRDYR(p, brdyr)    WIZCHIP_WRITE(Pn_BRDYR(p), brdyr & 0x00E7)
#define getPn_BDPTHR(p)     WIZCHIP_READ(Pn_BDPTHR(p))
#define setPn_BDPTHR(p, bdpthr)   WIZCHIP_WRITE(Pn_BDPTHR(p),bdpthr)
//W5X00 ioLibrary End


//W5100 OLD Codes, TODO: Delete
typedef uint8_t SOCKET;

class SnMR {
public:
  static const uint8_t CLOSE  = 0x00;
  static const uint8_t TCP    = 0x21;
  static const uint8_t UDP    = 0x02;
  static const uint8_t IPRAW  = 0x03;
  static const uint8_t MACRAW = 0x04;
  static const uint8_t PPPOE  = 0x05;
  static const uint8_t ND     = 0x20;
  static const uint8_t MULTI  = 0x80;
};

enum SockCMD {
  Sock_OPEN      = 0x01,
  Sock_LISTEN    = 0x02,
  Sock_CONNECT   = 0x04,
  Sock_DISCON    = 0x08,
  Sock_CLOSE     = 0x10,
  Sock_SEND      = 0x20,
  Sock_SEND_MAC  = 0x21,
  Sock_SEND_KEEP = 0x22,
  Sock_RECV      = 0x40
};

class SnIR {
public:
  static const uint8_t SEND_OK = 0x10;
  static const uint8_t TIMEOUT = 0x08;
  static const uint8_t RECV    = 0x04;
  static const uint8_t DISCON  = 0x02;
  static const uint8_t CON     = 0x01;
};

class SnSR {
public:
  static const uint8_t CLOSED      = 0x00;
  static const uint8_t INIT        = 0x13;
  static const uint8_t LISTEN      = 0x14;
  static const uint8_t SYNSENT     = 0x15;
  static const uint8_t SYNRECV     = 0x16;
  static const uint8_t ESTABLISHED = 0x17;
  static const uint8_t FIN_WAIT    = 0x18;
  static const uint8_t CLOSING     = 0x1A;
  static const uint8_t TIME_WAIT   = 0x1B;
  static const uint8_t CLOSE_WAIT  = 0x1C;
  static const uint8_t LAST_ACK    = 0x1D;
  static const uint8_t UDP         = 0x22;
  static const uint8_t IPRAW       = 0x32;
  static const uint8_t MACRAW      = 0x42;
  static const uint8_t PPPOE       = 0x5F;
};

class IPPROTO {
public:
  static const uint8_t IP   = 0;
  static const uint8_t ICMP = 1;
  static const uint8_t IGMP = 2;
  static const uint8_t GGP  = 3;
  static const uint8_t TCP  = 6;
  static const uint8_t PUP  = 12;
  static const uint8_t UDP  = 17;
  static const uint8_t IDP  = 22;
  static const uint8_t ND   = 77;
  static const uint8_t RAW  = 255;
};

enum W5300Linkstatus {
  UNKNOWN,
  LINK_ON,
  LINK_OFF
};


#ifdef __cplusplus
}
#endif

#endif   // _W5300_H_
