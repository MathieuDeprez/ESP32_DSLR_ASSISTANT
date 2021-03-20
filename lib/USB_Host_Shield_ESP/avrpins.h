/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
 */

/* derived from Konstantin Chizhov's AVR port templates */

#if !defined(_usb_h_) || defined(_avrpins_h_)
#error "Never include avrpins.h directly; include Usb.h instead"
#else
#define _avrpins_h_

#if defined(ESP8266) || defined(ESP32)

#define MAKE_PIN(className, pin) \
class className { \
public: \
  static void Set() { \
    digitalWrite(pin, HIGH);\
  } \
  static void Clear() { \
    digitalWrite(pin, LOW); \
  } \
  static void SetDirRead() { \
    pinMode(pin, INPUT); \
  } \
  static void SetDirWrite() { \
    pinMode(pin, OUTPUT); \
  } \
  static uint8_t IsSet() { \
    return digitalRead(pin); \
  } \
};

#if defined(ESP32)

// Workaround strict-aliasing warnings
#ifdef pgm_read_word
#undef pgm_read_word
#endif
#ifdef pgm_read_dword
#undef pgm_read_dword
#endif
#ifdef  pgm_read_float
#undef pgm_read_float
#endif
#ifdef  pgm_read_ptr
#undef pgm_read_ptr
#endif

#define pgm_read_word(addr) ({ \
  typeof(addr) _addr = (addr); \
  *(const unsigned short *)(_addr); \
})
#define pgm_read_dword(addr) ({ \
  typeof(addr) _addr = (addr); \
  *(const unsigned long *)(_addr); \
})
#define pgm_read_float(addr) ({ \
  typeof(addr) _addr = (addr); \
  *(const float *)(_addr); \
})
#define pgm_read_ptr(addr) ({ \
  typeof(addr) _addr = (addr); \
  *(void * const *)(_addr); \
})

// Pinout for ESP32 dev module

MAKE_PIN(P0, 0);
MAKE_PIN(P1, 1); // TX0
MAKE_PIN(P10, 10); // TX1
MAKE_PIN(P3, 3); // RX0
MAKE_PIN(P21, 21); // SDA
MAKE_PIN(P22, 22); // SCL
MAKE_PIN(P19, 19); // MISO
MAKE_PIN(P23, 23); // MOSI
MAKE_PIN(P18, 18); // SCK
MAKE_PIN(P5, 5); // SS
MAKE_PIN(P17, 17); // INT

#endif

#undef MAKE_PIN

// pgm_read_ptr is not defined in the ESP32, so we have to undef the diffinition from version_helper.h
#ifdef pgm_read_pointer
#undef pgm_read_pointer
#endif
#define pgm_read_pointer(p) pgm_read_ptr(p)

#else
#error "Please define board in avrpins.h"

#endif

#endif //_avrpins_h_
