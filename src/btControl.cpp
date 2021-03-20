#include <D3300.h>
#include <main.h>
bool LiveViewStatus = false;

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