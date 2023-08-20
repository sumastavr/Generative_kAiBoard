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

//This FMC structure is
//modified from Jean-Marc Zingg's GxIO_STM32F4_FSMC


#ifndef _GxIO_STM32DUINO_STM32F4_FSMC_H_
#define _GxIO_STM32DUINO_STM32F4_FSMC_H_

#include <Arduino.h>


class GxIO_STM32F4_FSMC
{
  public:
    GxIO_STM32F4_FSMC(bool bl_active_high = true);
    const char* name = "GxIO_STM32F4_FSMC";
    void reset();
    void init();

  private:
    int8_t _cs, _rs, _rst, _wr, _rd, _bl; // Control lines
    bool _bl_active_high;
};

#define GxIO_Class GxIO_STM32F4_FSMC
extern GxIO_STM32F4_FSMC FMC;

//#endif

#endif
