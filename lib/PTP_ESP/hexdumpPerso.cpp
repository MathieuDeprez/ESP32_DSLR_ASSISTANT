#include <Arduino.h>

#include "nikon.h"

uint8_t HexPersoDumper::actualPercent = 0;

BluetoothSerial HexPersoDumper::SerialBT;

void HexPersoDumper::Parse(const uint8_t len, const uint8_t *pbuf, const uint32_t &offset, const uint32_t total) {
    
    
    uint8_t actualPercentNew = ((float)(offset + len) / total) * 10;
    if (actualPercentNew != actualPercent) {
        actualPercent = actualPercentNew;
        Serial.print("[");
        for (int i = 0; i < actualPercent; i++) {
            Serial.print("o");
        }
        for (int i = 0; i < 10 - actualPercent; i++) {
            Serial.print("-");
        }
        Serial.println("]");

        if (actualPercent == 10) {
            actualPercent = 0;
        }
    }

    //SerialBT.flush();
    SerialBT.write(pbuf, len);

    //unsigned int flushWrite = SerialBT.flushWrite();
    /*while(SerialBT.flushWrite()>30){

    }*/

    //if (!SerialBT.hasClient()) {
    //    Serial.println("No client, return;");
    //    return;
    //}

    /*while (flushWrite > 30) {
        Serial.print(".");
        Serial.print(flushWrite);
        Serial.print(".");
        flushWrite = SerialBT.flushWrite();
    }*/

    //SerialBT.flush();
    /*Serial.print("Message");
    Serial.print(offset);
    Serial.print("/");
    Serial.println(total);
    if (UsbDEBUGlvl >= 0x80) {  // Fully bypass this block of code if we do not debug.

        for (uint16_t j = 0; j < len; j++, byteCount++, byteTotal++) {
            Serial.printf("%02x ", pbuf[j]);

            if (byteCount == 31) {
                Serial.println();
                byteCount = 0xFF;
            }
        }
        Serial.println();
    }*/
}

void HexPersoDumper::SetBl(BluetoothSerial *serialBt) {
    SerialBT = *serialBt;
    Serial.println("Serial Bluetooth Setted");
}