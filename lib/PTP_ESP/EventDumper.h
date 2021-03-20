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

#ifndef EVENT_DUMPER_H
#define EVENT_DUMPER_H
#include <Arduino.h>

extern int UsbDEBUGlvl;

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
class EventDumper : public BASE_CLASS {
    uint8_t byteCount;
    OFFSET_TYPE byteTotal;

   public:
    EventDumper() : byteCount(0), byteTotal(0){};

    void Initialize() {
        byteCount = 0;
        byteTotal = 0;
    };

    void Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset);
};

template <class BASE_CLASS, class LEN_TYPE, class OFFSET_TYPE>
void EventDumper<BASE_CLASS, LEN_TYPE, OFFSET_TYPE>::Parse(const LEN_TYPE len, const uint8_t *pbuf, const OFFSET_TYPE &offset __attribute__((unused))) {
    int numberEvent = (len - 14) / 6;
    Serial.print("numberEvent: ");
    Serial.println(numberEvent);

    //int sizeList = sizeof(LIST_EVENT) / sizeof(LIST_EVENT[0]);

    for (LEN_TYPE j = 0; j < numberEvent; j++) {
        Serial.print("Event ");
        Serial.print(j + 1);
        Serial.print(": ");

        uint16_t eventValue = ((uint16_t)pbuf[17 + j * 6] << 8) | pbuf[16 + j * 6];

        Serial.printf("%04x -> ", eventValue);

        /*bool nameFound = false;
        for (int i = 0; i < sizeList; i++) {
            if (LIST_EVENT[i].value == eventValue) {
                Serial.println(LIST_EVENT[i].name);
                nameFound = true;
                break;
            }
        }
        if (!nameFound) {
            Serial.println("Event Undefinned");
        }*/
    }

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

#endif  // MY_DUMPER_H
