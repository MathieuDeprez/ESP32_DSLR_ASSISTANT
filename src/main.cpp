#include <main.h>

#include "_icons/ble_blue.c"
#include "_icons/ble_orange.h"
#include "_icons/wifi.c"
SSD_13XX tft = SSD_13XX(CS_OLED, DC_OLED);
//BluetoothSerial static SerialBT;

//QueueHandle_t queueOledCmd = nullptr;
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
char timeLapseParameterFixe[][21] = {"Initial/final length", "Mult. factor", "Raw delay"};

int NUMBER_TL = 0;
int TIMER_TIMELAPSE = 3000;  //ms
bool TL_ENABLE = false;
void handleTimeLapse(Nikon &nikon) {
    static unsigned long TIMER_TL = millis();
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
    PTP::Poll();

    if (!bPollEnabled)
        return 0;

    static bool setupPoll = true;
    if (setupPoll) {
        xTaskCreatePinnedToCore(
            oledCode,        // Task function.
            "oledFunction",  // name of task.
            10000,           // Stack size of task
            (void *)this,    // parameter of the task
            1,               // priority of the task
            NULL,            // Task handle to keep track of created task
            1);

        xTaskCreatePinnedToCore(
            modCamCode,        // Task function.
            "modCamFunction",  // name of task.
            10000,             // Stack size of task
            (void *)this,      // parameter of the task
            1,                 // priority of the task
            NULL,              // Task handle to keep track of created task
            1);
        setupPoll = false;
    }

    handleTimeLapse(*this);

    uint32_t current_time = millis();

    static unsigned long batteryTimer = millis();
    if (millis() - batteryTimer > 5000) {
        batteryTimer = millis();
        const int batterieCode = 5;
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &batterieCode, 0);
    }

    if (current_time >= nextPollTime) {
        if (SerialBT.available()) {  //On recoit quelque chose via Bluetooth
            Serial.println("\r\n");
            Serial.print("received: ");
        }
        bool liveViewOn = false;
        while (SerialBT.available()) {
            int received = SerialBT.read();
            Serial.println(received);
            if (received == static_cast<int>(MyBluetoothCode::ShutterSpeedIn)) {
                setShutterSpeed();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::ApertureIn)) {
                setAperture();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::IsoIn)) {
                setIso();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::InfoIn)) {
                getInfo();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::ExpoBiasIn)) {
                setExpoBias();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::MoveFocusIn)) {
                moveFocusAndroid();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::TakePhotoIn)) {
                takePhotoAndroid();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::TimelapseIn)) {
                takeTimeLapse(TIMER_TIMELAPSE);
                NUMBER_TL = 0;
                TL_ENABLE = true;
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::TimelapseStop)) {
                TIMER_TIMELAPSE = 1000;
                NUMBER_TL = 0;
                TL_ENABLE = false;
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::PosFocusIn)) {
                tryFocus();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::HdrIn)) {
                takeHdr();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::LongExpoIn)) {
                takeLongExposure();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::ThumbnailIn)) {
                downloadThumbnail();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::ListHandles)) {
                getAndSendListHandles();
                SerialBT.flush();
                delay(100);
            } else if (received == static_cast<int>(MyBluetoothCode::JpegIn)) {
                downloadJpeg();
                SerialBT.flush();
                delay(100);
                Serial.println("end downloadJpeg");

                /*Serial.println("realease");
                this->ResetDevice();
                this->pUsb->reset();
                if (this->pUsb->Init() == -1)
                    Serial.println("OSC did not start.");*/
                ESP.restart();
            } else if (received == static_cast<int>(MyBluetoothCode::JpegHqIn)) {
                downloadJpegHq();
                SerialBT.flush();
                delay(100);
                Serial.println("end downloadJpegHq");
                ESP.restart();
            }
            //LiveView
            if (received == static_cast<int>(MyBluetoothCode::LiveViewOn)) {  // Start Live View
                liveViewOn = true;
            } else if (received == static_cast<int>(MyBluetoothCode::LiveViewOff)) {  // Stop live View
                liveViewOn = false;
                stopLiveView();
            }
        }

        static int CountLvSend = 1;
        if (liveViewOn && CountLvSend > 0) {
            CountLvSend--;
            Serial.println();
            bool buf = getLiveView();

            Serial.print("Live View Status : ");
            Serial.println(buf);

            unsigned long imageSendTimer = millis();

            if (!buf) {  //If liveview is not yet activated, do it
                startLiveView();
            } else {  //Get an liveViewImage and send it to Android
                //HexPersoDumper myDumpPerso;
                //myDumpPerso.SetBl(&SerialBT);
                //HexDump hex;
                uint8_t *response;
                uint32_t responseLenght;
                GetLiveViewImageV2(response, responseLenght);

                Serial.printf("Get liveview image, len:%d\n", responseLenght);

                sendBluetoothData(MyBluetoothCode::LiveViewImageOut, response, responseLenght);

                delete[] response;
                //delay(1000);
            }

            Serial.print("timer A: ");
            Serial.print(millis() - imageSendTimer);
            Serial.print("ms");

            if (CountLvSend < 1) {
                SerialBT.flush();
            }

            Serial.print(" || timer sent: ");
            Serial.print(millis() - imageSendTimer);
            Serial.println("ms");
            CountLvSend++;
        }

        uint16_t bufEvent[6] = {};
        EventCheck(bufEvent);

        printEvent(bufEvent);

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

                takeHdr(hdrParams);
            } else if (commande.para1 == 1) {  //Stop timelapse
                TIMER_TIMELAPSE = 1000;
                NUMBER_TL = 0;
                TL_ENABLE = false;
            } else if (commande.para1 == 2) {
                TIMER_TIMELAPSE = ((uint8_t)commande.para2 << 8) | (uint8_t)commande.para3;

                //Serial.printf("para2: %d\n", commande.para2);
                //Serial.printf("para3: %d\n", commande.para3);
                Serial.printf("TIMER_TIMELAPSE: %d\n", TIMER_TIMELAPSE);
                NUMBER_TL = 0;
                TL_ENABLE = true;
            } else if (commande.para1 == 99) {
                Serial.println("Debug cmd received!");

                //Get thumb
                /*HexDump myDumpPerso;
                GetThumb(0x0919000e, &myDumpPerso);
*/
                //HexDump dmp;
                //GetObjectHandles(0xFFFFFFFF, 0, 0, &dmp);

                //ObjectHandlesDumper objectHandlesDumper;
                //objectHandlesDumper.SetBl(&SerialBT);

                uint8_t *response;
                uint32_t responseLenght;
                GetObjectHandlesV2(0xFFFFFFFF, 0, 0, response, responseLenght);

                /*Serial.printf("Response exported len(%d):\n", responseLenght);
                for (int i = 0; i < responseLenght; i++) {
                    Serial.printf("%02x ", response[i]);
                    if (!i % 16 && i)
                        Serial.println();
                }
                Serial.println();*/

                sendObjectHandlesBt(response, responseLenght);

                delete[] response;
            }
        }

        /*if (numberTouchPressed > 0 && millis() - timerTouchPin > 500) {
            Serial.print("Touched: ");
            Serial.println(numberTouchPressed);

            if (numberTouchPressed > 0) {
                uint8_t hdrParams[3] = {3, 10, 15};
                takeHdr(, hdrParams);
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
        Serial.println("\r\nDevice disconnected.\r\n");
        const int deviceDisconnectedCode = 7;
        if (ptp->queueOledCmd != nullptr)
            xQueueSend(ptp->queueOledCmd, &deviceDisconnectedCode, 0);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp
                                                __attribute__((unused))) {
    if (stateConnected == stDisconnected || stateConnected == stInitial) {
        stateConnected = stConnected;
        Serial.println("\r\nDevice connected.\r\n");
        ((Nikon *)ptp)->bPollEnabled = true;

        const int deviceConnectedCode = 6;
        if (ptp->queueOledCmd != nullptr)
            xQueueSend(ptp->queueOledCmd, &deviceConnectedCode, 0);
    }
}

/*void callbackBluetooth(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    const int btHasClientCode = 8;
    const int btNoClientCode = 9;
    if (event == ESP_SPP_SRV_OPEN_EVT) {
        Serial.println("Client Connected");
        if (ptp->queueOledCmd != nullptr)
            xQueueSend(ptp->queueOledCmd, &btHasClientCode, 0);
    } else if (event == ESP_SPP_CLOSE_EVT) {
        Serial.println("Client Disconnected");
        if (ptp->queueOledCmd != nullptr)
            xQueueSend(ptp->queueOledCmd, &btNoClientCode, 0);
    }
}*/

void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    std::string myStdString = "monStdString";
    Serial.println(myStdString.c_str());

    //delay(500);
    Serial.println("START 500ms");

    Serial.printf("\033[3;31m Some error\n\033[0m;");
    Serial.println("abc");
    Serial.printf("\033[1;31m Some error\n");
    Serial.println("abc");
    Serial.printf("\033[2;32m Some error\n\033[0m;");
    Serial.println("abc");
    Serial.printf("\033[1;31m Some error\n\033[0m;");
    Serial.println("abc");

    //Bluetooth init
    //SerialBT.register_callback(callbackBluetooth);
    //SerialBT.begin("ESP32test");  //Bluetooth device name

    //Usb init
    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);
    //SerialBT.flush();

    //delay(500);
    Serial.println("START 1s");

    //Display init
    tft.begin();
    tft.setRotation(2);

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

    /*xTaskCreatePinnedToCore(
        oledCode,        // Task function.
        "oledFunction",  // name of task.
        10000,           // Stack size of task
        NULL,            // parameter of the task
        1,               // priority of the task
        NULL,            // Task handle to keep track of created task
        1);*/

    /*xTaskCreatePinnedToCore(
        batterieLevelCode,        // Task function.
        "batterieLevelFunction",  // name of task.
        1000,                     // Stack size of task
        NULL,                     // parameter of the task
        1,                        // priority of the task
        NULL,                     // Task handle to keep track of created task
        1);*/

    /*xTaskCreatePinnedToCore(
        modCamCode,        // Task function.
        "modCamFunction",  // name of task.
        10000,             // Stack size of task
        NULL,              // parameter of the task
        1,                 // priority of the task
        NULL,              // Task handle to keep track of created task
        1);*/

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

/*void batterieLevelCode(void *pvParameters) {
    while (1) {
        delay(5000);
        const int batterieCode = 5;
        if (queueOledCmd != nullptr)
            xQueueSend(queueOledCmd, &batterieCode, 0);
    }
}*/

void oledCode(void *pvParameters) {
    Nikon *nikon;
    nikon = (Nikon *)pvParameters;

    pinMode(34, INPUT);
    nikon->queueOledCmd = xQueueCreate(10, sizeof(int));

    while (nikon->queueOledCmd == NULL) {
        Serial.println("Error creating the queueOledCmd");
    }

    tft.clearScreen();
    tft.fillScreen(BLACK);

    //uint16_t tftHeight = tft.height();  //64
    //uint16_t tftWidth = tft.width();    //96

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
    xQueueSend(nikon->queueOledCmd, &initHdrMode, 0);

    while (1) {
        int element = 0;
        if (xQueueReceive(nikon->queueOledCmd, &element, portMAX_DELAY)) {
            if (element == 1) {  // Changement mode

                /*tft.fillRect(0, 0, 61, 12, BLACK);
                tft.setCursor(0, 0);
                tft.setTextScale(1);
                tft.setTextColor(YELLOW);
                tft.print(modArray[indexMod]);*/
            } else if (element == 5) {  // update batterie Value
                //tft.fillRect(0, 55, 32, 7, BLACK);
                //tft.setCursor(0, 55);
                int batteryValue = analogRead(34);
                //tft.setTextColor(RED);
                //tft.print(batteryValue);
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
                tft.fillRect(0, 12, 96, 52, BLACK);

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

                tft.setTextScale(1);
                tft.setTextColor(ORANGE);

                tft.setCursor(5, 16);
                tft.printf("%s", timeLapseParameterFixe[0]);

                tft.setTextColor(WHITE);

                tft.setCursor(5, 35);
                tft.printf("length: %d s", 60);
                tft.setCursor(58, 35);
                tft.printf("->");
                tft.setCursor(71, 35);
                tft.printf("%d s", 10);

                tft.setCursor(5, 50);
                tft.printf("Fps: %d", 24);

                tft.fillRect(2, 18, 2, 2, WHITE);
                tft.fillRect(35, 43, 17, 2, BLACK);
                tft.fillRect(75, 43, 17, 2, BLACK);
                tft.fillRect(2, 52, 2, 2, BLACK);

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
                xQueueReceive(nikon->queueOledCmd, &indexNbrPhoto, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 16, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 16);
                tft.printf("Nbr. of photos: %d", HdrNbrPhotoArray[indexNbrPhoto]);
            } else if (element == 17) {  // Value HDR - Exp Pas

                int indexExpPas = 0;
                xQueueReceive(nikon->queueOledCmd, &indexExpPas, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 31, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 31);
                tft.printf("Exp. Pas: %.2f", HdrExpPasArray[indexExpPas]);
            } else if (element == 18) {  // Value HDR - Exp Offset

                int indexExpOffset = 0;
                xQueueReceive(nikon->queueOledCmd, &indexExpOffset, 100 / portTICK_PERIOD_MS);
                tft.fillRect(5, 46, 90, 9, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 46);
                tft.printf("Exp. offset: %s", LIST_EXP_BIAS_COMP[indexExpOffset].name);

            } else if (element == 19) {  // Change para TIMELAPSE - PARA FIX

                tft.fillRect(2, 18, 2, 2, WHITE);
                tft.fillRect(35, 43, 17, 2, BLACK);
                tft.fillRect(75, 43, 17, 2, BLACK);
                tft.fillRect(2, 52, 2, 2, BLACK);

            } else if (element == 20) {  // Change para TIMELAPSE - initial length

                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(35, 43, 17, 2, WHITE);
                tft.fillRect(75, 43, 17, 2, BLACK);
                tft.fillRect(2, 52, 2, 2, BLACK);

            } else if (element == 21) {  // Change para TIMELAPSE - final length

                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(35, 43, 17, 2, BLACK);
                tft.fillRect(75, 43, 17, 2, WHITE);
                tft.fillRect(2, 52, 2, 2, BLACK);

            } else if (element == 22) {  // Change para TIMELAPSE - fps

                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(35, 43, 17, 2, BLACK);
                tft.fillRect(75, 43, 17, 2, BLACK);
                tft.fillRect(2, 52, 2, 2, WHITE);

            } else if (element == 23) {  // Change value timelapse para fix

                int indexParaFix = 0;
                xQueueReceive(nikon->queueOledCmd, &indexParaFix, 100 / portTICK_PERIOD_MS);

                tft.fillRect(0, 12, 96, 52, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(ORANGE);

                tft.setCursor(5, 16);
                tft.printf("%s", timeLapseParameterFixe[indexParaFix]);

                tft.setTextColor(WHITE);

                switch (indexParaFix) {
                    case 0: {
                        tft.setCursor(5, 35);
                        tft.printf("length: %d s", 60);
                        tft.setCursor(60, 35);
                        tft.printf("->");
                        tft.setCursor(75, 35);
                        tft.printf("%d s", 10);

                        tft.setCursor(5, 50);
                        tft.printf("Fps: %d", 24);

                        tft.fillRect(2, 18, 2, 2, WHITE);
                        tft.fillRect(35, 43, 17, 2, BLACK);
                        tft.fillRect(75, 43, 17, 2, BLACK);
                        tft.fillRect(2, 52, 2, 2, BLACK);
                        break;
                    }
                    case 1: {
                        tft.setCursor(5, 35);
                        tft.printf("Multi factor: %d", 10);

                        tft.setCursor(5, 50);
                        tft.printf("Fps: %d", 24);

                        tft.fillRect(2, 18, 2, 2, WHITE);
                        tft.fillRect(2, 37, 2, 2, BLACK);
                        tft.fillRect(2, 52, 2, 2, BLACK);
                        break;
                    }
                    case 2: {
                        tft.setCursor(5, 40);
                        tft.printf("Raw delay:\n    %d ms", 2000);

                        tft.fillRect(2, 18, 2, 2, WHITE);
                        tft.fillRect(2, 42, 2, 2, BLACK);
                        break;
                    }
                    default:
                        break;
                }

            } else if (element == 24) {  // Change value timelapse init len
                int initLenValue = 0;
                xQueueReceive(nikon->queueOledCmd, &initLenValue, 100 / portTICK_PERIOD_MS);

                tft.fillRect(5, 35, 55, 8, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);

                tft.setCursor(5, 35);
                tft.printf("length: %d s", initLenValue);

            } else if (element == 25) {  // Change value timelapse final len
                int finalLenValue = 0;
                xQueueReceive(nikon->queueOledCmd, &finalLenValue, 100 / portTICK_PERIOD_MS);

                tft.fillRect(71, 35, 24, 8, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);
                tft.setCursor(71, 35);
                tft.printf("%d s", finalLenValue);

            } else if (element == 26) {  // Change value timelapse fps
                int fpsValue = 0;
                xQueueReceive(nikon->queueOledCmd, &fpsValue, 100 / portTICK_PERIOD_MS);

                tft.fillRect(5, 50, 50, 8, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);
                tft.setCursor(5, 50);
                tft.printf("Fps: %d", fpsValue);

            } else if (element == 27) {  // Stop Timelapse
                tft.fillRect(91, 60, 4, 4, RED);
            } else if (element == 28) {  // Stop Timelapse
                tft.fillRect(91, 60, 4, 4, GREEN);
            } else if (element == 29) {  // Timelapse para mult fac
                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(2, 37, 2, 2, WHITE);
                tft.fillRect(2, 52, 2, 2, BLACK);
            } else if (element == 30) {  // Timelapse para mult fps
                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(2, 37, 2, 2, BLACK);
                tft.fillRect(2, 52, 2, 2, WHITE);
            } else if (element == 31) {  // Timelapse para raw delay
                tft.fillRect(2, 18, 2, 2, BLACK);
                tft.fillRect(2, 42, 2, 2, WHITE);
            } else if (element == 32) {  // Timelapse para raw delay
                tft.fillRect(2, 18, 2, 2, WHITE);
                tft.fillRect(2, 42, 2, 2, BLACK);
            } else if (element == 33) {  // Timelapse para raw delay value

                int rawDelay = 0;
                xQueueReceive(nikon->queueOledCmd, &rawDelay, 100 / portTICK_PERIOD_MS);

                tft.fillRect(5, 40, 90, 20, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);
                tft.setCursor(5, 40);
                tft.printf("Raw delay:\n    %d ms", rawDelay);

            } else if (element == 34) {  // Timelapse mult factor value

                int multFactor = 0;
                xQueueReceive(nikon->queueOledCmd, &multFactor, 100 / portTICK_PERIOD_MS);

                tft.fillRect(5, 35, 90, 8, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);
                tft.setCursor(5, 35);
                tft.printf("Multi factor: %d", multFactor);

            } else if (element == 35) {  // Timelapse mult fps value
                int multFps = 0;
                xQueueReceive(nikon->queueOledCmd, &multFps, 100 / portTICK_PERIOD_MS);

                tft.fillRect(5, 50, 90, 8, BLACK);
                tft.setTextScale(1);
                tft.setTextColor(WHITE);
                tft.setCursor(5, 50);
                tft.printf("Fps: %d", multFps);

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
    int batteryLevel = (value * 0.2532 - 936.71) / 10;  //value == 3700 => minimum !!!

    //Serial.printf("analogBatterie: %d\n", value);
    //Serial.printf("batteryLevel: %d\n", batteryLevel);
    //int valueProcessed = (value * 0.2141 - 312.5) / 10;
    tft.fillRect(63, 6, 10, 5, BLACK);
    switch (batteryLevel) {
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
