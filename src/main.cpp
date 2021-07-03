#include <main.h>

#include "_icons/ble_blue.c"
#include "_icons/ble_orange.h"
#include "_icons/wifi.c"
SSD_13XX tft = SSD_13XX(CS_OLED, DC_OLED);
BluetoothSerial static SerialBT;
ImageDump imageDump;
int CountLvSend = 1;
QueueHandle_t queueOledCmd = nullptr;
QueueHandle_t queueCmdDslr = nullptr;

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

const uint8_t logoBB[] = {0x38, 0x6c, 0xe6, 0xea, 0xa6, 0xce, 0xce, 0xa6, 0xea, 0xe6, 0x6c, 0x38};
const uint8_t logoCam[] = {0x07, 0x00, 0x0f, 0x80, 0x78, 0xf0, 0xc0, 0x18, 0x87, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
                           0x87, 0x08, 0x80, 0x08, 0xff, 0xf8};

//#include <simplefifo.h>

/*CamStateHandlers CamStates;
USB Usb;
USBHub Hub1(&Usb);
PTP Ptp(&Usb, &CamStates);*/

void handleTimeLapse(Nikon &nikon);

unsigned long TIMER_TL = millis();
int NUMBER_TL = 0;
int TIMER_TIMELAPSE = 3000;  //ms
bool TL_ENABLE = false;
void handleTimeLapse(Nikon &nikon) {
    if (TL_ENABLE && millis() - TIMER_TL > TIMER_TIMELAPSE) {
        NUMBER_TL++;
        Serial.print("TimeLapse Photo:");
        Serial.println(NUMBER_TL);

        //prendre une photo
        nikon.waitCameraReady(3000);

        uint16_t ret = nikon.InitiateCapture();
        nikon.waitCameraReady(10000);
        Serial.print("capture: ");
        Serial.println(ret);
        nikon.waitCameraReady(3000);
        TIMER_TL = millis();
    }
}

uint8_t Nikon::Poll() {
    static bool first_time = true;
    PTP::Poll();

    if (!bPollEnabled)
        return 0;

    handleTimeLapse(*this);

    //if (first_time)
    //InitiateCapture();

    uint32_t current_time = millis();

    if (current_time >= nextPollTime) {
        //Serial.println("\r\n");
        //Serial.println("pool");

        bool receivedSomething = false;
        if (SerialBT.available()) {
            Serial.println("\r\n");
            Serial.print("received: ");
        }
        bool liveViewOn = false;
        while (SerialBT.available()) {
            int received = SerialBT.read();
            Serial.write(received);
            if (received == 'S') {
                setShutterSpeed(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'V') {
                setAperture(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'O') {
                setIso(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'I') {
                getInfo(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'X') {
                setExpoBias(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'D') {
                moveFocusAndroid(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'C') {
                takePhotoAndroid(*this, SerialBT);
                SerialBT.flush();
                delay(100);
            } else if (received == 'T') {
                //takeTimeLapse(*this, SerialBT);

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

                TIMER_TIMELAPSE = ((uint8_t)timerReceived[0] << 8) | (uint8_t)timerReceived[1];
                Serial.print("TIMER_TIMELAPSE:");
                Serial.println(TIMER_TIMELAPSE);
                NUMBER_TL = 0;
                TL_ENABLE = true;

                SerialBT.flush();
                delay(100);
            } else if (received == 'F') {
                if (SerialBT.available() >= 8) {
                    uint32_t posFocus[2] = {};

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
                    tryFocus(*this, SerialBT, posFocus);
                    SerialBT.flush();
                    delay(100);
                } else {
                    Serial.println("focus, not enough parameters");
                }
            } else if (received == 'H') {
                if (SerialBT.available() >= 4) {
                    uint8_t hdrParams[3] = {};

                    hdrParams[0] = SerialBT.read();
                    hdrParams[1] = SerialBT.read();
                    hdrParams[2] = SerialBT.read();

                    Serial.print("HDR: ");
                    Serial.print(hdrParams[0]);
                    Serial.print(" ");
                    Serial.print(hdrParams[1]);
                    Serial.print(" ");
                    Serial.println(hdrParams[2]);

                    takeHdr(*this, SerialBT, hdrParams);
                    SerialBT.flush();
                    delay(100);
                } else {
                    Serial.println("hdr, not enough parameters");
                }
            }

            if (received == 'A') {  // Start Live View
                liveViewOn = true;
            } else if (received == 'B') {  // Stop live View
                liveViewOn = false;
                stopLiveView(*this, SerialBT);
            }
        }

        if (liveViewOn && CountLvSend > 0) {
            CountLvSend--;
            Serial.println();
            uint8_t buf;
            GetDevicePropValue(0xD1A2, (uint8_t &)buf);

            Serial.print("Live View Status : ");
            Serial.println(buf);

            unsigned long imageSendTimer = millis();

            if (false) {
                uint8_t(*bufRaw) = new uint8_t[20000];
                bufRaw[0] = 0xFF;
                bufRaw[1] = 0xD8;
                bufRaw[2] = 0xFF;
                bufRaw[19998] = 0xFF;
                bufRaw[19999] = 0xD9;
                SerialBT.write(bufRaw, 20000);
                //Serial.println(SerialBT.flushWrite());
                //while (SerialBT.flushWrite())
                //    ;
                delete[] bufRaw;
            } else if (buf == 0) {
                uint16_t ret = Operation(PTP_OC_NIKON_StartLiveView, 0, NULL);
            } else {
                HexPersoDumper myDumpPerso;
                myDumpPerso.SetBl(&SerialBT);
                //HexDump hex;
                GetLiveViewImage(&myDumpPerso);
                //delay(1000);
            }

            unsigned long timer = millis();
            bool didWait = false;
            //unsigned int flushWrite = SerialBT.flushWrite();

            Serial.print("timer A: ");
            Serial.print(millis() - imageSendTimer);
            Serial.print("ms");

            if (CountLvSend < 1) {
                SerialBT.flush();
            }

            Serial.print(" || timer sent: ");
            Serial.print(millis() - imageSendTimer);
            Serial.println("ms");
            //Serial.println(SerialBT.flushWrite());
            CountLvSend++;
            //delay(250);
        }

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

                        if (strcmp("ExpTime", LIST_EVENT[j].name) == 0) {
                            getShutterSpeed(*this, SerialBT);
                        } else if (strcmp("ExpPrgMod", LIST_EVENT[j].name) == 0) {
                            getExpositionMode(*this, SerialBT);
                        } else if (strcmp("Fnumber", LIST_EVENT[j].name) == 0) {
                            getAperture(*this, SerialBT);
                        } else if (strcmp("IsoCtlSen", LIST_EVENT[j].name) == 0) {
                            getIso(*this, SerialBT);
                        }

                        break;
                    }
                }
                if (!nameFound) {
                    Serial.println("Event Undefinned");
                }
            }
        }

        COMMANDE_DSLR commande;
        if (xQueueReceive(queueCmdDslr, &commande, 0)) {
            if (commande.para1 == 0) {  //Take Hdr
                uint8_t hdrParams[3] = {commande.para2, commande.para3, commande.para4};

                Serial.print("HDR: ");
                Serial.print(hdrParams[0]);
                Serial.print(" ");
                Serial.print(hdrParams[1]);
                Serial.print(" ");
                Serial.println(hdrParams[2]);

                takeHdr(*this, SerialBT, hdrParams);
            }
        }

        /*if (numberTouchPressed > 0 && millis() - timerTouchPin > 500) {
            Serial.print("Touched: ");
            Serial.println(numberTouchPressed);

            if (numberTouchPressed > 0) {
                uint8_t hdrParams[3] = {3, 10, 15};
                takeHdr(*this, SerialBT, hdrParams);
            }

            numberTouchPressed = 0;
        }*/

        incrementPool++;
        nextPollTime = millis() + 100;
    }

    return 0;
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
        const int deviceDisconnectedCode = 7;
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &deviceDisconnectedCode, 0);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp
                                                __attribute__((unused))) {
    if (stateConnected == stDisconnected || stateConnected == stInitial) {
        stateConnected = stConnected;
        E_Notify(PSTR("\r\nDevice connected.\r\n"), 0x80);
        ((Nikon *)ptp)->bPollEnabled = true;

        const int deviceConnectedCode = 6;
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &deviceConnectedCode, 0);

    }  // if (stateConnected == stDisconnected || stateConnected == stInitial)
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    const int btHasClientCode = 8;
    const int btNoClientCode = 9;
    if (event == ESP_SPP_SRV_OPEN_EVT) {
        Serial.println("Client Connected");
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &btHasClientCode, 0);
    } else if (event == ESP_SPP_CLOSE_EVT) {
        Serial.println("Client Disconnected");
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &btNoClientCode, 0);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    SerialBT.register_callback(callback);
    SerialBT.begin("ESP32test");  //Bluetooth device name
    imageDump.SetBl(&SerialBT);
    Serial.print("timeout: ");
    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);
    SerialBT.flush();

    tft.begin();

    uint8_t errorCode = 0;
    errorCode = tft.getErrorCode();
    if (errorCode != 0) {
        Serial.print("Init error! ");
        if (bitRead(errorCode, 0)) Serial.print("MOSI or SCLK pin mismach!\n");
        if (bitRead(errorCode, 1)) Serial.print("CS or DC pin mismach!\n");
    } else {
        tft.print("Ready!");
        Serial.print("Ready! ");
    }

    queueCmdDslr = xQueueCreate(10, sizeof(COMMANDE_DSLR));

    xTaskCreatePinnedToCore(
        oledCode,        // Task function.
        "oledFunction",  // name of task.
        10000,           // Stack size of task
        NULL,            // parameter of the task
        1,               // priority of the task
        NULL,            // Task handle to keep track of created task
        1);

    xTaskCreatePinnedToCore(
        batterieLevelCode,        // Task function.
        "batterieLevelFunction",  // name of task.
        1000,                     // Stack size of task
        NULL,                     // parameter of the task
        1,                        // priority of the task
        NULL,                     // Task handle to keep track of created task
        1);

    xTaskCreatePinnedToCore(
        modCamCode,        // Task function.
        "modCamFunction",  // name of task.
        10000,             // Stack size of task
        NULL,              // parameter of the task
        1,                 // priority of the task
        NULL,              // Task handle to keep track of created task
        1);

    pinMode(14, INPUT_PULLUP);
    attachInterrupt(14, isr_1, FALLING);

    pinMode(27, INPUT_PULLUP);
    attachInterrupt(27, isr_2, FALLING);

    pinMode(26, INPUT_PULLUP);
    attachInterrupt(26, isr_3, FALLING);

    pinMode(25, INPUT_PULLUP);
    attachInterrupt(25, isr_4, FALLING);

    pinMode(33, INPUT_PULLUP);
    attachInterrupt(33, isr_5, FALLING);
}

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void batterieLevelCode(void *pvParameters) {
    while (1) {
        delay(5000);
        const int batterieCode = 5;
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &batterieCode, 0);
    }
}

void oledCode(void *pvParameters) {
    pinMode(34, INPUT);
    queueOledCmd = xQueueCreate(10, sizeof(int));

    while (queueOledCmd == NULL) {
        Serial.println("Error creating the queueOledCmd");
    }

    tft.clearScreen();
    tft.fillScreen(BLACK);

    uint16_t tftHeight = tft.height();  //64
    uint16_t tftWidth = tft.width();    //96

    tft.fillRect(61, 0, 21, 12, BLACK);  //Zone batterie + cam + BT

    tft.drawBitmap(89, 0, logoBB, 7, 12, WHITE);
    tft.drawBitmap(75, 1, logoCam, 13, 11, WHITE);

    tft.drawLine(63, 5, 72, 5, WHITE);
    tft.drawLine(63, 11, 72, 11, WHITE);
    tft.drawLine(62, 6, 62, 10, WHITE);
    tft.drawLine(73, 6, 73, 10, WHITE);
    tft.drawLine(61, 7, 61, 9, WHITE);

    const char modArray[3][10] = {"HDR", "FOCUS", "TIMELAPSE"};
    tft.print(modArray[0]);
    //int modCam = 0;
    //static int paraHdrIndex = 0;
    //int nbrOfPhoto = 3;
    //float expPas = 1.0;
    //float expOffset = 0.33;

    int initHdrMode = 10;
    xQueueSend(queueOledCmd, &initHdrMode, 0);

    while (1) {
        int element = 0;
        if (xQueueReceive(queueOledCmd, &element, portMAX_DELAY)) {
            if (element == 1) {  // Changement mode

                /*tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(YELLOW);
                tft.print(modArray[indexMod]);*/
            } else if (element == 5) {  // update batterie Value
                tft.fillRect(0, 55, 32, 7, BLACK);
                tft.setCursor(0, 55);
                int batteryValue = analogRead(34);
                tft.setTextColor(RED);
                tft.print(batteryValue);
                fillBatteryIcon(batteryValue);
            } else if (element == 6) {  // Camera connectée
                tft.drawBitmap(75, 1, logoCam, 13, 11, BLUE);
            } else if (element == 7) {  // Camera deconnectée
                tft.drawBitmap(75, 1, logoCam, 13, 11, WHITE);
            } else if (element == 8) {  // Bluetooth connecté
                tft.drawBitmap(89, 0, logoBB, 7, 12, BLUE);
            } else if (element == 9) {  // Bluetooth deconnecté
                tft.drawBitmap(89, 0, logoBB, 7, 12, WHITE);
            } else if (element == 10) {  // HDR mod
                tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(YELLOW);
                tft.print(modArray[0]);

                //tft.fillRect(0, 12, 96, 52, BLUE);
                //tft.fillRect(32, 12, 32, 52, GREEN);

                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 16);

                tft.printf("Nbr. of photos: %d", HdrNbrPhotoArray[0]);

                tft.setCursor(5, 31);
                tft.printf("Exp. Pas: %.2f", HdrExpPasArray[2]);

                tft.setCursor(5, 46);
                tft.printf("Exp. offset: %s", LIST_EXP_BIAS_COMP[15].name);

                tft.fillRect(2, 18, 2, 2, WHITE);
                tft.fillRect(2, 33, 2, 2, BLACK);
                tft.fillRect(2, 48, 2, 2, BLACK);

                //paraHdrIndex = 0;
                //modCam = 0;

            } else if (element == 11) {  // Focus mod
                tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(YELLOW);
                tft.print(modArray[1]);

                tft.fillRect(0, 12, 96, 52, BLACK);
                //modCam = 1;
            } else if (element == 12) {  // Timelapse mod
                tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(YELLOW);
                tft.print(modArray[2]);
                tft.fillRect(0, 12, 96, 52, BLACK);
                //modCam = 2;
            } else if (element == 13) {  // Change para HDR - Nbr Photo

                tft.fillRect(2, 18, 2, 2, WHITE);
                tft.fillRect(2, 33, 2, 2, BLACK);
                tft.fillRect(2, 48, 2, 2, BLACK);

            } else if (element == 14) {  // Change para HDR - Exp Pas

                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(2, 33, 2, 2, WHITE);
                tft.fillRect(2, 48, 2, 2, BLACK);

            } else if (element == 15) {  // Change para HDR - Exp Offset

                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(2, 33, 2, 2, BLACK);
                tft.fillRect(2, 48, 2, 2, WHITE);

            } else if (element == 16) {  // Value HDR - Nbr Photo

                int indexNbrPhoto = 0;
                xQueueReceive(queueOledCmd, &indexNbrPhoto, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 16, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 16);
                tft.printf("Nbr. of photos: %d", HdrNbrPhotoArray[indexNbrPhoto]);
            } else if (element == 17) {  // Value HDR - Exp Pas

                int indexExpPas = 0;
                xQueueReceive(queueOledCmd, &indexExpPas, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 31, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 31);
                tft.printf("Exp. Pas: %.2f", HdrExpPasArray[indexExpPas]);
            } else if (element == 18) {  // Value HDR - Exp Offset

                int indexExpOffset = 0;
                xQueueReceive(queueOledCmd, &indexExpOffset, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 46, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 46);
                tft.printf("Exp. offset: %s", LIST_EXP_BIAS_COMP[indexExpOffset].name);

            } else {
                tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(GREEN);
                tft.print(element);
            }
        }
    }
}

void fillBatteryIcon(int value) {
    int valueProcessed = (value * 0.2141 - 312.5) / 10;
    tft.fillRect(63, 6, 10, 5, BLACK);
    switch (valueProcessed) {
        case 10:
            tft.drawLine(63, 6, 63, 10, GREEN);
        case 9:
            tft.drawLine(64, 6, 64, 10, GREEN);
        case 8:
            tft.drawLine(65, 6, 65, 10, GREENYELLOW);
        case 7:
            tft.drawLine(66, 6, 66, 10, GREENYELLOW);
        case 6:
            tft.drawLine(67, 6, 67, 10, YELLOW);
        case 5:
            tft.drawLine(68, 6, 68, 10, YELLOW);
        case 4:
            tft.drawLine(69, 6, 69, 10, ORANGE);
        case 3:
            tft.drawLine(70, 6, 70, 10, ORANGE);
        case 2:
            tft.drawLine(71, 6, 71, 10, RED);
        case 1:
            tft.drawLine(72, 6, 72, 10, RED);
        default:
            break;
    }
}

void oledCodeOld(void *pvParameters) {
    while (1) {
        tft.clearScreen();
        tft.print("First");
        delay(500);

        tft.clearScreen();
        tft.print("Second");
        delay(500);

        tft.clearScreen();
        tft.fillScreen(RED);
        delay(500);
        tft.fillScreen(GREEN);
        delay(500);
        tft.fillScreen(BLUE);

        delay(500);

        tft.clearScreen();
        tft.setCursor(0, 0);
        tft.setTextColor(WHITE);
        tft.setTextScale(1);
        tft.println("Hello World!");
        tft.setTextColor(YELLOW);
        tft.setTextScale(2);
        tft.println(1234.56);
        tft.setTextColor(RED);
        tft.setTextScale(2);
        tft.println("ESP32");
        tft.setTextScale(1);
        tft.setTextColor(BLUE);
        tft.println("FULL COLOR!");
        tft.println();
        tft.setTextColor(GREEN);
        tft.setTextScale(4);
        tft.println("Hello");

        delay(500);

        tft.clearScreen();
        tft.setCursor(0, 0);
        tft.setTextColor(WHITE);
        tft.setTextScale(2);
        tft.println("I implore thee,");
        tft.setTextScale(1);
        tft.println("my foonting turlingdromes.");
        tft.println("And hooptiously drangle me");
        tft.println("with crinkly bindlewurdles,");
        tft.println("Or I will rend thee");
        tft.println("in the gobberwarts");
        tft.println("with my blurglecruncheon,");
        tft.println("see if I don't!");
        delay(2000);
    }
}

void loop() {
    Usb.Task();
}
