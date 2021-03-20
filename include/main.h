#ifndef _MAIN_H
#define _MAIN_H

#include <Arduino.h>
#include <myDumper.h>
#include <nikon.h>
//#include <psvaluetitles.h>
//#include <main.h>
#include <ptp.h>
#include <ptpdebug.h>
#include <ptpdpparser.h>
#include <struct_list.h>
#include <usbhub.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#include "devpropparser.h"

class Nikon : public NikonDSLR {
    uint32_t nextPollTime;  // Time of the next poll to occure

   public:
    bool bPollEnabled;  // Enables or disables camera poll
    int incrementPool = 0;

    Nikon(USB *pusb, PTPStateHandlers *pstates) : NikonDSLR(pusb, pstates), nextPollTime(0), bPollEnabled(false){};

    virtual uint8_t Poll();
};

extern valueNameStructUint16 LIST_ATPERTURE[17];
extern valueNameStructUint8 LIST_COMPRESSION[5];
extern valueNameStructUint8_19 LIST_IMAGE_FORMAT[3];
extern valueNameStructUint16 LIST_WHITE_BALANCE[8];
extern valueNameStructUint16 LIST_FOCUS_MODE[4];
extern valueNameStructUint16 LIST_EXP_MET_MODE[3];
extern valueNameStructUint32 LIST_EXPOSURE_TIME[54];
extern valueNameStructUint16 LIST_EXP_PROG_MODE[13];
extern valueNameStructUint16 LIST_EXP_INDEX[9];
extern valueNameStructUint16 LIST_STILL_CAPTURE_MODE[6];
extern valueNameStructUint8 LIST_COLOR_SPACE[2];
extern valueNameStructUint16 LIST_EXP_BIAS_COMP[31];
extern valueNameStructUint16 LIST_EVENT[42];

bool getLiveView(Nikon &nikon, BluetoothSerial &SerialBT);
void startLiveView(Nikon &nikon, BluetoothSerial &SerialBT);
void stopLiveView(Nikon &nikon, BluetoothSerial &SerialBT);

void getInfo(Nikon &nikon, BluetoothSerial &SerialBT);

bool setShutterSpeed(Nikon &nikon, BluetoothSerial &SerialBT);
int getShutterSpeed(Nikon &nikon, BluetoothSerial &SerialBT);

bool setAperture(Nikon &nikon, BluetoothSerial &SerialBT);
int getAperture(Nikon &nikon, BluetoothSerial &SerialBT);

bool setIso(Nikon &nikon, BluetoothSerial &SerialBT);
int getIso(Nikon &nikon, BluetoothSerial &SerialBT);

#endif