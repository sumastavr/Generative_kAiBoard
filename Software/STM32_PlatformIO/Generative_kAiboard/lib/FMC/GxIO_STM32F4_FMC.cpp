// created by Jean-Marc Zingg to be the GxIO_STM32F4_FSMC io class for the GxTFT library
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// this is the io class for STM32F407V, STM32F407Z boards with FMSC TFT connector, e.g. for
// https://www.aliexpress.com/item/Free-shipping-STM32F407VET6-development-board-Cortex-M4-STM32-minimum-system-learning-board-ARM-core-board/32618222721.html
//
// e.g. for display with matching connector
// https://www.aliexpress.com/item/3-2-inch-TFT-LCD-screen-with-resistive-touch-screens-ILI9341-display-module/32662835059.html
//
// for pin information see the backside of the TFT, for the data pin to port pin mapping see FSMC pin table STM32F407V doc.
//
// this io class can be used with or adapted to other STM32F407V/Z processor boards with FSMC TFT connector.
//
// this version is for use with Arduino package STM32DUINO, board "STM32 Discovery F407".
// https://github.com/rogerclarkmelbourne/Arduino_STM32

//#if defined(ARDUINO_ARCH_STM32F4) || defined(ARDUINO_ARCH_STM32L4)


//This FMC structure is
//modified from Jean-Marc Zingg's GxIO_STM32F4_FSMC


#include "GxIO_STM32F4_FMC.h"


typedef struct
{
  volatile uint32_t BTCR[8];
} FSMC_Bank1_TypeDef;

#define FSMC_BASE             ((uint32_t)0x60000000) /*!< FSMC base address */
#define FSMC_R_BASE           ((uint32_t)0xA0000000) /*!< FSMC registers base address */

#define FSMC_BANK1            (FSMC_BASE)               /*!< FSMC Bank1 base address */
#define FSMC_BANK1_R_BASE     (FSMC_R_BASE + 0x0000)    /*!< FSMC Bank1 registers base address */
#define FSMC_Bank1            ((FSMC_Bank1_TypeDef *) FSMC_BANK1_R_BASE)

#define CommandAccess FSMC_BANK1
#define DataAccess (FSMC_BANK1 + 0x80000)


GxIO_STM32F4_FSMC::GxIO_STM32F4_FSMC(bool bl_active_high)
{
  _cs   = PD7;  // FSMC_NE1
  _rs   = PD13; // FSMC_A18
  _rst  = 0;    // not available, driven from NRST
  _wr   = PD5;  // FSMC_NWE
  _rd   = PD4;  // FSMC_NOE
  _bl   = PB1;
  _bl_active_high = bl_active_high;
}

void GxIO_STM32F4_FSMC::reset()
{
  // _rst pin not available
}

void GxIO_STM32F4_FSMC::init()
{
  //FROM STM32F429ZI's FMC Register values
  RCC->AHB1ENR   = 0x001010FF;
  volatile uint32_t t = RCC->AHB1ENR; // delay
  //Serial3.printf("RCC->AHB1ENR =  0x%08X\r\n", RCC->AHB1ENR);

  GPIOD->AFR[0]  = 0xC0CC00CC;
  GPIOD->AFR[1]  = 0xCCC00CCC;
  GPIOD->MODER   = 0xA82A8A0A;
  GPIOD->OSPEEDR = 0x54154505;
  GPIOD->OTYPER  = 0x00000000;
  GPIOD->PUPDR   = 0x00000000;

  GPIOE->AFR[0]  = 0xC0000000;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  GPIOE->MODER   = 0xAAAA8000;
  GPIOE->OSPEEDR = 0xFFFFC000;
  GPIOE->OTYPER  = 0x00000000;
  GPIOE->PUPDR   = 0x00000000;

  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  GPIOF->MODER   = 0xAA000AAA;
  GPIOF->OSPEEDR = 0xFF000FFF;
  GPIOF->OTYPER  = 0x00000000;
  GPIOF->PUPDR   = 0x00000000;

  RCC->AHB3ENR         |= 0x00000001;
  t = RCC->AHB1ENR; // delay
  (void)(t);

  /* F407 */
  //FSMC_Bank1->BTCR[0] = 0x000010D9;
  //FSMC_Bank1->BTCR[1] = (DATAST << 8) | ADDSET;
  /* F429ZI */
  FMC_Bank1->BTCR[0] = 0x00001091;
  FMC_Bank1->BTCR[1] = 0x0FFFFFFF;

  digitalWrite(_bl, LOW);
  pinMode(_bl, OUTPUT);

  //FMC and FSMC to be Seperated on LOW Level of Chip Series
}


GxIO_STM32F4_FSMC FMC;