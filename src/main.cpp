#include <Arduino.h>


#include <usbhub.h>

#include <ptp.h>
#include <ptpdebug.h>
#include <canonps.h>
#include <ptpdpparser.h>
#include <psvaluetitles.h>
#include "devpropparser.h"

class CamStateHandlers : public PTPStateHandlers
{
      enum CamStates { stInitial, stDisconnected, stConnected };
      CamStates stateConnected;

public:
      CamStateHandlers() : stateConnected(stInitial) {};

      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
};

CamStateHandlers  CamStates;
USB                 Usb;
USBHub              Hub1(&Usb);
PTP                 Ptp(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp
    __attribute__((unused)))
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        E_Notify(PSTR("Camera disconnected\r\n"), 0x80);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp
    __attribute__((unused)))
{
    if (stateConnected == stDisconnected || stateConnected == stInitial)
    {
        stateConnected = stConnected;
        E_Notify(PSTR("Camera connected\r\n"), 0x80);

        uint16_t    prefix[3] = { /* 0x5000 */ 0xD400, 0xD300, 0x5000 };

        for (uint8_t i=0; i<3; i++)
        {
            for (uint8_t j=0; j<128; j++)
            {

                HexDump          dmp;

                if (Ptp.GetDevicePropDesc((prefix[i] | j), &dmp) == PTP_RC_OK)
                {
                    E_Notify(PSTR("\r\n"), 0x80);

                    DevPropParser    prs;

            	    if (Ptp.GetDevicePropDesc((prefix[i] | j), &prs) == PTP_RC_OK)
                        E_Notify(PSTR("\r\n"), 0x80);
                }
            }

            E_Notify(PSTR("\r\n"), 0x80);

        } // for (uint8_t i=0; i<2; i++)

    }  // if (stateConnected == stDisconnected || stateConnected == stInitial)
}

void setup()
{
  Serial.begin( 115200 );
  Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

  delay( 200 );
}

void loop()
{
    Usb.Task();
}


/*#include <eoseventdump.h>
#include <nikon.h>
#include <ptp.h>
#include <ptpdebug.h>
#include <usbhub.h>

#include "devpropparser.h"

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

class Nikon : public NikonDSLR {
    uint32_t nextPollTime;  // Time of the next poll to occure

   public:
    bool bPollEnabled;  // Enables or disables camera poll

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
            */
            /*
            HexDump hex;
            EventCheck(&hex);

            delay(1000);
            //CaptureImage();
            uint16_t rep;

            HexDump hex2;
            //uint16_t ret = GetDevicePropValue(NK_DPC_ExposureTime, &hex2);  // ça marche!!!
            int32_t val2 = 0;
            uint16_t ret = GetDevicePropValue(PTP_DPC_FNumber, val2);
            Serial.print("val: ");
            Serial.println(val2);

            //            GetLiveViewImage(&hex);
            //MoveFocus(1, 10);

            //PrintPropValues(this);
            Serial.print("ret: ");
            Serial.println(ret);
*/
            /*uint16_t prefix[] = {*//* 0x5000*//* 0xD400, 0xD300, 0x5000};

            for (uint8_t i = 0; i < sizeof(prefix) / sizeof(prefix[0]); i++) {
                Serial.print("i: ");
                Serial.println(i);
                for (uint16_t j = 0; j < 128; j++) {
                    HexDump dmp;
                    if (GetDevicePropDesc((prefix[i] | j), &dmp) == PTP_RC_OK) {
                        E_Notify(PSTR("\r\n"), 0x80);

                        DevPropParser prs;
                        GetDevicePropDesc((prefix[i] | j), &prs);
                        //if (GetDevicePropDesc((prefix[i] | j), &prs) == PTP_RC_OK)
                         //   E_Notify(PSTR("\r\n"), 0x80);
                    }
                }

                E_Notify(PSTR("\r\n"), 0x80);
            }

            Serial.println("yes !!!!!");
            delay(50000);
            // for (uint8_t i=0; i<2; i++)
            /*
            //uint16_t listSpeed[] = {2000, 1600, 1250, 1000, 800, 640, 500, 400, 320, 250, 200, 160, 125, 100, 80, 60, 50, 40, 30, 25, 20, 15, 13, 10, 8, 6, 5, 4, 3, 2, 1};
            //uint32_t listSpeed[] = {67536, 67136, 65537, 67136, 655373, 655376, 655385, 851978, 1048586, 131073, 1638410, 196609};

            if (first_time) {
                for (size_t i = 0; i < sizeof(listSpeed) / sizeof(listSpeed[0]); i++) {
                    rep = SetDevicePropValue(NK_DPC_ExposureTime, listSpeed[i]);
                    Serial.print(listSpeed[i]);
                    Serial.print(" ->rep: ");
                    Serial.println(rep);
                    delay(200);
                }
                first_time = false;
            }*/

            /*
            uint32_t listAperture[] = {630, 560, 500, 450,2200};
            if (first_time) {
                for (size_t i = 0; i < sizeof(listAperture) / sizeof(listAperture[0]); i++) {
                    rep = SetDevicePropValue(PTP_DPC_FNumber, listAperture[i]);
                    Serial.print(listAperture[i]);
                    Serial.print(" ->rep: ");
                    Serial.println(rep);
                    delay(200);
                }
                first_time = false;
            }
            */
            /*
            for (int i = 0; i < 100000; i++) {
                rep = SetDevicePropValue(NK_DPC_ExposureTime, (uint8_t)i);
                Serial.print("rep: ");
                Serial.println(rep);
                if (rep != 8220) {
                    break;
                }
            }
            delay(10000);*/
            //rep = SetDevicePropValue(NK_DPC_LowLight, (uint8_t)1);
            //Serial.print("rep: ");
            //Serial.println(rep);
/*
            nextPollTime = millis() + 2000;
        }

        return 0;
    };
};

CamStateHandlers CamStates;
USB Usb;
USBHub Hub1(&Usb);
Nikon Nik(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp) {
    PTPTRACE("Disconnected\r\n");
    if (stateConnected == stConnected || stateConnected == stInitial) {
        ((Nikon *)ptp)->bPollEnabled = false;
        stateConnected = stDisconnected;
        E_Notify(PSTR("\r\nDevice disconnected.\r\n"), 0x80);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp) {
    if (stateConnected == stDisconnected || stateConnected == stInitial) {
        stateConnected = stConnected;
        E_Notify(PSTR("\r\nDevice connected.\r\n"), 0x80);
        ((Nikon *)ptp)->bPollEnabled = true;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);
}

void loop() {
    Usb.Task();
}
*/