/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
#include "eoseventname.h"

// custom parser made to extract a filename from EOS 'Object Created' event
void EOSEventName::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset __attribute__ ((unused)))
{
	Serial.println("PArse 05!");
	uint8_t* p = (uint8_t*)pbuf;
	uint16_t cntdn = (uint16_t)len;

	switch (parseStage) {
            case 0:
		// Get PTP data packet size
		ptppktSize = *((uint32_t*)p);

		// Return if the packet has only one empty record
		if (ptppktSize == 0x14) {
			return;
                }

		// Serial.println("\r\n");

		for (uint8_t i=0; i<4; i++) {
                    // Serial.println("Print Packet Size");
                    // PrintHex<uint8_t>(((uint8_t*)&ptppktSize)[i], 0x80);
                    // Serial.print(" ");
		}
		// Skip PTP packet header
		p += 12;
		cntdn -= 12;
		parseStage++;
            case 1:
		parseSubstage = 0;
		parseStage++;
            case 2:
		while (1) {
			switch (parseSubstage) {
			// CR after each event record
			case 0:
				// Serial.println("");
				valueParser.Initialize(&valueBuffer);
				parseSubstage ++;

			// Parse record size value
			case 1:
				if (!valueParser.Parse(&p, &cntdn)) {
					return;
                                }
				recordSize = (uint32_t)theBuffer;
                                // Serial.println("\r\nPrint Record Size\r\n");
				// for (uint8_t i=0; i<4; i++) {
				//	PrintHex<uint8_t>(((uint8_t*)&theBuffer)[i], 0x80);
				//	Serial.print(" ");
				//}
				recordSize -= 4;
				valueParser.Initialize(&valueBuffer);
				parseSubstage++;
			// Parse the first uint32_t value
			case 2:
				if (!valueParser.Parse(&p, &cntdn)) {
					return;
                                }
                                // Serial.print("\r\nPrint Property Code: ");
				// for (uint8_t i=0; i<4; i++) {
				//	PrintHex<uint8_t>(((uint8_t*)&theBuffer)[i], 0x80);
				//	Serial.print(" ");
				//}
                                // PrintHex<uint32_t>(theBuffer,0x80);
                                if (theBuffer == EOS_EC_ObjectCreated) {
                                    Serial.print("\r\nObject Created: ");
                                    // p += 0x20;
                                    Serial.println((char*)(p+0x20));
                                }
				recordSize -= 4;
				// Return if empty(last) record
				if (recordSize == 0x08 && (uint32_t)theBuffer == 0) {
					parseSubstage = 0;
					parseStage = 0;
					return;
				}
				parseSubstage++;
			case 3:
                                // Serial.println("\r\nPrint Record");
				for (; recordSize && cntdn; recordSize--, cntdn--, p++) {
				//	PrintHex<uint8_t>(*p, 0x80);
				//	Serial.print(" ");
				}
				if (!recordSize) {
					parseSubstage = 0;
                                }
				if (!cntdn) {
					return;
                                }
			} // switch (parseSubstage...
		} // while(1...
	} // switch (parseStage...
}
