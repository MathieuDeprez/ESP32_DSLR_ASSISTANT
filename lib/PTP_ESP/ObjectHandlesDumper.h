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

#ifndef OBJECT_HANDLES_DUMPER_H
#define OBJECT_HANDLES_DUMPER_H
#include <Arduino.h>

#include "BluetoothSerial.h"
extern int UsbDEBUGlvl;

class ObjectHandlesDumper {
    uint8_t byteCount;
    uint16_t byteTotal;

   public:
    ObjectHandlesDumper() : byteCount(0), byteTotal(0){};
    void SetBl(BluetoothSerial *serialBT);

    void Initialize() {
        byteCount = 0;
        byteTotal = 0;
    };

    void Parse(const uint8_t len, const uint8_t *pbuf, const uint32_t &offset, const uint32_t total);

   private:
    static uint8_t actualPercent;
    static bool constructing;
    static BluetoothSerial SerialBT;
    static uint8_t *bufRaw;
};

#endif  // HEX_PERSO_DUMPER_H
