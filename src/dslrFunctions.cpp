#include <D3300.h>
#include <main.h>
bool LiveViewStatus = false;

bool getLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint8_t buf;
    nikon.GetDevicePropValue(MyPcode::LiveView, (uint8_t &)buf);

    if (buf == 1) {
        return true;
    }
    return false;
}

void startLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    if (!getLiveView(nikon, SerialBT)) {
        if (nikon.Operation(MyOpCode::StartLiveView, 0, NULL) == PTP_RC_OK) {
            LiveViewStatus = true;
        }
    }
}

void stopLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    if (getLiveView(nikon, SerialBT)) {
        if (nikon.Operation(MyOpCode::EndLiveView, 0, NULL) == PTP_RC_OK) {
            LiveViewStatus = false;
        }
    }
}

void getInfo(Nikon &nikon, BluetoothSerial &SerialBT) {
    //Shutter
    getShutterSpeed(nikon, SerialBT);

    //Aperture
    getAperture(nikon, SerialBT);

    //Iso
    getIso(nikon, SerialBT);

    //Expo Bias
    getExpoBias(nikon, SerialBT);

    //Exposition Mode
    getExpositionMode(nikon, SerialBT);
}

//Shutter Speed : 217
bool setShutterSpeed(Nikon &nikon, BluetoothSerial &SerialBT) {
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

            uint16_t rep = nikon.SetDevicePropValue(MyPcode::ExposureTime, LIST_EXPOSURE_TIME[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufShutterNo[] = {255, 217, 255, 'N'};
                SerialBT.write(bufShutterNo, sizeof(bufShutterNo));
            } else {
                uint8_t bufShutterYes[] = {255, 217, 255, 'Y'};
                SerialBT.write(bufShutterYes, sizeof(bufShutterYes));
            }

            Serial.print("Exposure Time changed to: ");
            Serial.println(LIST_EXPOSURE_TIME[i].name);
        }
    }
}

//return -1 when error, otherwith, the index
int getShutterSpeed(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint16_t buf;
    nikon.GetDevicePropValue(MyPcode::ExposureTime, (uint16_t &)buf);
    int sizeList = sizeof(LIST_EXPOSURE_TIME) / sizeof(LIST_EXPOSURE_TIME[0]);
    for (int i = 0; i < sizeList; i++) {
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
bool setAperture(Nikon &nikon, BluetoothSerial &SerialBT) {
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

            uint16_t rep = nikon.SetDevicePropValue(MyPcode::FNumber, LIST_ATPERTURE[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufApertureNo[] = {255, 218, 255, 'N'};
                SerialBT.write(bufApertureNo, sizeof(bufApertureNo));
            } else {
                uint8_t bufApertureYes[] = {255, 218, 255, 'Y'};
                SerialBT.write(bufApertureYes, sizeof(bufApertureYes));
            }

            Serial.print("Aperture changed to: ");
            Serial.println(LIST_EXPOSURE_TIME[i].name);
        }
    }
}

int getAperture(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint16_t buf;
    nikon.GetDevicePropValue(MyPcode::FNumber, (uint16_t &)buf);

    int sizeList = sizeof(LIST_ATPERTURE) / sizeof(LIST_ATPERTURE[0]);
    for (int i = 0; i < sizeList; i++) {
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
bool setIso(Nikon &nikon, BluetoothSerial &SerialBT) {
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

            uint16_t rep = nikon.SetDevicePropValue(MyPcode::ExposureIndex, LIST_EXP_INDEX[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufIsoNo[] = {255, 219, 255, 'N'};
                SerialBT.write(bufIsoNo, sizeof(bufIsoNo));
            } else {
                uint8_t bufIsoYes[] = {255, 219, 255, 'Y'};
                SerialBT.write(bufIsoYes, sizeof(bufIsoYes));
            }

            Serial.print("Iso changed to: ");
            Serial.print(LIST_EXP_INDEX[i].name);
        }
    }
}

int getIso(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint16_t buf;
    nikon.GetDevicePropValue(MyPcode::ExposureIndex, (uint16_t &)buf);

    int sizeList = sizeof(LIST_ATPERTURE) / sizeof(LIST_ATPERTURE[0]);
    for (int i = 0; i < sizeList; i++) {
        if (LIST_EXP_INDEX[i].value == buf) {
            Serial.print("Get iso: ");
            Serial.println(LIST_EXP_INDEX[i].name);

            uint8_t bufIsoGet[] = {255, 219, 255, i};
            SerialBT.write(bufIsoGet, sizeof(bufIsoGet));
        }
    }
}

bool takePhotoAndroid(Nikon &nikon, BluetoothSerial &SerialBT) {
    char focusReveived[10] = {};
    uint8_t sizeReceived = 0;
    while (SerialBT.available()) {
        uint8_t received = SerialBT.read();
        if (sizeReceived > 10 || received == ';') {
            Serial.println("break takephotoAndroid");
            break;
        }
        focusReveived[sizeReceived] = received;
        sizeReceived++;
    }

    if (sizeReceived < 1) {
        Serial.println("misse info take photo android");
        return false;
    }

    //Get actual focus mod
    int initialFocusMod = getFocusMod(nikon, SerialBT);
    Serial.print("initial focusMod: ");
    Serial.println(initialFocusMod);

    //Get LV status
    bool initialLvStatus = getLiveView(nikon, SerialBT);
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);
    //Stop LV
    if (initialLvStatus) {
        stopLiveView(nikon, SerialBT);
        delay(200);
        while (getLiveView(nikon, SerialBT)) {
            Serial.println("waiting for lv turning off...");
            delay(200);
        }
    }

    //prendre une photo
    uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)0x04);
    if (rep != PTP_RC_OK) {
        Serial.print("set autoFocus to manual fail, rc: ");
        Serial.println(rep);
    } else {
        Serial.println("autoFocus to manual");
    }
    nikon.waitCameraReady(3000);

    uint16_t ret = nikon.InitiateCapture();
    nikon.waitCameraReady(10000);
    Serial.print("capture: ");
    Serial.println(ret);
    nikon.waitCameraReady(3000);

    //Turn on auto-focus
    if (initialFocusMod != 0x04) {
        uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)initialFocusMod);
        if (rep != PTP_RC_OK) {
            Serial.print("set autoFocus to initial fail, rc: ");
            Serial.println(rep);
        } else {
            Serial.println("autoFocus to initial");
        }
    }
    nikon.waitCameraReady(3000);

    if (initialLvStatus) {
        startLiveView(nikon, SerialBT);
        delay(200);
        while (!getLiveView(nikon, SerialBT)) {
            Serial.println("waiting for lv turning on...");
            delay(200);
            startLiveView(nikon, SerialBT);
        }
    }
    nikon.waitCameraReady(3000);

    Serial.println("takePhotoAndroid done !!!");
}

bool downloadJpegHq(Nikon &nikon, BluetoothSerial &SerialBT) {
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
    if (nikon.GetJpegHqV2(handle, response, responseLenght, SerialBT) != PTP_RC_OK) {
        Serial.println("Can't get object...");
    }
}

bool downloadJpeg(Nikon &nikon, BluetoothSerial &SerialBT) {
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

    /*uint32_t handle = (handleReceived[0] << 24) |
                      (handleReceived[1] << 16) |
                      (handleReceived[2] << 8) |
                      (handleReceived[3]);*/
    uint32_t handle = (handleReceived[3] << 24) |
                      (handleReceived[2] << 16) |
                      (handleReceived[1] << 8) |
                      (handleReceived[0]);

    Serial.printf("handle: %d\n", handle);

    uint8_t *response;
    uint32_t responseLenght;
    if (nikon.GetJpegV2(handle, response, responseLenght, SerialBT) != PTP_RC_OK) {
        Serial.println("Can't get object...");
    }
}

void getAndSendListHandles(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint8_t *response;
    uint32_t responseLenght;
    nikon.GetObjectHandlesV2(0xFFFFFFFF, 0, 0, response, responseLenght, SerialBT);
    sendObjectHandlesBt(nikon, SerialBT, response, responseLenght);

    delete[] response;
}

bool downloadThumbnail(Nikon &nikon, BluetoothSerial &SerialBT) {
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

    /*uint32_t handle = (handleReceived[0] << 24) |
                      (handleReceived[1] << 16) |
                      (handleReceived[2] << 8) |
                      (handleReceived[3]);*/
    uint32_t handle = (handleReceived[3] << 24) |
                      (handleReceived[2] << 16) |
                      (handleReceived[1] << 8) |
                      (handleReceived[0]);

    Serial.printf("handle: %d\n", handle);

    uint8_t *response;
    uint32_t responseLenght;
    if (nikon.GetThumbV2(handle, response, responseLenght, SerialBT) != PTP_RC_OK) {
        Serial.println("Can't get thumbnail...");
    } else {
        sendThumbnailBt(nikon, SerialBT, response, responseLenght);
        delete[] response;
    }
}

bool moveFocusAndroid(Nikon &nikon, BluetoothSerial &SerialBT) {
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

    uint8_t buf;
    nikon.GetDevicePropValue(MyPcode::LiveView, (uint8_t &)buf);

    Serial.print("Live View Status : ");
    Serial.println(buf);

    if (buf == 0) {
        Serial.println("not in LV, return");
        return false;
    } else {
        Serial.println("moving");
        nikon.MoveFocus(directionToMove, distanceToMove * 10);
    }
    return true;
}

//Expo Bias : 221
bool setExpoBias(Nikon &nikon, BluetoothSerial &SerialBT) {
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

            uint16_t rep = nikon.SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
                uint8_t bufExpoBiasNo[] = {255, 221, 255, 'N'};
                SerialBT.write(bufExpoBiasNo, sizeof(bufExpoBiasNo));
            } else {
                uint8_t bufExpoBiasYes[] = {255, 221, 255, 'Y'};
                SerialBT.write(bufExpoBiasYes, sizeof(bufExpoBiasYes));
            }

            Serial.print("Expo Bias changed to: ");
            Serial.print(LIST_EXP_BIAS_COMP[i].name);
        }
    }
}

int getExpoBias(Nikon &nikon, BluetoothSerial &SerialBT) {
    ////////////
    uint16_t buf;
    nikon.GetDevicePropValue(MyPcode::ExposureBiasCompensation, (uint16_t &)buf);

    int sizeList = sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[0]);
    for (int i = 0; i < sizeList; i++) {
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

int getExpositionMode(Nikon &nikon, BluetoothSerial &SerialBT) {
    ////////////
    uint16_t buf;
    nikon.GetDevicePropValue(MyPcode::ExposureProgramMode, (uint16_t &)buf);

    int sizeList = sizeof(LIST_EXP_PROG_MODE) / sizeof(LIST_EXP_PROG_MODE[0]);
    for (int i = 0; i < sizeList; i++) {
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

int getFocusMod(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint8_t buf;
    uint16_t rep = nikon.GetDevicePropValue(MyPcode::AutofocusMode, (uint8_t &)buf);
    if (rep != PTP_RC_OK) {
        Serial.print("set AF lock ");
        Serial.println(rep);
    }

    return buf;
}

void takeLongExposure(Nikon &nikon, BluetoothSerial &SerialBT) {
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
    int initialFocusMod = getFocusMod(nikon, SerialBT);
    Serial.print("initial focusMod: ");
    Serial.println(initialFocusMod);

    //Set autoFocus manual
    uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)0x04);
    if (rep != PTP_RC_OK) {
        Serial.print("set autoFocus to manual fail, rc: ");
        Serial.println(rep);
    } else {
        Serial.println("autoFocus to manual");
    }
    nikon.waitCameraReady(3000);

    //Get LV status
    bool initialLvStatus = getLiveView(nikon, SerialBT);
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);
    //Stop LV
    if (!initialLvStatus) {
        stopLiveView(nikon, SerialBT);
        delay(200);
        while (getLiveView(nikon, SerialBT)) {
            Serial.println("waiting for lv turning on...");
            delay(200);
        }
    }

    uint16_t repBulb = nikon.SetDevicePropValue(MyPcode::ExposureTime, LIST_EXPOSURE_TIME[53].value);
    if (repBulb != PTP_RC_OK) {
        Serial.print("error, rc: ");
        Serial.println(repBulb);

    } else {
        Serial.println("set expo time to bulb succes!!");
    }
    nikon.waitCameraReady(3000);
    delay(500);
    //Boucle pour prendre des photos

    uint32_t paraCapture[2] = {0xFFFFFFFF, 0x00000000};
    uint16_t retCaptur = nikon.Operation(MyOpCode::StartBulbCapture, 2, paraCapture);
    Serial.print("CAptureInit: ");
    Serial.println(retCaptur, HEX);
    delay(200);

    do {
        retCaptur = nikon.Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready2 : ");
        Serial.println(retCaptur, HEX);
        delay(200);
    } while (retCaptur == PTP_RC_DeviceBusy);

    delay(delayLongExpo * 1000);

    uint32_t paraCaptureStop[2] = {0xFFFFFFFF, 0x00000000};
    uint16_t retCapturEnd = nikon.Operation(MyOpCode::EndBulbCapture, 2, paraCaptureStop);
    Serial.print("Capture end: ");
    Serial.println(retCapturEnd, HEX);
    delay(200);

    do {
        retCapturEnd = nikon.Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready2 : ");
        Serial.println(retCapturEnd, HEX);
        delay(200);
    } while (retCapturEnd == PTP_RC_DeviceBusy);
    nikon.waitCameraReady(3000);
    /*uint16_t repinit = nikon.InitiateCapture();
    if (repinit != PTP_RC_OK) {
        Serial.printf("repinit image error: %d\n", repinit);
    }
    nikon.waitCameraReady(3000);
    delay(500);
    uint16_t repCapOpen = nikon.InitiateOpenCapture();
    if (repCapOpen != PTP_RC_OK) {
        Serial.printf("repCapOpen image error: %d\n", repCapOpen);
    }
    nikon.waitCameraReady(3000);
    delay(500);
    //nikon.Capture();
    uint16_t repCap = nikon.CaptureImage();
    if (repCap != PTP_RC_OK) {
        Serial.printf("capture image error: %d\n", repCap);
    }
    nikon.waitCameraReady(3000);
    delay(5000);
    nikon.TerminateOpenCapture(0);
    nikon.waitCameraReady(3000);*/
    //Turn on auto-focus
    if (initialFocusMod != 0x04) {
        uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)initialFocusMod);
        if (rep != PTP_RC_OK) {
            Serial.print("set autoFocus to initial fail, rc: ");
            Serial.println(rep);
        } else {
            Serial.println("autoFocus to initial");
        }
    }

    Serial.println("Long exposure done !!!");
}

bool takeTimeLapse(Nikon &nikon, BluetoothSerial &SerialBT, int &timeTimelapse) {
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
    }

    timeTimelapse = ((uint8_t)timerReceived[0] << 8) | (uint8_t)timerReceived[1];
    Serial.print("TIMER_TIMELAPSE:");
    Serial.println(timeTimelapse);
}
void takeHdr(Nikon &nikon, BluetoothSerial &SerialBT, uint8_t *hdrParams) {
    Serial.println("takeHdr function");
    uint8_t hdrNumber = hdrParams[0];       //3
    uint8_t hdrPasDix = hdrParams[1];       //10  = 10x exp pas  10= 1.0, 13= 1.33
    uint8_t hdrOffsetIndex = hdrParams[2];  //15 == 15eme valeur dans LIST_EXP_BIAS_COMP = 0.0

    //Get actual expo Bias
    int initialExpoBiasIndex = getExpoBias(nikon, SerialBT);

    //Get actual focus mod
    int initialFocusMod = getFocusMod(nikon, SerialBT);
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
    bool initialLvStatus = getLiveView(nikon, SerialBT);
    Serial.print("initialLvStatus: ");
    Serial.println(initialLvStatus);
    //Stop LV
    if (initialLvStatus) {
        stopLiveView(nikon, SerialBT);
        delay(200);
        while (getLiveView(nikon, SerialBT)) {
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
                uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)0x04);
                if (rep != PTP_RC_OK) {
                    Serial.print("set autoFocus to manual fail, rc: ");
                    Serial.println(rep);
                } else {
                    Serial.println("autoFocus to manual");
                }
                nikon.waitCameraReady(3000);
            }
            uint16_t rep = nikon.SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
            if (rep != PTP_RC_OK) {
                Serial.print("set Expo bias fail, rc: ");
                Serial.println(rep);
            } else {
                Serial.print("set Expo bias success: ");
                Serial.println(i);
                nikon.waitCameraReady(3000);
                uint16_t ret = nikon.InitiateCapture();

                //uint16_t ret = nikon.CaptureImage();
                nikon.waitCameraReady(10000);
                Serial.print("capture: ");
                Serial.println(ret);
            }
            nikon.waitCameraReady(3000);
        }
    }

    //Turn on auto-focus
    if (initialFocusMod != 0x04) {
        uint16_t rep = nikon.SetDevicePropValue(MyPcode::AutofocusMode, (uint8_t)initialFocusMod);
        if (rep != PTP_RC_OK) {
            Serial.print("set autoFocus to initial fail, rc: ");
            Serial.println(rep);
        } else {
            Serial.println("autoFocus to initial");
        }
    }

    //Remise à l'ancien expoBias
    uint16_t rep = nikon.SetDevicePropValue(MyPcode::ExposureBiasCompensation, LIST_EXP_BIAS_COMP[initialExpoBiasIndex].value);
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
        startLiveView(nikon, SerialBT);
        delay(200);
        while (!getLiveView(nikon, SerialBT)) {
            Serial.println("waiting for lv turning on...");
            delay(200);
            startLiveView(nikon, SerialBT);
        }
    }*/

    Serial.println("HDR done !!!");
    //On envoi une réponse à Android "fin !"
}

void takeHdr(Nikon &nikon, BluetoothSerial &SerialBT) {
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

    takeHdr(nikon, SerialBT, hdrParams);
}

void tryFocus(Nikon &nikon, BluetoothSerial &SerialBT) {
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
    uint8_t buf;
    nikon.GetDevicePropValue(MyPcode::LiveView, (uint8_t &)buf);

    Serial.print("Live View Status : ");
    Serial.println(buf);

    uint16_t ret = 0;

    if (buf == 0) {  //Si on est pas en live view, on l'active => à changer pour autofocus sans LV
        ret = nikon.Operation(MyOpCode::StartLiveView, 0, NULL);
        delay(500);
    } else {
        //uint32_t coordinate[2] = {4500, 1000};
        /*if (incrementPool % 2 == 0) {
            Serial.println("coordinate 1");
            coordinate[0] = 2000;
            coordinate[1] = 1000;
        } else if (incrementPool % 2 == 1) {
            Serial.println("coordinate 2");
            coordinate[0] = 5000;
            coordinate[1] = 3000;
        }
*/
        do {
            ret = nikon.Operation(MyOpCode::ChangeAfArea, 2, posFocus);
            Serial.print("Change AF Area: ");
            Serial.println(ret, HEX);
            delay(200);
        } while (ret == NK_RC_NotLiveView);

        ret = nikon.Operation(MyOpCode::DeviceReady, 0, NULL);
        Serial.print("Ready : ");
        Serial.println(ret, HEX);
        delay(200);

        // 3 tentatives d'autofocus
        uint8_t tentativeFocus = 0;
        do {
            ret = nikon.Operation(MyOpCode::AutoFocus, 0, NULL);
            Serial.print("AfDRive: ");
            Serial.println(ret, HEX);
            delay(200);
            tentativeFocus++;
            if (tentativeFocus > 3) break;
            do {
                ret = nikon.Operation(MyOpCode::DeviceReady, 0, NULL);
                Serial.print("Ready : ");
                Serial.println(ret, HEX);
                delay(200);
            } while (ret == PTP_RC_DeviceBusy);
            Serial.println("Ready01");
        } while (ret == NK_RC_OutOfFocus);
        if (ret == NK_RC_OutOfFocus) {
            Serial.println("Focus done, out of it");
            uint8_t bufFocusNo[] = {255, 220, 255, 'N'};
            SerialBT.write(bufFocusNo, sizeof(bufFocusNo));
        } else {
            Serial.println("Focus done, succes !!!");
            uint8_t bufFocusYes[] = {255, 220, 255, 'Y'};
            SerialBT.write(bufFocusYes, sizeof(bufFocusYes));
        }

        /*uint32_t paraCapture[2] = {0xFFFFFFFF, 0x00000000};
        ret = nikon.Operation(0x9207, 2, paraCapture);
        Serial.print("CAptureInit: ");
        Serial.println(ret, HEX);
        delay(200);

        do {
            ret = nikon.Operation(MyOpCode::DeviceReady, 0, NULL);
            Serial.print("Ready2 : ");
            Serial.println(ret, HEX);
            delay(200);
        } while (ret == PTP_RC_DeviceBusy);*/
    }
}

void sendThumbnailBt(Nikon &nikon, BluetoothSerial &SerialBT, uint8_t *response, uint32_t responseLenght) {
    sendBluetoothData(SerialBT, MyBluetoothCode::ThumbnailOut, response, responseLenght);
}

/*void sendJpegBt(Nikon &nikon, BluetoothSerial &SerialBT, uint8_t *response, uint32_t responseLenght) {
    int beginIndex = 0;
    int endIndex = 0;

    for (int i = 0; i < responseLenght - 1; i++) {
        if (response[i] == 0xFF && response[i + 1] == 0xD8)
            beginIndex = i;
        else if (response[i] == 0xFF && response[i + 1] == 0xD9)
            endIndex = i + 1;
    }

    Serial.printf("beginIndex jpeg:%d, endIndex Jpeg: %d\n", beginIndex, endIndex);

    //sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, response, responseLenght);
}*/
/*void sendLiveViewImage(Nikon &nikon, BluetoothSerial &SerialBT, uint8_t *response, uint32_t responseLenght) {
    sendBluetoothData(SerialBT, MyBluetoothCode::LiveViewImageOut, response, responseLenght);
}*/

void sendObjectHandlesBt(Nikon &nikon, BluetoothSerial &SerialBT, uint8_t *response, uint32_t responseLenght) {
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
        nikon.GetObjectInfoV2(handle, responseDesc, responseDescLenght, SerialBT);

        //Serial.printf("Response desc exported handle(%08x):\n", handle);
        /*for (int i = 0; i < responseDescLenght; i++) {
            Serial.printf("%d : %02x\n", i, responseDesc[i]);
            //if (!i % 16 && i)
            //    Serial.println();
        }
        Serial.println();*/

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

        for (int m = 0; m < 32; m++) {
            Serial.printf("%02x ", objectHandle[m]);
        }
        Serial.println();

        sendBluetoothData(SerialBT, MyBluetoothCode::ObjectHandlesOut, objectHandle, sizeof(objectHandle));
    }

    Serial.println("Done !");
}

void sendBluetoothData(BluetoothSerial &SerialBT, MyBluetoothCode bluetoothCode, uint8_t *buf, int bufLen) {
    static MyBluetoothCode mylastCode = MyBluetoothCode::LiveViewOff;

    if (bluetoothCode == MyBluetoothCode::LiveViewImageOut ||
        bluetoothCode == MyBluetoothCode::ThumbnailOut) {
        uint8_t bufHeader[] = {255, static_cast<uint8_t>(bluetoothCode), 255};
        SerialBT.write(bufHeader, sizeof(bufHeader));
        SerialBT.write(buf, bufLen);
    } else if (bluetoothCode == MyBluetoothCode::JpegOut || bluetoothCode == MyBluetoothCode::JpegHqOut) {
        if (bluetoothCode != mylastCode) {
            uint8_t bufHeader[] = {255, static_cast<uint8_t>(bluetoothCode), 255};
            SerialBT.write(bufHeader, sizeof(bufHeader));
        }
        SerialBT.write(buf, bufLen);
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
