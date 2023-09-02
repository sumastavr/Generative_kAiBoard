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

#define EN  13 // enter
#define AS  96 // asterisks
#define SH  254 // shift
#define DL  127  
#define BL  92 // back slash
#define BS  8 // backspace
#define VT  11 // vertical tab
#define ES  27 // Escape
#define CT  253 // Control
#define SP  252 // Windows Super
#define AL  251 // ALT

const char caseMux[][4]  = {{'3'  ,'b'  ,'0'  , SH  }, // case 0
                            { SP  ,'g'  ,NULL , EN  }, // case 1
                            {'z'  ,'t'  ,','  ,']'  }, // case 2
                            {'a'  ,'6'  ,'k'  , BS  }, // case 3
                            {'q'  ,'v'  ,'i'  ,' '  }, // case 4
                            {'2'  ,'f'  ,'9'  , AS  }, // case 5
                            { CT  ,'r'  , DL  ,'['  }, // case 6
                            {'|'  ,'5'  ,'m'  ,'+'  }, // case 7
                            {']'  ,'c'  ,'j'  ,'/'  }, // case 8
                            { BL  ,'d'  ,'u'  ,';'  }, // case 9
                            {'1'  ,'e'  ,'8'  ,'p'  }, // case 10
                            { SH  ,'4'  ,' '  ,'-'  }, // case 11
                            {NULL , AL  ,'n'  ,NULL }, // case 12
                            {NULL ,'x'  ,'h'  ,'.'  }, // case 13
                            { VT  ,'s'  ,'y'  ,'l'  }, // case 14
                            { ES  ,'w'  ,'7'  ,'o'  }  // case 15
                            };

const uint8_t digits_7s[10] = {     0b00011000,  // digit 0
                                    0b01111011,  // digit 1
                                    0b00101100,  // digit 2
                                    0b00101001,  // digit 3 
                                    0b01001011,  // digit 4
                                    0b10001001,  // digit 5
                                    0b10001000,  // digit 6
                                    0b00111011,  // digit 7
                                    0b00001000,  // digit 8
                                    0b00001001,  // digit 9
                                };