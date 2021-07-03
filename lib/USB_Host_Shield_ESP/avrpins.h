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

#undef MAKE_PIN

#endif //_avrpins_h_
