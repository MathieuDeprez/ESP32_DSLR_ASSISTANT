#ifndef _MAIN_H
#define _MAIN_H

#include <Arduino.h>
#include <SPI.h>
#include <SSD_13XX.h>
#include <nikon.h>
#include <ptp.h>
#include <ptpdebug.h>
#include <ptpdpparser.h>
#include <struct_list.h>
#include <usbhub.h>

#include "BluetoothSerial.h"

#define CS_OLED 17
#define DC_OLED 16

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//#include "devpropparser.h"

class Nikon : public NikonDSLR {
    uint32_t nextPollTime;  // Time of the next poll to occure

   public:
    bool bPollEnabled;  // Enables or disables camera poll
    int incrementPool = 0;

    Nikon(USB *pusb, PTPStateHandlers *pstates) : NikonDSLR(pusb, pstates), nextPollTime(0), bPollEnabled(false){};

    virtual uint8_t Poll();
};

extern valueNameStructUint16 LIST_ATPERTURE[19];
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

extern uint8_t HdrNbrPhotoArray[10];
extern float HdrExpPasArray[9];

extern QueueHandle_t queueCamMod;
extern QueueHandle_t queueCmdDslr;

void oledCode(void *pvParameters);
void batterieLevelCode(void *pvParameters);

void fillBatteryIcon(int value);
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

void IRAM_ATTR isr_1();
void IRAM_ATTR isr_2();
void IRAM_ATTR isr_3();
void IRAM_ATTR isr_4();
void IRAM_ATTR isr_5();

struct COMMANDE_DSLR {
    uint8_t para1;
    uint8_t para2;
    uint8_t para3;
    uint8_t para4;
    uint8_t para5;
};

void handleTimeLapse(Nikon &nikon);
void modCamCode(void *pvParameters);

#endif