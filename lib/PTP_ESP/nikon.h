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
#if !defined(__NIKON_H__)
#define __NIKON_H__

#include <MyBluetoothCode.h>
#include <MyOpCode.h>
#include <MyPcode.h>
#include <MyRpCode.h>

#include "ptp.h"


class NikonDSLR : public PTP {
   public:
    NikonDSLR(USB *pusb, PTPStateHandlers *s);

    virtual uint8_t Init(uint8_t parent, uint8_t port, bool lowspeed);

    uint16_t Capture();
    uint16_t CaptureInSDRAM();

    uint16_t EventCheck(uint16_t val[6]);
    uint16_t GetLiveViewImageV2(uint8_t *&response, uint32_t &responseLenght);
    uint16_t MoveFocus(uint8_t direction, uint16_t step);
    void waitCameraReady(unsigned long timeout);

    bool getLiveView();

    void startLiveView();
    void stopLiveView();

    void getInfo();

    bool setShutterSpeed();
    int getShutterSpeed();

    bool setAperture();
    int getAperture();

    bool setIso();
    int getIso();

    bool setExpoBias();
    int getExpoBias();

    int getExpositionMode();

    void tryFocus();
    void takeHdr();
    void takeHdr(uint8_t *hdrParams);

    int getFocusMod();
    bool takePhotoAndroid();

    bool moveFocusAndroid();

    bool desactivateAutoFocus();
    bool activateAutoFocus();
    bool setAutoFocus(uint8_t value);

    bool takeTimeLapse(int &timeTimelapse);

    void takeLongExposure();
    void sendObjectHandlesBt(uint8_t *response, uint32_t responseLenght);
    void sendBluetoothData(MyBluetoothCode bluetoothCode, uint8_t *buf, int bufLen);
    bool downloadThumbnail();
    bool downloadJpeg();
    bool downloadJpegHq();
    void sendThumbnailBt(uint8_t *response, uint32_t responseLenght);
    void printEvent(uint16_t *bufEvent);
    void getAndSendListHandles();

    uint16_t TransactionV2(MyOpCode opcode, MyParamBlock myParamBlock, MyDataBlock myDataBlock, uint8_t *&response, uint32_t &responseLenght);

    uint16_t GetObjectHandlesV2(uint32_t storage_id, uint16_t format, uint16_t assoc, uint8_t *&response, uint32_t &responseLenght);
    uint16_t GetObjectInfoV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght);
    uint16_t GetObjectV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght);
    uint16_t GetJpegV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght);
    uint16_t GetJpegHqV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght);
    uint16_t GetThumbV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght);
};

#endif  // __NIKON_H__