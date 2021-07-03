#include <D3300.h>
#include <main.h>
bool LiveViewStatus = false;

void timeLapseTask(void *pvParameters);

bool getLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    uint8_t buf;
    nikon.GetDevicePropValue(0xD1A2, (uint8_t &)buf);

    if (buf == 1) {
        return true;
    }
    return false;
}

void startLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    if (!getLiveView(nikon, SerialBT)) {
        if (nikon.Operation(PTP_OC_NIKON_StartLiveView, 0, NULL) == PTP_RC_OK) {
            LiveViewStatus = true;
        }
    }
}
void stopLiveView(Nikon &nikon, BluetoothSerial &SerialBT) {
    if (getLiveView(nikon, SerialBT)) {
        if (nikon.Operation(PTP_OC_NIKON_EndLiveView, 0, NULL) == PTP_RC_OK) {
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

            uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_ExposureTime, LIST_EXPOSURE_TIME[i].value);
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
    nikon.GetDevicePropValue(PTP_DPC_ExposureTime, (uint16_t &)buf);
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

            uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_FNumber, LIST_ATPERTURE[i].value);
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
    nikon.GetDevicePropValue(PTP_DPC_FNumber, (uint16_t &)buf);

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

            uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_ExposureIndex, LIST_EXP_INDEX[i].value);
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
    nikon.GetDevicePropValue(PTP_DPC_ExposureIndex, (uint16_t &)buf);

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
/*
bool takeTimeLapse(Nikon &nikon, BluetoothSerial &SerialBT) {
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
    TIMER_ENABLE = true;

    //StartTimeLapse
    xTaskCreatePinnedToCore(
        timeLapseTask,    
        "timeLapse Task", 
        10000,            
        (void *)&nikon,   
        1,                
        NULL, 1);         
}*/

/*void timeLapseTask(void *pvParameters) {
    Serial.println("CreateTimeLapse Task");
    unsigned long timerTL = millis();
    int numberphoto = 0;
    Nikon nikon = *((Nikon *)pvParameters);
    while (true) {
        numberphoto++;
        Serial.print("TimeLapse Photo:");
        Serial.println(numberphoto);

        //prendre une photo
        nikon.waitCameraReady(3000);

        uint16_t ret = nikon.InitiateCapture();
        nikon.waitCameraReady(10000);
        Serial.print("capture: ");
        Serial.println(ret);
        nikon.waitCameraReady(3000);
        timerTL = millis();
        while (millis() - timerTL < TIMER_TIMELAPSE) {
            if (!TIMER_ENABLE) {
                Serial.print("Finish timeLapseTask");
                vTaskDelete(NULL);
            }
        }
    }
}*/

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
    uint16_t rep = nikon.SetDevicePropValue(NK_DPC_AutofocusMode, (uint8_t)0x04);
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
        uint16_t rep = nikon.SetDevicePropValue(NK_DPC_AutofocusMode, (uint8_t)initialFocusMod);
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
    nikon.GetDevicePropValue(0xD1A2, (uint8_t &)buf);

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

            uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
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
    nikon.GetDevicePropValue(PTP_DPC_ExposureBiasCompensation, (uint16_t &)buf);

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
    nikon.GetDevicePropValue(PTP_DPC_ExposureProgramMode, (uint16_t &)buf);

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
    uint16_t rep = nikon.GetDevicePropValue(NK_DPC_AutofocusMode, (uint8_t &)buf);
    if (rep != PTP_RC_OK) {
        Serial.print("set AF lock ");
        Serial.println(rep);
    }

    return buf;
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
    int pasIndex = ((hdrPasDix + 1) / 10 * 3); //(10+1)/10*3 = 11/10*3 = 1.1*3 = 3    ///////  (11+1)/10*3 = 1.2*3 = 3.6 = 3
    Serial.print("pasIndex: ");
    Serial.println(pasIndex);

    //Make a list with expo Bias to do
    int startIndex = -hdrNumber / 2; //-3/2 = -1
    Serial.print("startIndex: ");
    Serial.println(startIndex);

    uint8_t expoBiasIndexList[31] = {0};
    for (int i = 0; i < hdrNumber; i++) { //i<3
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
                uint16_t rep = nikon.SetDevicePropValue(NK_DPC_AutofocusMode, (uint8_t)0x04);
                if (rep != PTP_RC_OK) {
                    Serial.print("set autoFocus to manual fail, rc: ");
                    Serial.println(rep);
                } else {
                    Serial.println("autoFocus to manual");
                }
                nikon.waitCameraReady(3000);
            }
            uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_ExposureBiasCompensation, LIST_EXP_BIAS_COMP[i].value);
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
        uint16_t rep = nikon.SetDevicePropValue(NK_DPC_AutofocusMode, (uint8_t)initialFocusMod);
        if (rep != PTP_RC_OK) {
            Serial.print("set autoFocus to initial fail, rc: ");
            Serial.println(rep);
        } else {
            Serial.println("autoFocus to initial");
        }
    }

    //Remise à l'ancien expoBias
    uint16_t rep = nikon.SetDevicePropValue(PTP_DPC_ExposureBiasCompensation, LIST_EXP_BIAS_COMP[initialExpoBiasIndex].value);
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

void tryFocus(Nikon &nikon, BluetoothSerial &SerialBT, uint32_t *posFocus) {
    //Check live view status
    uint8_t buf;
    nikon.GetDevicePropValue(0xD1A2, (uint8_t &)buf);

    Serial.print("Live View Status : ");
    Serial.println(buf);

    uint16_t ret = 0;

    if (buf == 0) {  //Si on est pas en live view, on l'active => à changer pour autofocus sans LV
        ret = nikon.Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
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
            ret = nikon.Operation(PTP_OC_NIKON_ChangeAfArea, 2, posFocus);
            Serial.print("Change AF Area: ");
            Serial.println(ret, HEX);
            delay(200);
        } while (ret == NK_RC_NotLiveView);

        ret = nikon.Operation(NK_OC_DeviceReady, 0, NULL);
        Serial.print("Ready : ");
        Serial.println(ret, HEX);
        delay(200);

        // 3 tentatives d'autofocus
        uint8_t tentativeFocus = 0;
        do {
            ret = nikon.Operation(0x90C1, 0, NULL);
            Serial.print("AfDRive: ");
            Serial.println(ret, HEX);
            delay(200);
            tentativeFocus++;
            if (tentativeFocus > 3) break;
            do {
                ret = nikon.Operation(NK_OC_DeviceReady, 0, NULL);
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
            ret = nikon.Operation(NK_OC_DeviceReady, 0, NULL);
            Serial.print("Ready2 : ");
            Serial.println(ret, HEX);
            delay(200);
        } while (ret == PTP_RC_DeviceBusy);*/
    }
}