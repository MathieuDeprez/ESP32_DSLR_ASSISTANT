#include <Arduino.h>
#include <D3300.h>
#include <canonps.h>
#include <myDumper.h>
#include <nikon.h>
#include <psvaluetitles.h>
#include <ptp.h>
#include <ptpdebug.h>
#include <ptpdpparser.h>
#include <usbhub.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#include "devpropparser.h"

BluetoothSerial static SerialBT;
ImageDump imageDump;

/**
 * 1 : Prop Desc
 * 2 : Aperture
 * 3 : Compression
 * 4 : Batterie Level
 * 5 : Image Format
 * 6 : White Balance
 * 7 : Focal Length
 * 8 : Focus Mode
 * 9 : Exposition Metering Mode
 * 10: Exposure Time
 * 11: Exposure Program Mode
 * 12: Exposure Index (ISO)
 * 13: Still Capture Mode
 * 14: Burst Number
 * 15: Noise Reduction
 * 16: Lock photo No SD
 * 17: Color Space
 * 18: Min Focal Length
 * 19: Max Focal Length
 * 20: Exposure Bias Compensation
 * 21: Live View Status
 * 22: Enable/Disable LiveView
 * 23: Capture Live View
 * 24: MoveFocus
 * 25: Capture
 * 26: Capture via live View
 * 27: Check Event
 * 28: Bluetooth send/receive data
 * 29: Bluetooth send/receive event
 * */
#define CHANGE_TEST 30

class CamStateHandlers : public PTPStateHandlers {
    enum CamStates { stInitial,
                     stDisconnected,
                     stConnected };
    CamStates stateConnected;

   public:
    CamStateHandlers() : stateConnected(stInitial){};

    virtual void OnDeviceDisconnectedState(PTP *ptp);
    virtual void OnDeviceInitializedState(PTP *ptp);
};

#include <simplefifo.h>

SimpleFIFO<uint16_t, 16> Fifo;

void PrintPropValues(PTP *ptp) {
    while (Fifo.Size()) {
        uint16_t prop = Fifo.Pop();

        Serial.println("");
        PrintHex<uint16_t>(prop, 0x80);
        Serial.println("");

        HexDump hex;
        uint16_t ret = ptp->GetDevicePropValue(prop, &hex);
    }
}

/*CamStateHandlers CamStates;
USB Usb;
USBHub Hub1(&Usb);
PTP Ptp(&Usb, &CamStates);*/

class Nikon : public NikonDSLR {
    uint32_t nextPollTime;  // Time of the next poll to occure

   public:
    bool bPollEnabled;  // Enables or disables camera poll
    int incrementPool = 0;

    Nikon(USB *pusb, PTPStateHandlers *pstates) : NikonDSLR(pusb, pstates), nextPollTime(0), bPollEnabled(false){};

    virtual uint8_t Poll() {
        static bool first_time = true;
        PTP::Poll();

        if (!bPollEnabled)
            return 0;

        //if (first_time)
        //InitiateCapture();

        uint32_t current_time = millis();

        if (current_time >= nextPollTime) {
            Serial.println("\r\n");
            Serial.println("pool");

#if CHANGE_TEST == 1  // Get device Prop Desc
            if (first_time) {
                uint16_t prefix[3] = {0xD400, 0xD300, 0x5000};

                for (uint16_t i = 20000; i < 20500; i++) {
                    HexDump dmp;

                    if (GetDevicePropDesc(i, &dmp) == PTP_RC_OK) {
                        E_Notify(PSTR("\r\n"), 0x80);

                        DevPropParser prs;

                        if (GetDevicePropDesc(i, &prs) == PTP_RC_OK)
                            E_Notify(PSTR("\r\n"), 0x80);
                    }

                    E_Notify(PSTR("\r\n"), 0x80);
                }
                first_time = false;
            }

#elif CHANGE_TEST == 2  // Set/Get Aperture

            int sizeList = sizeof(LIST_ATPERTURE) / sizeof(LIST_ATPERTURE[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_FNumber, LIST_ATPERTURE[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Comp change to: ");
            Serial.print(LIST_ATPERTURE[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_FNumber, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_ATPERTURE[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_ATPERTURE[i].name);
                }
            }

#elif CHANGE_TEST == 3  // Set/Get Compression Settings

            int sizeList = sizeof(LIST_COMPRESSION) / sizeof(LIST_COMPRESSION[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_CompressionSetting, LIST_COMPRESSION[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Comp change to: ");
            Serial.print(LIST_COMPRESSION[indexP].name);

            uint8_t buf;
            GetDevicePropValue(PTP_DPC_CompressionSetting, (uint8_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_COMPRESSION[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_COMPRESSION[i].name);
                }
            }

#elif CHANGE_TEST == 4  // Get Battery Level

            uint8_t buf;
            GetDevicePropValue(PTP_DPC_BatteryLevel, (uint8_t &)buf);

            Serial.print("Battery Level: ");
            Serial.print(buf / 64 * 100);
            Serial.println("%");

#elif CHANGE_TEST == 5  // Set/Get Image Size

            int sizeList = sizeof(LIST_IMAGE_FORMAT) / sizeof(LIST_IMAGE_FORMAT[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_ImageSize, LIST_IMAGE_FORMAT[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Image Size change to: ");
            Serial.print(LIST_IMAGE_FORMAT[indexP].name);

            char buf[11] = {};
            GetDevicePropValue(PTP_DPC_ImageSize, buf);

            Serial.print(" ->rep: ");
            Serial.println(buf);

#elif CHANGE_TEST == 6  // Set/Get White Balance

            int sizeList = sizeof(LIST_WHITE_BALANCE) / sizeof(LIST_WHITE_BALANCE[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_WhiteBalance, LIST_WHITE_BALANCE[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("White Balance change to: ");
            Serial.print(LIST_WHITE_BALANCE[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_WhiteBalance, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_WHITE_BALANCE[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_WHITE_BALANCE[i].name);
                }
            }
#elif CHANGE_TEST == 7  // Get Focus Mode

            uint32_t buf;
            GetDevicePropValue(PTP_DPC_FocalLength, (uint32_t &)buf);

            Serial.print("Focal Length: ");
            Serial.print(buf / 100, DEC);
            Serial.println("mm");

#elif CHANGE_TEST == 8  // Get Focal Length

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_FocusMode, (uint16_t &)buf);

            int sizeList = sizeof(LIST_FOCUS_MODE) / sizeof(LIST_FOCUS_MODE[0]);
            for (int i = 0; i < sizeList; i++) {
                if (LIST_FOCUS_MODE[i].value == buf) {
                    Serial.print("Focus Mode: ");
                    Serial.println(LIST_FOCUS_MODE[i].name);
                }
            }

#elif CHANGE_TEST == 9  // Set/Get Exposition Metering Mod

            int sizeList = sizeof(LIST_EXP_MET_MODE) / sizeof(LIST_EXP_MET_MODE[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_ExposureMeteringMode, LIST_EXP_MET_MODE[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Exposition Metering Mod change to: ");
            Serial.print(LIST_EXP_MET_MODE[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_ExposureMeteringMode, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_EXP_MET_MODE[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_EXP_MET_MODE[i].name);
                }
            }

#elif CHANGE_TEST == 10  // Set/Get Exposure Time

            int sizeList = sizeof(LIST_EXPOSURE_TIME) / sizeof(LIST_EXPOSURE_TIME[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_ExposureTime, LIST_EXPOSURE_TIME[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Exposure Time change to: ");
            Serial.print(LIST_EXPOSURE_TIME[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_ExposureTime, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_EXPOSURE_TIME[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_EXPOSURE_TIME[i].name);
                }
            }

#elif CHANGE_TEST == 11  // Get Exposure Program Mode

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_ExposureProgramMode, (uint16_t &)buf);

            int sizeList = sizeof(LIST_EXP_PROG_MODE) / sizeof(LIST_EXP_PROG_MODE[0]);
            for (int i = 0; i < sizeList; i++) {
                if (LIST_EXP_PROG_MODE[i].value == buf) {
                    Serial.print("Exposure Prog Mode: ");
                    Serial.println(LIST_EXP_PROG_MODE[i].name);
                }
            }

#elif CHANGE_TEST == 12  // Set/Get Exposure Index (ISO)

            int sizeList = sizeof(LIST_EXP_INDEX) / sizeof(LIST_EXP_INDEX[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_ExposureIndex, LIST_EXP_INDEX[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Exposure Index change to: ");
            Serial.print(LIST_EXP_INDEX[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_ExposureIndex, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_EXP_INDEX[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_EXP_INDEX[i].name);
                }
            }

#elif CHANGE_TEST == 13  // Set/Get Still Capture Mode

            int sizeList = sizeof(LIST_STILL_CAPTURE_MODE) / sizeof(LIST_STILL_CAPTURE_MODE[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_StillCaptureMode, LIST_STILL_CAPTURE_MODE[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Still Capture Mode change to: ");
            Serial.print(LIST_STILL_CAPTURE_MODE[indexP].name);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_StillCaptureMode, (uint16_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_STILL_CAPTURE_MODE[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_STILL_CAPTURE_MODE[i].name);
                }
            }

#elif CHANGE_TEST == 14  // Set/Get Burst Number ???

            int sizeList = 64;
            int indexP = incrementPool % sizeList;

            uint16_t indexL = indexP;

            uint16_t rep = SetDevicePropValue(PTP_DPC_BurstNumber, indexL);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Burst Number change to: ");
            Serial.print(indexL);

            uint16_t buf;
            GetDevicePropValue(PTP_DPC_BurstNumber, (uint16_t &)buf);

            Serial.print(" ->rep: ");
            Serial.println(buf);

#elif CHANGE_TEST == 15  // Set/Get Noise Reduction

            int sizeList = 2;
            int indexP = incrementPool % sizeList;

            uint8_t indexL = indexP;

            uint16_t rep = SetDevicePropValue(NK_DPC_LongExposureNoiseReduction, indexL);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Noise Reduction change to: ");
            Serial.print(indexL);

            uint8_t buf;
            GetDevicePropValue(NK_DPC_LongExposureNoiseReduction, (uint8_t &)buf);

            Serial.print(" ->rep: ");
            Serial.println(buf);

#elif CHANGE_TEST == 16  // Set/Get Lock photo No SD (1:Lock)

            int sizeList = 2;
            int indexP = incrementPool % sizeList;

            uint8_t indexL = indexP;

            uint16_t rep = SetDevicePropValue(NK_DPC_NoCFCard, indexL);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Lock photo change to: ");
            Serial.print(indexL);

            uint8_t buf;
            GetDevicePropValue(NK_DPC_NoCFCard, (uint8_t &)buf);

            Serial.print(" ->rep: ");
            Serial.println(buf);

#elif CHANGE_TEST == 17  // Set/Get Lock photo No SD (1:Lock)  1:Adobe 0:SRGB

            int sizeList = sizeof(LIST_COLOR_SPACE) / sizeof(LIST_COLOR_SPACE[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(0xD032, LIST_COLOR_SPACE[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Color Space change to: ");
            Serial.print(LIST_COLOR_SPACE[indexP].name);

            uint8_t buf;
            GetDevicePropValue(0xD032, (uint8_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_COLOR_SPACE[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_COLOR_SPACE[i].name);
                }
            }

#elif CHANGE_TEST == 18  // Get Min Focal Length

            uint32_t buf;
            GetDevicePropValue(NK_DPC_FocalLengthMin, (uint32_t &)buf);

            Serial.print("Min focal length : ");
            Serial.print(buf / 100);
            Serial.println("mm");

#elif CHANGE_TEST == 19  // Get Max Focal Length

            uint32_t buf;
            GetDevicePropValue(NK_DPC_FocalLengthMax, (uint32_t &)buf);

            Serial.print("Max focal length : ");
            Serial.print(buf / 100);
            Serial.println("mm");

#elif CHANGE_TEST == 20  // Set/Get Exposure Bias Compensation

            int sizeList = sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[0]);
            int indexP = incrementPool % sizeList;

            uint16_t rep = SetDevicePropValue(PTP_DPC_ExposureBiasCompensation, LIST_EXP_BIAS_COMP[indexP].value);
            if (rep != PTP_RC_OK) {
                Serial.print("error, rc: ");
                Serial.println(rep);
            }

            Serial.print("Exposure Bias Compensation change to: ");
            Serial.print(LIST_EXP_BIAS_COMP[indexP].name);

            uint8_t buf;
            GetDevicePropValue(PTP_DPC_ExposureBiasCompensation, (uint8_t &)buf);

            for (int i = 0; i < sizeList; i++) {
                if (LIST_EXP_BIAS_COMP[i].value == buf) {
                    Serial.print(" ->rep: ");
                    Serial.println(LIST_EXP_BIAS_COMP[i].name);
                }
            }

#elif CHANGE_TEST == 21  // Get Live View Status

            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

#elif CHANGE_TEST == 22  // Enable/Disable LiveView

            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

            if (buf == 0) {
                uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
            } else {
                uint16_t ret = Operation(PTP_OC_NIKON_EndLiveView, 0, NULL);
            }

#elif CHANGE_TEST == 23  // Capture Live View

            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

            if (buf == 0) {
                uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
            } else {
                MyDump myDump;
                HexDump hex;

                GetLiveViewImage(&imageDump);
                //delay(1000);
            }

#elif CHANGE_TEST == 24  // MoveFocus

            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

            if (buf == 0) {
                uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
            } else {
                uint8_t direction = (incrementPool % 2) + 1;
                MoveFocus(direction, 5000);
            }

#elif CHANGE_TEST == 25  // Capture

            //uint8_t buf;
            //GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            //Serial.print("Live View Status : ");
            //Serial.println(buf);
            /*
            if (buf == 0) {
                uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
            } else {*/
            //uint16_t ret = InitiateCapture();
            uint16_t ret = CaptureImage();
            Serial.print("ret: ");
            Serial.println(ret);
            //}

#elif CHANGE_TEST == 26  // Move Focus Live View Area

            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

            uint16_t ret = 0;

            if (buf == 0) {
                ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
                delay(500);
            } else {
                uint32_t coordinate[2] = {4500, 1000};
                if (incrementPool % 2 == 0) {
                    Serial.println("coordinate 1");
                    coordinate[0] = 2000;
                    coordinate[1] = 1000;
                } else if (incrementPool % 2 == 1) {
                    Serial.println("coordinate 2");
                    coordinate[0] = 5000;
                    coordinate[1] = 3000;
                }

                do {
                    ret = Operation(PTP_OC_NIKON_ChangeAfArea, 2, coordinate);
                    Serial.print("Change AF Area: ");
                    Serial.println(ret, HEX);
                    delay(200);
                } while (ret == NK_RC_NotLiveView);

                ret = Operation(NK_OC_DeviceReady, 0, NULL);
                Serial.print("Ready : ");
                Serial.println(ret, HEX);
                delay(200);

                uint8_t tentativeFocus = 0;
                do {
                    ret = Operation(0x90C1, 0, NULL);
                    Serial.print("AfDRive: ");
                    Serial.println(ret, HEX);
                    delay(200);
                    tentativeFocus++;
                    if (tentativeFocus > 3) break;
                    do {
                        ret = Operation(NK_OC_DeviceReady, 0, NULL);
                        Serial.print("Ready : ");
                        Serial.println(ret, HEX);
                        delay(200);
                    } while (ret == PTP_RC_DeviceBusy);
                    Serial.println("Ready01");
                } while (ret == NK_RC_OutOfFocus);
                if (ret == NK_RC_OutOfFocus) {
                    Serial.println("Focus done, out of it");
                } else {
                    Serial.println("Focus done, succes !!!");
                }

                uint32_t paraCapture[2] = {0xFFFFFFFF, 0x00000000};
                ret = Operation(0x9207, 2, paraCapture);
                Serial.print("CAptureInit: ");
                Serial.println(ret, HEX);
                delay(200);

                do {
                    ret = Operation(NK_OC_DeviceReady, 0, NULL);
                    Serial.print("Ready2 : ");
                    Serial.println(ret, HEX);
                    delay(200);
                } while (ret == PTP_RC_DeviceBusy);
            }
#elif CHANGE_TEST == 27  // Check Event
            uint16_t bufEvent[6] = {};
            EventCheck(bufEvent);

            int sizeList = sizeof(LIST_EVENT) / sizeof(LIST_EVENT[0]);

            for (int i = 0; i < 6; i++) {
                if (bufEvent[i] != 0x000) {
                    Serial.printf("%04x -> ", bufEvent[i]);
                    bool nameFound = false;
                    for (int j = 0; j < sizeList; j++) {
                        if (LIST_EVENT[j].value == bufEvent[i]) {
                            Serial.println(LIST_EVENT[j].name);
                            nameFound = true;
                            break;
                        }
                    }
                    if (!nameFound) {
                        Serial.println("Event Undefinned");
                    }
                }
            }

#elif CHANGE_TEST == 28  // Bluetooth Send/Receive data

            unsigned char messageBl[] = "Salut, je suis l'esp32\n";
            SerialBT.write(messageBl, sizeof(messageBl) - 1);

            while (SerialBT.available()) {
                Serial.write(SerialBT.read());
            }

#elif CHANGE_TEST == 29  // Bluetooth Send/Receive data

            uint16_t bufEvent[6] = {};
            EventCheck(bufEvent);

            int sizeList = sizeof(LIST_EVENT) / sizeof(LIST_EVENT[0]);

            for (int i = 0; i < 6; i++) {
                if (bufEvent[i] != 0x000) {
                    Serial.printf("%04x -> ", bufEvent[i]);
                    bool nameFound = false;
                    for (int j = 0; j < sizeList; j++) {
                        if (LIST_EVENT[j].value == bufEvent[i]) {
                            Serial.println(LIST_EVENT[j].name);
                            unsigned char EventBl[10];
                            memcpy(EventBl, LIST_EVENT[j].name, strlen(LIST_EVENT[j].name));
                            memcpy(EventBl + strlen(LIST_EVENT[j].name), "\n", 1);
                            SerialBT.write(EventBl, strlen(LIST_EVENT[j].name) + 1);

                            nameFound = true;
                            break;
                        }
                    }
                    if (!nameFound) {
                        Serial.println("Event Undefinned");
                    }
                }
            }

            while (SerialBT.available()) {
                Serial.write(SerialBT.read());
            }
#elif CHANGE_TEST == 30

            bool receivedSomething = false;
            while (SerialBT.available()) {
                int received = SerialBT.read();
                Serial.write(received);
                receivedSomething = true;
            }

            if (receivedSomething) {
                uint8_t buf;
                GetDevicePropValue(0xD1A2, (uint8_t &)buf);

                Serial.print("Live View Status : ");
                Serial.println(buf);

                if (buf == 0) {
                    uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
                } else {
                    MyDump myDump;
                    HexDump hex;

                    GetLiveViewImage(&imageDump);
                    //delay(1000);
                }
            }

#endif

            incrementPool++;
            nextPollTime = millis() + 1000;
        }

        return 0;
    };
};

CamStateHandlers CamStates;
USB Usb;
USBHub Hub1(&Usb);
//PTP ptp(&Usb, &CamStates)
Nikon Nik(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp
                                                 __attribute__((unused))) {
    if (stateConnected == stConnected || stateConnected == stInitial) {
        ((Nikon *)ptp)->bPollEnabled = false;
        stateConnected = stDisconnected;
        E_Notify(PSTR("\r\nDevice disconnected.\r\n"), 0x80);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp
                                                __attribute__((unused))) {
    if (stateConnected == stDisconnected || stateConnected == stInitial) {
        stateConnected = stConnected;
        E_Notify(PSTR("\r\nDevice connected.\r\n"), 0x80);
        ((Nikon *)ptp)->bPollEnabled = true;

    }  // if (stateConnected == stDisconnected || stateConnected == stInitial)
}

void setup() {
    Serial.begin(115200);
    Serial.println("Start");
    SerialBT.begin("ESP32test");  //Bluetooth device name
    imageDump.SetBl(&SerialBT);
    Serial.print("timeout: ");
    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);
}

void loop() {
    Usb.Task();
}
