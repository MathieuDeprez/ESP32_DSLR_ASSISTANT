#include <Arduino.h>

#include "BluetoothSerial.h"
#include "nikon.h"

uint8_t ObjectHandlesDumper::actualPercent = 0;
bool ObjectHandlesDumper::constructing = false;
uint8_t *ObjectHandlesDumper::bufRaw = nullptr;

BluetoothSerial ObjectHandlesDumper::SerialBT;

void ObjectHandlesDumper::Parse(const uint8_t len, const uint8_t *pbuf, const uint32_t &offset, const uint32_t total) {
    if (bufRaw == nullptr && offset == 0) {
        Serial.println("new RawBuf");
        bufRaw = new uint8_t[total];
    } else if (offset == 0) {
        Serial.println("delete old RawBuf");
        delete[] bufRaw;
        bufRaw = nullptr;

        Serial.println("new RawBuf");
        bufRaw = new uint8_t[total];
    }

    for (int i = 0; i < len; i++) {
        bufRaw[i + offset] = pbuf[i];
    }

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

    unsigned long timerSending = millis();
    if (offset + len == total) {
        Serial.println("final message objectHandles !, sending via BT");

        uint8_t objectHandlerLetterBT[1] = {'K'};
        SerialBT.write(objectHandlerLetterBT, 1);
        SerialBT.write(bufRaw, total);

        Serial.printf("total: %d\n", total);

        for (int k = 0; k < total; k++) {
            Serial.printf("%d : %02x\n", k, bufRaw[k]);
        }
        Serial.println();
        delay(500);
        //while (SerialBT.flushWrite() > 0)
        //    ;
        Serial.print("sent in ");
        Serial.print(millis() - timerSending);
        Serial.println("ms");

        Serial.println("delete RawBuf");
        delete[] bufRaw;
        bufRaw = nullptr;
    }
}

void ObjectHandlesDumper::SetBl(BluetoothSerial *serialBt) {
    SerialBT = *serialBt;
    Serial.println("Serial Bluetooth Setted");
}