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
#include "nikon.h"

#include <D3300.h>

NikonDSLR::NikonDSLR(USB *pusb, PTPStateHandlers *s)
    : PTP(pusb, s) {
}

uint8_t NikonDSLR::Init(uint8_t parent, uint8_t port, bool lowspeed) {
    uint8_t buf[10];
    USB_DEVICE_DESCRIPTOR *udd =
        reinterpret_cast<USB_DEVICE_DESCRIPTOR *>(buf);
    uint8_t rcode;
    UsbDevice *p = NULL;
    EpInfo *oldep_ptr = NULL;

    AddressPool &addrPool = pUsb->GetAddressPool();

    if (devAddress)
        return USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE;

    // Get pointer to pseudo device with address 0 assigned
    p = addrPool.GetUsbDevicePtr(0);

    if (!p)
        return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

    if (!p->epinfo) {
        return USB_ERROR_EPINFO_IS_NULL;
    }

    // Save old pointer to EP_RECORD of address 0
    oldep_ptr = p->epinfo;

    // Temporary assign new pointer to epInfo to p->epinfo in order to avoid toggle inconsistence
    p->epinfo = epInfo;

    // Get device descriptor
    rcode = pUsb->getDevDescr(0, 0, 10, (uint8_t *)buf);

    // Restore p->epinfo
    p->epinfo = oldep_ptr;

    if (rcode) {
        PTPTRACE2("getDevDesc:", rcode);
        return rcode;
    }

    if (udd->idVendor == 0x04B0)
        return PTP::Init(parent, port, lowspeed);
    else {
        PTPTRACE("Camera isn't Nikon\r\n");
        return USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED;
    }
}

uint16_t NikonDSLR::Capture() {
    return Operation(MyOpCode::Capture, 0, NULL);
}

uint16_t NikonDSLR::CaptureInSDRAM() {
    return Operation(MyOpCode::CaptureInSDRAM, 0, NULL);
}

uint16_t NikonDSLR::EventCheck(uint16_t val[6]) {
    uint16_t ptp_error = PTP_RC_GeneralError;
    OperFlags flags = {0, 0, 0, 1, 4, 6};
    uint8_t bufLocal[12] = {};

    if ((ptp_error = Transaction(MyOpCode::CheckEvent, &flags, NULL, bufLocal)) != PTP_RC_OK) {
        PTPTRACE2("EventCheck error:", ptp_error);
    } else {
        val[0] = ((uint16_t)bufLocal[1] << 8) | bufLocal[0];
        val[1] = ((uint16_t)bufLocal[3] << 8) | bufLocal[2];
        val[2] = ((uint16_t)bufLocal[5] << 8) | bufLocal[4];
        val[3] = ((uint16_t)bufLocal[7] << 8) | bufLocal[6];
        val[4] = ((uint16_t)bufLocal[9] << 8) | bufLocal[8];
        val[5] = ((uint16_t)bufLocal[11] << 8) | bufLocal[10];
    }
    return ptp_error;
}

uint16_t NikonDSLR::GetLiveViewImageV2(uint8_t *&response, uint32_t &responseLenght) {
    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 0;

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetLiveViewImg, myParamBlock, myDataBlock, response, responseLenght);
}

bool NikonDSLR::getLiveView() {
    uint8_t buf;
    GetDevicePropValue(MyPcode::LiveView, (uint8_t &)buf);

    if (buf == 1) {
        return true;
    }
    return false;
}

uint16_t NikonDSLR::MoveFocus(uint8_t direction, uint16_t step) {
    OperFlags flags = {2, 0, 0, 0, 0, 0};

    uint32_t params[2];
    params[0] = (uint32_t)direction;
    params[1] = (uint32_t)step;

    return Transaction(MyOpCode::MfDrive, &flags, params, NULL);
}

void NikonDSLR::waitCameraReady(unsigned long timeout) {
    unsigned long timer = millis();
    uint16_t ret = PTP_RC_DeviceBusy;
    ret = Operation(MyOpCode::DeviceReady, 0, NULL);

    while (ret == PTP_RC_DeviceBusy) {
        if (millis() - timer > timeout) {
            Serial.println("Timeout wait Camera ready");
            return;
        }
        delay(200);
        ret = Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready : ");
        Serial.println(ret, HEX);
    }
}

//////////////////////////////
////////////////////////////////
bool LiveViewStatus = false;
void NikonDSLR::startLiveView() {
    if (!getLiveView()) {
        if (Operation(MyOpCode::StartLiveView, 0, NULL) == PTP_RC_OK) {
            LiveViewStatus = true;
        }
    }
}

void NikonDSLR::stopLiveView() {
    if (getLiveView()) {
        if (Operation(MyOpCode::EndLiveView, 0, NULL) == PTP_RC_OK) {
            LiveViewStatus = false;
        }
    }
}

void NikonDSLR::getInfo() {
    //Shutter
    getShutterSpeed();

    //Aperture
    getAperture();

    //Iso
    getIso();

    //Expo Bias
    getExpoBias();

    //Exposition Mode
    getExpositionMode();
}

//Shutter Speed : 217
bool NikonDSLR::setShutterSpeed() {
    char shutterSpeedReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            break;
        }
        shutterSpeedReceived[sizeReceived] = received;
        sizeReceived++;
    }

    int sizeList = sizeof(LIST_EXPOSURE_TIME) / sizeof(LIST_EXPOSURE_TIME[0]);
    bool shutterFound = false;
    for (int i = 0; i < sizeList; i++) {
        if (strcmp(LIST_EXPOSURE_TIME[i].name, shutterSpeedReceived) == 0) {
            shutterFound = true;
            Serial.print(" ->shutterSpeedReceived good !!: ");
            Serial.println(LIST_EXPOSURE_TIME[i].value);

            uint16_t rep = SetDevicePropValue(MyPcode::ExposureTime, LIST_EXPOSURE_TIME[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufShutterNo[] = {255, 217, 255, 'N'};
                SerialBT.write(bufShutterNo, sizeof(bufShutterNo));
                shutterFound = false;
            } else {
                uint8_t bufShutterYes[] = {255, 217, 255, 'Y'};
                SerialBT.write(bufShutterYes, sizeof(bufShutterYes));
            }

            Serial.print("Exposure Time changed to: ");
            Serial.println(LIST_EXPOSURE_TIME[i].name);
        }
    }
    return shutterFound;
}

//return -1 when error, otherwith, the index
int NikonDSLR::getShutterSpeed() {
    uint32_t buf;
    GetDevicePropValue(MyPcode::ExposureTime, (uint32_t &)buf);
    int sizeList = sizeof(LIST_EXPOSURE_TIME) / sizeof(LIST_EXPOSURE_TIME[0]);
    Serial.printf("getShutterSpeed? %d\n", buf);
    for (uint8_t i = 0; i < sizeList; i++) {
        if (LIST_EXPOSURE_TIME[i].value == buf) {
            Serial.print("Get shutter Speed: ");
            Serial.println(LIST_EXPOSURE_TIME[i].name);

            uint8_t bufShutterGet[] = {255, 217, 255, i};
            SerialBT.write(bufShutterGet, sizeof(bufShutterGet));

            return i;
        }
    }
    return -1;
}

//Aperture : 218
bool NikonDSLR::setAperture() {
    char apertureReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            break;
        }
        apertureReceived[sizeReceived] = received;
        sizeReceived++;
    }

    int sizeList = sizeof(LIST_ATPERTURE) / sizeof(LIST_ATPERTURE[0]);
    bool apertureFound = false;
    for (int i = 0; i < sizeList; i++) {
        if (strcmp(LIST_ATPERTURE[i].name, apertureReceived) == 0) {
            apertureFound = true;
            Serial.print(" ->apertureReceived good !!: ");
            Serial.println(LIST_ATPERTURE[i].value);

            uint16_t rep = SetDevicePropValue(MyPcode::FNumber, LIST_ATPERTURE[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufApertureNo[] = {255, 218, 255, 'N'};
                SerialBT.write(bufApertureNo, sizeof(bufApertureNo));
                apertureFound = false;
            } else {
                uint8_t bufApertureYes[] = {255, 218, 255, 'Y'};
                SerialBT.write(bufApertureYes, sizeof(bufApertureYes));
            }

            Serial.print("Aperture changed to: ");
            Serial.println(LIST_EXPOSURE_TIME[i].name);
        }
    }
    return apertureFound;
}

int NikonDSLR::getAperture() {
    uint16_t buf;
    GetDevicePropValue(MyPcode::FNumber, (uint16_t &)buf);

    int sizeList = sizeof(LIST_ATPERTURE) / sizeof(LIST_ATPERTURE[0]);
    for (uint8_t i = 0; i < sizeList; i++) {
        if (LIST_ATPERTURE[i].value == buf) {
            Serial.print("Get Aperture: ");
            Serial.println(LIST_ATPERTURE[i].name);

            uint8_t bufApertureGet[] = {255, 218, 255, i};
            SerialBT.write(bufApertureGet, sizeof(bufApertureGet));

            return i;
        }
    }
    return -1;
}

//Iso : 219
bool NikonDSLR::setIso() {
    char isoReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break iso");
            break;
        }
        isoReceived[sizeReceived] = received;
        sizeReceived++;
    }

    int sizeListIso = sizeof(LIST_EXP_INDEX) / sizeof(LIST_EXP_INDEX[0]);
    bool isoFound = false;
    for (int i = 0; i < sizeListIso; i++) {
        if (strcmp(LIST_EXP_INDEX[i].name, isoReceived) == 0) {
            isoFound = true;
            Serial.print(" ->IsoReceived good !!: ");
            Serial.println(LIST_EXP_INDEX[i].value);

            uint16_t rep = SetDevicePropValue(MyPcode::ExposureIndex, LIST_EXP_INDEX[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufIsoNo[] = {255, 219, 255, 'N'};
                SerialBT.write(bufIsoNo, sizeof(bufIsoNo));
                isoFound = false;
            } else {
                uint8_t bufIsoYes[] = {255, 219, 255, 'Y'};
                SerialBT.write(bufIsoYes, sizeof(bufIsoYes));
            }

            Serial.print("Iso changed to: ");
            Serial.print(LIST_EXP_INDEX[i].name);
        }
    }
    return isoFound;
}

int NikonDSLR::getIso() {
    uint16_t buf;
    GetDevicePropValue(MyPcode::ExposureIndex, (uint16_t &)buf);

    bool foundIso = false;
    int sizeList = sizeof(LIST_EXP_INDEX) / sizeof(LIST_EXP_INDEX[0]);
    for (uint8_t i = 0; i < sizeList; i++) {
        if (LIST_EXP_INDEX[i].value == buf) {
            foundIso = true;
            Serial.print("Get iso: ");
            Serial.println(LIST_EXP_INDEX[i].name);

            uint8_t bufIsoGet[] = {255, 219, 255, i};
            SerialBT.write(bufIsoGet, sizeof(bufIsoGet));
            break;
        }
    }
    return foundIso;
}

bool NikonDSLR::desactivateAutoFocus() {
    uint16_t rep = SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)0x04);
    if (rep != PTP_RC_OK) {
        Serial.print("set autoFocus to manual fail, rc: ");
        Serial.println(rep);
        return false;
    } else {
        Serial.println("autoFocus to manual");
    }
    return true;
}

bool NikonDSLR::activateAutoFocus() {
    uint16_t rep = SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)0x00);
    if (rep != PTP_RC_OK) {
        Serial.print("activate autoFocus fail, rc: ");
        Serial.println(rep);
        return false;
    } else {
        Serial.println("autoFocus activated");
    }
    return true;
}

bool NikonDSLR::setAutoFocus(uint8_t value) {
    uint16_t rep = SetDevicePropValue(MyPcode::AutofocusMode, value);
    if (rep != PTP_RC_OK) {
        Serial.print("activate autoFocus fail, rc: ");
        Serial.println(rep);
        return false;
    } else {
        Serial.println("autoFocus activated");
    }
    return true;
}

bool NikonDSLR::takePhotoAndroid() {
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break takephotoAndroid");
            break;
        }
        sizeReceived++;
    }

    if (sizeReceived < 1) {
        Serial.println("misse info take photo android");
        return false;
    }

    //Get actual focus mod
    int initialFocusMod = getFocusMod();
    Serial.print("initial focusMod: ");
    Serial.println(initialFocusMod);

    //Get LV status
    bool initialLvStatus = getLiveView();
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);

    //Stop LV
    if (initialLvStatus) {
        stopLiveView();
        delay(200);
        while (getLiveView()) {
            Serial.println("waiting for lv turning off...");
            delay(200);
        }
    }

    //desactivate autoFocus
    desactivateAutoFocus();
    waitCameraReady(3000);

    //take photo
    uint16_t ret = InitiateCapture();
    waitCameraReady(10000);
    Serial.printf("capture: %d\n", ret);
    waitCameraReady(3000);

    //Turn on auto-focus
    if (initialFocusMod != 0x04)
        setAutoFocus(initialFocusMod);

    waitCameraReady(3000);

    //Turn on liveView
    if (initialLvStatus) {
        startLiveView();
        delay(200);
        while (!getLiveView()) {
            Serial.println("waiting for lv turning on...");
            delay(200);
            startLiveView();
        }
    }
    waitCameraReady(3000);

    Serial.println("takePhotoAndroid done !!!");
    return true;
}

bool NikonDSLR::downloadJpegHq() {
    Serial.println("downloadJpegHq");
    Serial.println();
    uint8_t handleReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break object");
            break;
        }
        handleReceived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 4) {
        Serial.println("misse DownloadJpeg info");
        return false;
    }

    uint32_t handle = (handleReceived[3] << 24) |
                      (handleReceived[2] << 16) |
                      (handleReceived[1] << 8) |
                      (handleReceived[0]);

    Serial.printf("handle: %d\n", handle);

    uint8_t *response;
    uint32_t responseLenght;
    if (GetJpegHqV2(handle, response, responseLenght) != PTP_RC_OK) {
        Serial.println("GetJpegHqV2 done!");
        return true;
    }
    return false;
}

bool NikonDSLR::downloadJpeg() {
    Serial.println("downloadJpeg");
    Serial.println();
    uint8_t handleReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break object");
            break;
        }
        handleReceived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 4) {
        Serial.println("misse DownloadJpeg info");
        return false;
    }

    uint32_t handle = (handleReceived[3] << 24) |
                      (handleReceived[2] << 16) |
                      (handleReceived[1] << 8) |
                      (handleReceived[0]);

    Serial.printf("handle: %d\n", handle);

    uint8_t *response;
    uint32_t responseLenght;
    if (GetJpegV2(handle, response, responseLenght) != PTP_RC_OK) {
        Serial.println("GetJpegV2 done");
        return true;
    }
    return false;
}

void NikonDSLR::getAndSendListHandles() {
    uint8_t *response;
    uint32_t responseLenght;
    GetObjectHandlesV2(0xFFFFFFFF, 0, 0, response, responseLenght);
    sendObjectHandlesBt(response, responseLenght);

    delete[] response;
}

bool NikonDSLR::downloadThumbnail() {
    Serial.println();
    uint8_t handleReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break handle");
            break;
        }
        handleReceived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 4) {
        Serial.println("misse Downloadhandle info");
        return false;
    }

    uint32_t handle = (handleReceived[3] << 24) |
                      (handleReceived[2] << 16) |
                      (handleReceived[1] << 8) |
                      (handleReceived[0]);

    Serial.printf("handle: %d\n", handle);

    uint8_t *response;
    uint32_t responseLenght;
    if (GetThumbV2(handle, response, responseLenght) != PTP_RC_OK) {
        Serial.println("Can't get thumbnail...");
        return false;
    } else {
        sendThumbnailBt(response, responseLenght);
        delete[] response;
    }
    return true;
}

bool NikonDSLR::moveFocusAndroid() {
    char distanceReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break moveFocus");
            break;
        }
        distanceReceived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 2) {
        Serial.println("misse move focus info");
        return false;
    }

    int directionToMove = (byte)distanceReceived[0] + 1;
    int distanceToMove = (byte)distanceReceived[1];
    Serial.print("Direction: ");
    Serial.print(directionToMove);
    Serial.print("| Distance: ");
    Serial.println(distanceToMove);

    bool buf = getLiveView();

    Serial.print("Live View Status : ");
    Serial.println(buf);

    if (!buf) {
        Serial.println("not in LV, return");
        return false;
    } else {
        Serial.println("moving");
        MoveFocus(directionToMove, distanceToMove * 10);
    }
    return true;
}

//Expo Bias : 221
bool NikonDSLR::setExpoBias() {
    char expoReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break expoBias");
            break;
        }
        expoReceived[sizeReceived] = received;
        sizeReceived++;
    }

    int sizeListExpoBias = sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[0]);
    bool expoBiasFound = false;
    for (int i = 0; i < sizeListExpoBias; i++) {
        if (strcmp(LIST_EXP_BIAS_COMP[i].name, expoReceived) == 0) {
            expoBiasFound = true;
            Serial.print(" ->ExpoBiasReceived good !!: ");
            Serial.println(LIST_EXP_BIAS_COMP[i].value);

            uint16_t rep = SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufExpoBiasNo[] = {255, 221, 255, 'N'};
                SerialBT.write(bufExpoBiasNo, sizeof(bufExpoBiasNo));
                expoBiasFound = false;
            } else {
                uint8_t bufExpoBiasYes[] = {255, 221, 255, 'Y'};
                SerialBT.write(bufExpoBiasYes, sizeof(bufExpoBiasYes));
            }

            Serial.print("Expo Bias changed to: ");
            Serial.print(LIST_EXP_BIAS_COMP[i].name);
        }
    }
    return expoBiasFound;
}

int NikonDSLR::getExpoBias() {
    uint16_t buf;
    GetDevicePropValue(MyPcode::ExposureBiasCompensation, (uint16_t &)buf);

    int sizeList = sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[0]);
    for (uint8_t i = 0; i < sizeList; i++) {
        if (LIST_EXP_BIAS_COMP[i].value == buf) {
            Serial.print("Get expo Bias: ");
            Serial.println(LIST_EXP_BIAS_COMP[i].name);

            uint8_t bufExpoBiasGet[] = {255, 221, 255, i};
            SerialBT.write(bufExpoBiasGet, sizeof(bufExpoBiasGet));
            return i;
        }
    }
    Serial.println("fail get ExpoBias");
    Serial.println(buf);
    return 16;
}

int NikonDSLR::getExpositionMode() {
    uint16_t buf;
    GetDevicePropValue(MyPcode::ExposureProgramMode, (uint16_t &)buf);

    int sizeList = sizeof(LIST_EXP_PROG_MODE) / sizeof(LIST_EXP_PROG_MODE[0]);
    for (uint8_t i = 0; i < sizeList; i++) {
        if (LIST_EXP_PROG_MODE[i].value == buf) {
            Serial.print("Exposure Prog Mode: ");
            Serial.println(LIST_EXP_PROG_MODE[i].name);

            uint8_t bufExpoModeGet[] = {255, 222, 255, i};
            SerialBT.write(bufExpoModeGet, sizeof(bufExpoModeGet));
            return i;
        }
    }

    Serial.println("fail get Exposition Mode");
    Serial.println(buf);
    return 0;
}

int NikonDSLR::getFocusMod() {
    uint8_t buf;
    uint16_t rep = GetDevicePropValue(MyPcode::AutofocusMode, (uint8_t &)buf);
    if (rep != PTP_RC_OK) {
        Serial.print("set AF lock ");
        Serial.println(rep);
    }

    return buf;
}

void NikonDSLR::takeLongExposure() {
    uint8_t delay_01 = SerialBT.read();
    uint8_t delay_02 = SerialBT.read();
    int delayLongExpo = 1;
    if (SerialBT.read() == ';') {
        delayLongExpo = delay_01 << 8 | delay_02;
        Serial.printf("\ndelayLongExpo: %d\n", delayLongExpo);

    } else {
        Serial.println("\nerror receiving long expo");
        return;
    }

    //Get actual focus mod
    int initialFocusMod = getFocusMod();
    Serial.print("initial focusMod: ");
    Serial.println(initialFocusMod);

    //Set autoFocus manual
    desactivateAutoFocus();
    waitCameraReady(3000);

    //Get LV status
    bool initialLvStatus = getLiveView();
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);
    //Stop LV
    if (!initialLvStatus) {
        stopLiveView();
        delay(200);
        while (getLiveView()) {
            Serial.println("waiting for lv turning on...");
            delay(200);
        }
    }

    uint16_t repBulb = SetDevicePropValue(MyPcode::ExposureTime, LIST_EXPOSURE_TIME[53].value);
    if (repBulb != PTP_RC_OK) {
        Serial.print("error, rc: ");
        Serial.println(repBulb);

    } else {
        Serial.println("set expo time to bulb succes!!");
    }
    waitCameraReady(3000);
    delay(500);
    //Boucle pour prendre des photos

    uint32_t paraCapture[2] = {0xFFFFFFFF, 0x00000000};
    uint16_t retCaptur = Operation(MyOpCode::StartBulbCapture, 2, paraCapture);
    Serial.print("CAptureInit: ");
    Serial.println(retCaptur, HEX);
    delay(200);

    do {
        retCaptur = Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready2 : ");
        Serial.println(retCaptur, HEX);
        delay(200);
    } while (retCaptur == PTP_RC_DeviceBusy);

    delay(delayLongExpo * 1000);

    uint32_t paraCaptureStop[2] = {0xFFFFFFFF, 0x00000000};
    uint16_t retCapturEnd = Operation(MyOpCode::EndBulbCapture, 2, paraCaptureStop);
    Serial.print("Capture end: ");
    Serial.println(retCapturEnd, HEX);
    delay(200);

    do {
        retCapturEnd = Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready2 : ");
        Serial.println(retCapturEnd, HEX);
        delay(200);
    } while (retCapturEnd == PTP_RC_DeviceBusy);
    waitCameraReady(3000);

    //Turn on auto-focus
    if (initialFocusMod != 0x04)
        setAutoFocus(initialFocusMod);

    Serial.println("Long exposure done !!!");
}

bool NikonDSLR::takeTimeLapse(int &timeTimelapse) {
    char timerReceived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break takeTimeLapse");
            break;
        }
        timerReceived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 1) {
        Serial.println("misse info take timelapse android");
        return false;
    }

    timeTimelapse = ((uint8_t)timerReceived[0] << 8) | (uint8_t)timerReceived[1];
    Serial.print("TIMER_TIMELAPSE:");
    Serial.println(timeTimelapse);
    return true;
}

void NikonDSLR::takeHdr(uint8_t *hdrParams) {
    Serial.println("takeHdr function");
    uint8_t hdrNumber = hdrParams[0];       //3
    uint8_t hdrPasDix = hdrParams[1];       //10  = 10x exp pas  10= 1.0, 13= 1.33
    uint8_t hdrOffsetIndex = hdrParams[2];  //15 == 15eme valeur dans LIST_EXP_BIAS_COMP = 0.0

    //Get actual expo Bias
    int initialExpoBiasIndex = getExpoBias();

    //Get actual focus mod
    int initialFocusMod = getFocusMod();
    Serial.print("initial focusMod: ");
    Serial.println(initialFocusMod);

    //Convert Pas to number of index
    int pasIndex = ((hdrPasDix + 1) / 10 * 3);  //(10+1)/10*3 = 11/10*3 = 1.1*3 = 3    ///////  (11+1)/10*3 = 1.2*3 = 3.6 = 3
    Serial.print("pasIndex: ");
    Serial.println(pasIndex);

    //Make a list with expo Bias to do
    int startIndex = -hdrNumber / 2;  //-3/2 = -1
    Serial.print("startIndex: ");
    Serial.println(startIndex);

    uint8_t expoBiasIndexList[31] = {0};
    for (int i = 0; i < hdrNumber; i++) {                          //i<3
        int index = (startIndex + i) * pasIndex + hdrOffsetIndex;  //(-1+0)*3+15 = -3+15 = 12
        Serial.print("index: ");
        Serial.println(index);
        expoBiasIndexList[index] = 1;
    }

    //Get LV status
    bool initialLvStatus = getLiveView();
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);
    //Stop LV
    if (initialLvStatus) {
        stopLiveView();
        delay(200);
        while (getLiveView()) {
            Serial.println("waiting for lv turning off...");
            delay(200);
        }
    }

    //Boucle pour prendre des photos
    int countCaptures = 0;
    for (int i = 0; i < sizeof(expoBiasIndexList); i++) {
        if (expoBiasIndexList[i] == 1) {
            countCaptures++;
            if ((countCaptures == 2 && !initialLvStatus) || (countCaptures == 1 && initialLvStatus)) {
                desactivateAutoFocus();
                waitCameraReady(3000);
            }
            uint16_t rep = SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("set Expo bias fail, rc: ");
                Serial.println(rep);
            } else {
                Serial.print("set Expo bias success: ");
                Serial.println(i);
                waitCameraReady(3000);
                uint16_t ret = InitiateCapture();

                //uint16_t ret = CaptureImage();
                waitCameraReady(10000);
                Serial.print("capture: ");
                Serial.println(ret);
            }
            waitCameraReady(3000);
        }
    }

    //Turn on auto-focus
    if (initialFocusMod != 0x04)
        setAutoFocus(initialFocusMod);

    //Remise à l'ancien expoBias
    uint16_t rep = SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[initialExpoBiasIndex].value);
    if (rep != PTP_RC_OK) {
        Serial.print("set Expo bias initial fail, rc: ");
        Serial.println(rep);
        Serial.println(initialExpoBiasIndex);
        Serial.println(LIST_EXP_BIAS_COMP[initialExpoBiasIndex].value);
    } else {
        Serial.println("set Expo bias initial success: ");
    }

    //Si LV était activé, on l'active à nouveau
    /*if (initialLvStatus) {
        startLiveView();
        delay(200);
        while (!getLiveView()) {
            Serial.println("waiting for lv turning on...");
            delay(200);
            startLiveView();
        }
    }*/

    Serial.println("HDR done !!!");
    //On envoi une réponse à Android "fin !"
}

void NikonDSLR::takeHdr() {
    uint8_t hdrParams[3] = {};
    if (SerialBT.available() >= 4) {
        hdrParams[0] = SerialBT.read();
        hdrParams[1] = SerialBT.read();
        hdrParams[2] = SerialBT.read();

        Serial.print("HDR: ");
        Serial.print(hdrParams[0]);
        Serial.print(" ");
        Serial.print(hdrParams[1]);
        Serial.print(" ");
        Serial.println(hdrParams[2]);
    } else {
        Serial.println("hdr, not enough parameters");
        return;
    }

    takeHdr(hdrParams);
}

void NikonDSLR::tryFocus() {
    uint32_t posFocus[2] = {};
    if (SerialBT.available() >= 8) {
        int posX1 = SerialBT.read();
        int posX2 = SerialBT.read();
        int posX3 = SerialBT.read();
        int posX4 = SerialBT.read();

        Serial.print("X: ");
        Serial.print(posX1);
        Serial.print("|");
        Serial.print(posX2);
        Serial.print("|");
        Serial.print(posX3);
        Serial.print("|");
        Serial.println(posX4);

        posFocus[0] = ((posX1 << 24) |
                       (posX2 << 16) |
                       (posX3 << 8) |
                       posX4);

        int posY1 = SerialBT.read();
        int posY2 = SerialBT.read();
        int posY3 = SerialBT.read();
        int posY4 = SerialBT.read();

        Serial.print("Y: ");
        Serial.print(posY1);
        Serial.print("|");
        Serial.print(posY2);
        Serial.print("|");
        Serial.print(posY3);
        Serial.print("|");
        Serial.println(posY4);

        posFocus[1] = ((posY1 << 24) |
                       (posY2 << 16) |
                       (posY3 << 8) |
                       posY4);
        Serial.print(" || X: ");
        Serial.print(posFocus[0]);
        Serial.print("  Y: ");
        Serial.println(posFocus[1]);
    } else {
        Serial.println("focus, not enough parameters");
        return;
    }

    //Check live view status
    bool buf = getLiveView();

    Serial.print("Live View Status : ");
    Serial.println(buf);

    uint16_t ret = 0;

    if (!buf) {  //Si on est pas en live view, on l'active => à changer pour autofocus sans LV
        startLiveView();
        delay(500);
    } else {
        do {
            ret = Operation(MyOpCode::ChangeAfArea, 2, posFocus);
            Serial.print("Change AF Area: ");
            Serial.println(ret, HEX);
            delay(200);
        } while (ret == (uint16_t)MyRpCode::NotLiveView);

        ret = Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready : ");
        Serial.println(ret, HEX);
        delay(200);

        // 3 tentatives d'autofocus
        uint8_t tentativeFocus = 0;
        do {
            ret = Operation(MyOpCode::AutoFocus, 0, NULL);
            Serial.print("AfDRive: ");
            Serial.println(ret, HEX);
            delay(200);
            tentativeFocus++;
            if (tentativeFocus > 3) break;
            do {
                ret = Operation(MyOpCode::DeviceReady, 0, NULL);
                Serial.print("Ready : ");
                Serial.println(ret, HEX);
                delay(200);
            } while (ret == PTP_RC_DeviceBusy);
            Serial.println("Ready01");
        } while (ret == (uint16_t)MyRpCode::OutOfFocus);
        if (ret == (uint16_t)MyRpCode::OutOfFocus) {
            Serial.println("Focus done, out of it");
            uint8_t bufFocusNo[] = {255, 220, 255, 'N'};
            SerialBT.write(bufFocusNo, sizeof(bufFocusNo));
        } else {
            Serial.println("Focus done, succes !!!");
            uint8_t bufFocusYes[] = {255, 220, 255, 'Y'};
            SerialBT.write(bufFocusYes, sizeof(bufFocusYes));
        }
    }
}

void NikonDSLR::sendThumbnailBt(uint8_t *response, uint32_t responseLenght) {
    sendBluetoothData(MyBluetoothCode::ThumbnailOut, response, responseLenght);
}

void NikonDSLR::sendObjectHandlesBt(uint8_t *response, uint32_t responseLenght) {
    if (responseLenght < 28) {
        Serial.println("No photos...");
        return;
    }

    int numberHandles = (responseLenght - 24) / 4;
    Serial.printf("%d handles\n", numberHandles);

    for (int i = 0; i < numberHandles; i++) {
        uint32_t handle = (response[27 + i * 4] << 24) |
                          (response[26 + i * 4] << 16) |
                          (response[25 + i * 4] << 8) |
                          response[24 + i * 4];
        //Serial.printf("%08x\n", handle);

        uint8_t *responseDesc;
        uint32_t responseDescLenght;
        GetObjectInfoV2(handle, responseDesc, responseDescLenght);

        char photoName[13] = {};  //DSC_0001.NEF
        for (int i = 65, y = 0; y < 12; i += 2, y++) {
            photoName[y] = responseDesc[i];
        }
        Serial.printf("PhotoName: %s\n", photoName);

        char photoTime[16] = {};  //20210723T211502
        for (int i = 92, y = 0; y < 16; i += 2, y++) {
            photoTime[y] = responseDesc[i];
        }
        //Serial.printf("PhotoTime: %s\n", photoTime);

        delete[] responseDesc;

        uint8_t objectHandle[32] = {};
        for (int m = 0; m < 4; m++)
            objectHandle[m] = response[27 - m + i * 4];
        for (int m = 4; m < 16; m++)
            objectHandle[m] = (uint8_t)photoName[m - 4];
        for (int m = 16; m < 31; m++)
            objectHandle[m] = (uint8_t)photoTime[m - 16];

        /*for (int m = 0; m < 32; m++) {
            Serial.printf("%02x ", objectHandle[m]);
        }
        Serial.println();*/

        sendBluetoothData(MyBluetoothCode::ObjectHandlesOut, objectHandle, sizeof(objectHandle));
    }
    uint8_t emptyList[] = {};
    sendBluetoothData(MyBluetoothCode::EndObjectHandlesListOut, emptyList, 0);
    Serial.println("Done !");
}

void NikonDSLR::sendBluetoothData(MyBluetoothCode bluetoothCode, uint8_t *buf, int bufLen) {
    static MyBluetoothCode mylastCode = MyBluetoothCode::LiveViewOff;

    if (bluetoothCode == MyBluetoothCode::LiveViewImageOut ||
        bluetoothCode == MyBluetoothCode::ThumbnailOut) {
        uint8_t bufHeader[] = {255, static_cast<uint8_t>(bluetoothCode), 255};
        SerialBT.write(bufHeader, sizeof(bufHeader));
        SerialBT.write(buf, bufLen);
    } else if (bluetoothCode == MyBluetoothCode::JpegOut || bluetoothCode == MyBluetoothCode::JpegHqOut) {
        if (bluetoothCode != mylastCode) {
            //uint8_t bufHeader[] = {255, static_cast<uint8_t>(bluetoothCode), 255};
            //SerialBT.write(bufHeader, sizeof(bufHeader));

            uint8_t *bufDynamic = new uint8_t[3 + bufLen];
            bufDynamic[0] = 255;
            bufDynamic[1] = static_cast<uint8_t>(bluetoothCode);
            bufDynamic[2] = 255;
            for (int i = 0; i < bufLen; i++) {
                bufDynamic[i + 3] = buf[i];
            }

            SerialBT.write(bufDynamic, 3 + bufLen);
            delete[] bufDynamic;
        } else {
            SerialBT.write(buf, bufLen);
        }

    } else {
        uint8_t *bufDynamic = new uint8_t[3 + bufLen];
        bufDynamic[0] = 255;
        bufDynamic[1] = static_cast<uint8_t>(bluetoothCode);
        bufDynamic[2] = 255;
        for (int i = 0; i < bufLen; i++) {
            bufDynamic[i + 3] = buf[i];
        }

        SerialBT.write(bufDynamic, 3 + bufLen);
        delete[] bufDynamic;
    }

    mylastCode = bluetoothCode;
}

void uint16_to_char_V2(uint16_t integer, unsigned char *data) {
    data[0] = (integer & 0xff);
    data[1] = ((integer >> 8) & 0xff);
}

void uint32_to_char_V2(uint32_t integer, unsigned char *data) {
    data[0] = (integer & 0xff);
    data[1] = ((integer >> 8) & 0xff);
    data[2] = ((integer >> 16) & 0xff);
    data[3] = ((integer >> 24) & 0xff);
}

uint16_t NikonDSLR::TransactionV2(MyOpCode opcode, MyParamBlock myParamBlock, MyDataBlock myDataBlock, uint8_t *&response, uint32_t &responseLenght) {
    Serial.printf("TrV2, opCode: %04x\n", (int)opcode);
    uint8_t rcode;

    //Send command block
    {
        uint8_t cmd[PTP_USB_BULK_HDR_LEN + 12];  // header + 3 uint32_t parameters
        ZerroMemory(PTP_USB_BULK_HDR_LEN + 12, cmd);

        // Make command PTP container header
        uint16_to_char_V2(PTP_USB_CONTAINER_COMMAND, (unsigned char *)(cmd + PTP_CONTAINER_CONTYPE_OFF));     // type
        uint16_to_char_V2(static_cast<uint16_t>(opcode), (unsigned char *)(cmd + PTP_CONTAINER_OPCODE_OFF));  // code
        uint32_to_char_V2(++idTransaction, (unsigned char *)(cmd + PTP_CONTAINER_TRANSID_OFF));               // transaction id

        uint8_t numberOfParam = myParamBlock.numberOfParam;  // number of parameters
        uint8_t len;

        //S'il y a des paramètres
        if (numberOfParam) {
            *((uint8_t *)cmd) = PTP_USB_BULK_HDR_LEN + (numberOfParam << 2);
            len = PTP_USB_BULK_HDR_LEN + (numberOfParam << 2);

            for (uint32_t *p1 = (uint32_t *)(cmd + PTP_CONTAINER_PAYLOAD_OFF), *p2 = (uint32_t *)myParamBlock.params; numberOfParam--; p1++, p2++) {
                uint32_to_char_V2(*p2, (unsigned char *)p1);
            }
        } else {
            *((uint8_t *)cmd) = PTP_USB_BULK_HDR_LEN;
            len = PTP_USB_BULK_HDR_LEN;
        }

        // Envoi de la commande
        rcode = pUsb->outTransfer(devAddress, epInfo[epDataOutIndex].epAddr, len, cmd);

        if (rcode) {  // S'il y a un problème
            Serial.printf("Sending transaction failed with error: %d\n", rcode);
            //ESP.restart();
            return PTP_RC_GeneralError;
        }
    }

    {  // Send data block
        uint8_t data[PTP_MAX_RX_BUFFER_LEN];
        uint32_t *pd32 = reinterpret_cast<uint32_t *>(data);

        if (myDataBlock.txOperation) {  //Si j'ai des data à envoyé
            ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

            //si pvoid est buffer pointer => BL = 12 + flags->dataSize
            //si pvoid est PTPReadParser => BL = 12 + ((PTPDataSupplier *)pVoid)->GetDataSize()
            //Sinon => BL = 12
            uint32_t bytes_left = myDataBlock.dataSize;

            // Make data PTP container header
            *pd32 = bytes_left;
            uint16_to_char_V2(PTP_USB_CONTAINER_DATA, (unsigned char *)(data + PTP_CONTAINER_CONTYPE_OFF));        // type
            uint16_to_char_V2(static_cast<uint16_t>(opcode), (unsigned char *)(data + PTP_CONTAINER_OPCODE_OFF));  // code
            uint32_to_char_V2(idTransaction, (unsigned char *)(data + PTP_CONTAINER_TRANSID_OFF));                 // transaction id

            uint16_t len = 0;

            /*if (flags->typeOfVoid == 1) {
                len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;
            }

            if (flags->typeOfVoid == 3) {
                uint8_t *p1 = (data + PTP_USB_BULK_HDR_LEN);
                uint8_t *p2 = (uint8_t *)pVoid;
                Serial.println("flags->typeOfVoid == 3");
                for (uint8_t i = flags->dataSize; i; i--, p1++, p2++) {
                    *p1 = *p2;
                }

                len = PTP_USB_BULK_HDR_LEN + flags->dataSize;
            }*/

            uint8_t *p1 = (data + PTP_USB_BULK_HDR_LEN);
            uint8_t *p2 = (uint8_t *)myDataBlock.data;

            for (uint8_t i = myDataBlock.dataSize; i; i--, p1++, p2++) {
                *p1 = *p2;
            }
            len = PTP_USB_BULK_HDR_LEN + myDataBlock.dataSize;

            while (bytes_left) {
                //if (flags->typeOfVoid == 1)
                //    ((PTPDataSupplier *)pVoid)->GetData((first_time) ? len - PTP_USB_BULK_HDR_LEN : len, (first_time) ? (data + PTP_USB_BULK_HDR_LEN) : data);

                rcode = pUsb->outTransfer(devAddress, epInfo[epDataOutIndex].epAddr, len, data);

                if (rcode) {
                    Serial.printf("Transaction: Data block send error.: %d\n", rcode);
                    ESP.restart();
                    return PTP_RC_GeneralError;
                }

                bytes_left -= len;
                len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;
            }
        }  //Fin envoi data

        // Because inTransfer does not return the actual number of bytes received, it should be
        // calculated here.
        uint32_t total = 0, data_off = 0;  // Total PTP data packet size, Data offset
        uint8_t inbuffer = 0;              // Number of bytes read into buffer
        uint16_t loops = 0;                // Number of loops necessary to get all the data from device

        uint8_t nefStartIndex = 254;
        uint32_t nefIndex = 0;
        uint32_t subIfdTagIndex = 0;
        uint32_t subIfdAddress = 0;
        uint32_t ifdAddress = 0;
        uint32_t jpegStartTagIndex = 0;
        uint32_t jpegLengthTagIndex = 0;
        uint32_t jpegStartAddress = 0;
        uint32_t jpegLength = 0;
        bool jpegStartFound = false;

        uint8_t jpegType = myParamBlock.params[1];  //0: JEPG, 1: JPEG HQ, 2: RAW
        while (1) {
            //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
            if (opcode == MyOpCode::GetObject) {
                if (loops == 0) {
                    Serial.println("loop 0 getObject");
                }
                //sendBluetoothData( MyBluetoothCode::JpegOut, data, inbuffer);
                //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
                if (data_off % 100000 == 0)
                    Serial.printf("##off: %d\n", data_off);

                //if (nefStartIndex == 254)
                //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);

                if (jpegStartFound) {
                    if (jpegType == 0)
                        sendBluetoothData(MyBluetoothCode::JpegOut, data, inbuffer);
                    else if (jpegType == 1)
                        sendBluetoothData(MyBluetoothCode::JpegHqOut, data, inbuffer);

                    //Check end of JPEG
                    for (int i = 0; i < inbuffer; i++) {
                        if (data[i] == 0xFF && data[i + 1] == 0xD9) {
                            //Envoi du dernier packet avec la longueur du JPEG
                            if (jpegType == 0)
                                sendBluetoothData(MyBluetoothCode::JpegOut, data, i + 2);
                            else if (jpegType == 1)
                                sendBluetoothData(MyBluetoothCode::JpegHqOut, data, i + 2);

                            Serial.println("Sending last JPEG packet");

                            Serial.printf("Found end jpeg at %d\n", nefIndex);
                            delay(500);
                            return PTP_RC_Undefined;
                        }
                    }
                } else {
                    for (int i = 0; i < inbuffer; i++) {
                        if (nefStartIndex == 254) {  // on commence par chercher le debut du fichier NEF
                            if (data[i] == 0x49 && data[i + 1] == 0x49 &&
                                data[i + 2] == 0x2a && data[i + 3] == 0x00) {
                                nefStartIndex = i + data_off - inbuffer;
                                Serial.printf("nefStart Index found at %d\n", nefStartIndex);
                            }
                        } else {  // Le fichier NEF est trouvé !
                            nefIndex++;
                            if (subIfdTagIndex == 0) {  //On cherche le tag qui indique les subIfds
                                //Serial.printf("subIfdTagIndex == 0 || %d | %d | %d\n", data[i], data[i + 1], nefIndex);
                                if (nefIndex > 9 && ((nefIndex - 10) % 12) == 0 && data[i] == 0x4a && data[i + 1] == 0x01) {
                                    Serial.printf("Found subIfdTagIndex at %d\n", nefIndex);
                                    subIfdTagIndex = nefIndex;
                                }
                            } else if (subIfdAddress == 0) {  // On cherche l'adresse des subIfds
                                if (nefIndex == subIfdTagIndex + 8) {
                                    subIfdAddress = (data[i]) |
                                                    (data[i + 1] << 8) |
                                                    (data[i + 2] << 16) |
                                                    (data[i + 3] << 24);
                                    Serial.printf("Found subIfdAddress at %d\n", subIfdAddress);
                                }
                            } else if (ifdAddress == 0) {  // On cherche l'adresse du subIfd qui nous interresse
                                if (jpegType == 0) {       //JPEG
                                    if (nefIndex == subIfdAddress + 8) {
                                        ifdAddress = (data[i]) |
                                                     (data[i + 1] << 8) |
                                                     (data[i + 2] << 16) |
                                                     (data[i + 3] << 24);
                                        Serial.printf("Found ifdAddress at %d\n", ifdAddress);
                                    }
                                } else if (jpegType == 1) {  // JPEG HQ
                                    if (nefIndex == subIfdAddress) {
                                        ifdAddress = (data[i]) |
                                                     (data[i + 1] << 8) |
                                                     (data[i + 2] << 16) |
                                                     (data[i + 3] << 24);
                                        Serial.printf("Found ifdAddress at %d\n", ifdAddress);
                                    }
                                }
                            } else if (jpegStartTagIndex == 0) {  // On cherche l'index du tag de l'adresse du fichier JPEG
                                if (nefIndex > ifdAddress && ((nefIndex - ifdAddress - 2) % 12) == 0 && data[i] == 0x01 && data[i + 1] == 0x02) {
                                    Serial.printf("Found jpegStartTagIndex at %d\n", nefIndex);
                                    jpegStartTagIndex = nefIndex;
                                }
                            } else if (jpegStartAddress == 0) {  // On cherche l'index de start du fichier JPEG
                                if (nefIndex == jpegStartTagIndex + 8) {
                                    jpegStartAddress = data[i] |
                                                       (data[i + 1] << 8) |
                                                       (data[i + 2] << 16) |
                                                       (data[i + 3] << 24);
                                    Serial.printf("Found jpegStartAddress at %d, address= %d\n", nefIndex, jpegStartAddress);
                                }
                            } else if (jpegLengthTagIndex == 0) {  // On cherche l'index du tag de la longueur du fichier JPEG
                                if (((nefIndex - ifdAddress - 2) % 12) == 0 && data[i] == 0x02 && data[i + 1] == 0x02) {
                                    Serial.printf("Found jpegLengthTagIndex at %d\n", nefIndex);
                                    jpegLengthTagIndex = nefIndex;
                                }
                            } else if (jpegLength == 0) {  // On relève la longueur du JPEG
                                if (nefIndex == jpegLengthTagIndex + 8) {
                                    jpegLength = data[i] |
                                                 (data[i + 1] << 8) |
                                                 (data[i + 2] << 16) |
                                                 (data[i + 3] << 24);
                                    Serial.printf("Found jpegLength at %d, length= %d\n", nefIndex, jpegLength);

                                    //Envoi du premier packet avec la longueur du JPEG
                                    if (jpegType == 0)
                                        sendBluetoothData(MyBluetoothCode::JpegOut, data + i, 4);
                                    else if (jpegType == 1)
                                        sendBluetoothData(MyBluetoothCode::JpegHqOut, data + i, 4);
                                    Serial.println("Sending JPEG length");
                                }
                            } else if (jpegStartFound == false) {  // On cherche le debut du JPEG
                                if (nefIndex == jpegStartAddress) {
                                    Serial.printf("Found start jpeg => %d | %d | %d \n", data[i], data[i + 1], data[i + 2]);
                                    jpegStartFound = true;

                                    //Envoi du premier packet du JPEG
                                    if (jpegType == 0)
                                        sendBluetoothData(MyBluetoothCode::JpegOut, data + i, inbuffer - i);
                                    else if (jpegType == 1)
                                        sendBluetoothData(MyBluetoothCode::JpegHqOut, data + i, inbuffer - i);
                                    Serial.println("Sending first packet");
                                }
                            }
                        }
                    }  //end for
                }

            } else {
                if (total != 0 && loops == 1) {
                    //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
                    //Serial.printf("creating num uint8_t[%d]\n", total);
                    response = new uint8_t[total];
                    responseLenght = total;
                    //Serial.printf("response buffer created, size:%d !\n", responseLenght);
                }

                //Serial.println("Data: ");
                for (int k = 0; k < inbuffer; k++) {
                    //Serial.printf("%02x ", data[k]);
                    response[k + data_off - inbuffer] = data[k];
                }
                //Serial.println();
            }

            ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

            uint16_t read = PTP_MAX_RX_BUFFER_LEN;

            rcode = pUsb->inTransfer(devAddress, epInfo[epDataInIndex].epAddr, &read, data);

            if (rcode) {
                Serial.printf("Transaction: inTransfer receive error: %d\n", rcode);
                ESP.restart();
                return PTP_RC_GeneralError;
            }

            // This can occur in case of unsupported operation or successive response after data reception stage
            if ((!loops || total == data_off) && *((uint16_t *)(data + PTP_CONTAINER_CONTYPE_OFF)) == PTP_USB_CONTAINER_RESPONSE) {
                uint16_t response = *((uint16_t *)(data + PTP_CONTAINER_OPCODE_OFF));

                if (response == PTP_RC_OK && *pd32 > PTP_USB_BULK_HDR_LEN) {
                    Serial.println("response with parameter !!");
                    // number of params = (container length - 12) / 4
                    uint8_t n = (*pd32 - PTP_USB_BULK_HDR_LEN) >> 2;
                    // 1 paramète = 4 bytes (0xFF 0xFF 0xFF 0xFF)

                    //
                    //
                    //flags->rsParams = n;
                    //
                    //

                    for (uint32_t *p1 = (uint32_t *)(data + PTP_USB_BULK_HDR_LEN), *p2 = (uint32_t *)myParamBlock.params; n; n--, p1++, p2++) {
                        Serial.printf("%02x # ", *p1);
                        //Serial.print(" # ");
                        p2 = p1;
                    }
                    Serial.println();
                }
                if (response != PTP_RC_OK) {
                    Serial.printf("Transaction: Response receive error: %d\n", response);
                    data_off = 0;
                }
                return response;
            }

            if (loops == 0) {
                total = *pd32;
                inbuffer = (total < PTP_MAX_RX_BUFFER_LEN) ? (uint8_t)total : PTP_MAX_RX_BUFFER_LEN;
            } else {
                inbuffer = ((total - data_off) > PTP_MAX_RX_BUFFER_LEN) ? PTP_MAX_RX_BUFFER_LEN : (uint8_t)(total - data_off);
            }

            data_off += inbuffer;
            //Serial.printf("data_off %d\n", data_off);

            loops++;
        }
    }  // end of scope
}

uint16_t NikonDSLR::GetObjectInfoV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght) {
    //OperFlags flags = {1, 0, 0, 1, 1, 0};

    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 1;
    myParamBlock.params[0] = handle;

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetObjectInfo, myParamBlock, myDataBlock, response, responseLenght);
}

uint16_t NikonDSLR::GetObjectHandlesV2(uint32_t storage_id, uint16_t format, uint16_t assoc, uint8_t *&response, uint32_t &responseLenght) {
    //OperFlags flags = {3, 0, 0, 1, 1, 0};

    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 3;
    myParamBlock.params[0] = storage_id;
    myParamBlock.params[1] = (uint32_t)format;
    myParamBlock.params[2] = (uint32_t)assoc;

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetObjectHandles, myParamBlock, myDataBlock, response, responseLenght);
}

uint16_t NikonDSLR::GetObjectV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght) {
    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 1;
    myParamBlock.params[0] = handle;

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetObject, myParamBlock, myDataBlock, response, responseLenght);
}

uint16_t NikonDSLR::GetJpegV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght) {
    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 1;
    myParamBlock.params[0] = handle;
    myParamBlock.params[1] = 0;  //0= JPEG

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetObject, myParamBlock, myDataBlock, response, responseLenght);
}

uint16_t NikonDSLR::GetJpegHqV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght) {
    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 1;
    myParamBlock.params[0] = handle;
    myParamBlock.params[1] = 1;  //1 = JPEG HQ

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetObject, myParamBlock, myDataBlock, response, responseLenght);
}

uint16_t NikonDSLR::GetThumbV2(uint32_t handle, uint8_t *&response, uint32_t &responseLenght) {
    MyParamBlock myParamBlock;
    myParamBlock.numberOfParam = 1;
    myParamBlock.params[0] = handle;

    MyDataBlock myDataBlock;
    myDataBlock.dataSize = 0;
    myDataBlock.txOperation = 0;

    return TransactionV2(MyOpCode::GetThumb, myParamBlock, myDataBlock, response, responseLenght);
}

void NikonDSLR::printEvent(uint16_t *bufEvent) {
    int sizeList = sizeof(LIST_EVENT) / sizeof(LIST_EVENT[0]);

    for (int i = 0; i < 6; i++) {
        if (bufEvent[i] != 0x000) {
            Serial.printf("%04x -> ", bufEvent[i]);
            bool nameFound = false;
            for (int j = 0; j < sizeList; j++) {
                if (LIST_EVENT[j].value == bufEvent[i]) {
                    Serial.println(LIST_EVENT[j].name);
                    nameFound = true;

                    if (strcmp("ExpTime", LIST_EVENT[j].name) == 0) {
                        getShutterSpeed();
                    } else if (strcmp("ExpPrgMod", LIST_EVENT[j].name) == 0) {
                        getExpositionMode();
                    } else if (strcmp("Fnumber", LIST_EVENT[j].name) == 0) {
                        getAperture();
                    } else if (strcmp("IsoCtlSen", LIST_EVENT[j].name) == 0) {
                        getIso();
                    }

                    break;
                }
            }
            if (!nameFound) {
                Serial.println("Event Undefinned");
            }
        }
    }
}