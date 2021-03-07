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

#ifndef IMAGE_DUMPER_H
#define IMAGE_DUMPER_H
#include <Arduino.h>

#include "BluetoothSerial.h"
extern int UsbDEBUGlvl;

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
class ImageDumper : public BASE_CLASS {
    uint8_t byteCount;
    OFFSET_TYPE byteTotal;
    BluetoothSerial SerialBT;

   public:
    ImageDumper() : byteCount(0), byteTotal(0){};

    void Initialize() {
        byteCount = 0;
        byteTotal = 0;
    };

    void SetBl(BluetoothSerial *serialBT);
    void Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset);
};

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
void ImageDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>::SetBl(BluetoothSerial *serialBt) {
    SerialBT = *serialBt;
    Serial.println("Serial Bluetooth Setted");
}

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
void ImageDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>::Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset __attribute__((unused))) {
    /*for (LEN_TYPE j = 0; j < len; j++, byteCount++, byteTotal++) {
        //PrintHex<uint8_t>(pbuf[j], 0x80);
        Serial.printf("%02x ", pbuf[j]);
        //E_Notify(PSTR(" "), 0x80);

        if (byteCount == 31) {
            Serial.println();
            //E_Notify(PSTR("\r\n"), 0x80);
            byteCount = 0xFF;
        }
    }*/
    //Serial.println("imageDump...");
    Serial.flush();
    SerialBT.flush();
    int writen = SerialBT.write(pbuf, len);
    //Serial.print("len was:");
    //Serial.print(len);
    //Serial.print(" ||  written:");
    //Serial.println(writen);
    //delay(100);
    

    /*if (UsbDEBUGlvl >= 0x80) {  // Fully bypass this block of code if we do not debug.
        for (LEN_TYPE j = 0; j < len; j++, byteCount++, byteTotal++) {
            PrintHex<uint8_t>(pbuf[j], 0x80);
            E_Notify(PSTR(" "), 0x80);

            if (byteCount == 15) {
                E_Notify(PSTR("\r\n"), 0x80);
                byteCount = 0xFF;
            }
        }
    }*/
}

#endif  // IMAGE_DUMPER_H
