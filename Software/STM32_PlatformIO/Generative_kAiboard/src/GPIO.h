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

#define PIN_POE_AT_DET        PF7   // Input pin indicating the PoE input power level, if AT compliant PSE is detected, pin pull low

#define PORT_WS_LED_DATA      PD3   
#define PORT_WS_LED_ENABLE    PG2

#define PORT_7S_DS            PB4
#define PORT_7S_SHCP          PB5
#define PORT_7S_STCP          PB3
#define PORT_7S_ENABLE        PB0

#define PORT_MUX_CTRL_S0      PD13
#define PORT_MUX_CTRL_S1      PD12
#define PORT_MUX_CTRL_S2      PD11
#define PORT_MUX_CTRL_S3      PG0

#define PIN_KBD_L_MUX_1       PC2
#define PIN_KBD_L_MUX_2       PC3
#define PIN_KBD_R_MUX_1       PC0
#define PIN_KBD_R_MUX_2       PB13

#define PIN_KBD_L_SPK_1       PB1
#define PIN_KBD_L_SPK_2       PF9
#define PIN_KBD_L_SPK_3       PB2

#define PIN_KBD_R_SPK_1       PB6
#define PIN_KBD_R_SPK_2       PE0
#define PIN_KBD_R_SPK_3       PB11

#define PORT_VIB_MOT_L        PC7
#define PORT_VIB_MOT_R        PC6

#define PORT_LOGO_PWM_R       PA0
#define PORT_LOGO_PWM_G       PA1
#define PORT_LOGO_PWM_B       PA15

#define PIN_GPT_STATUS        PA4

#define PN532_IRQ             PF7
#define PN532_RESET           PA4 

#define BLE_TX_PIN            PA2
#define BLE_RX_PIN            PA3

#define LCD_TX_PIN            PG14
#define LCD_RX_PIN            PG9