#include "main.h"

#define HDR_MODE 0
#define FOCUS_MODE 1
#define TIMELAPSE_MODE 2

#define HDR_PARA_NBR_PHOTO 0
#define HDR_PARA_EXP_PAS 1
#define HDR_PARA_EXP_OFFSET 2

#define TIMELAPSE_PARA_FIX 0
#define TIMELAPSE_PARA_INITIAL_LENGTH 1
#define TIMELAPSE_PARA_FINAL_LENGTH 2
#define TIMELAPSE_PARA_FPS 3
#define TIMELAPSE_PARA_MULT_FAC 1
#define TIMELAPSE_PARA_MULT_FPS 2
#define TIMELAPSE_PARA_RAW_DELAY 1

QueueHandle_t queueCamMod = nullptr;

void modCamCode(void *pvParameters) {

    Nikon *nikon;
    nikon = (Nikon *)pvParameters;

    queueCamMod = xQueueCreate(10, sizeof(int));

    while (queueCamMod == NULL) {
        Serial.println("Error creating the queueCamMod");
    }

    static int indexMod = HDR_MODE;                //0: HDR, 1: Focus, 2: Timelapse
    static int paraHdrIndex = HDR_PARA_NBR_PHOTO;  //0: nbr photos, 1: exp pas, 2: exp offset
    int indexNbrOfPhoto = 0;
    int indexExpPas = 2;
    int indexExpOffset = 15;

    int paraTimeLapseIndex = 0;     //0 paraFix
    int indexParaFixTimeLapse = 0;  //0: initial/final length, 1: mult. factor, 2: Raw delay
    int initLength = 60;
    int finalLength = 10;
    int fps = 24;
    bool timelapseEnable = false;
    int rawDelay = 2000;
    int multFactor = 10;

    while (1) {
        int element = 0;
        if (xQueueReceive(queueCamMod, &element, portMAX_DELAY)) {
            switch (element) {
                case 0:  // Change Mode
                {
                    indexMod = ((indexMod + 1) % 3);    //0: HDR, 1: Focus, 2: Timelapse
                    int oledChangeMod = indexMod + 10;  //10: HDR, 11: Focus, 12: Timelapse
                    paraHdrIndex = 0;
                    paraTimeLapseIndex = 0;

                    if (oledChangeMod == 10 && nikon->queueOledCmd != nullptr) {  // HDR Mode
                        xQueueSend(nikon->queueOledCmd, &oledChangeMod, 0);

                        int oledResponseNbrPhotoValue = 16;
                        xQueueSend(nikon->queueOledCmd, &oledResponseNbrPhotoValue, 0);
                        xQueueSend(nikon->queueOledCmd, &indexNbrOfPhoto, 0);

                        int oledResponseExpoPasValue = 17;
                        xQueueSend(nikon->queueOledCmd, &oledResponseExpoPasValue, 0);
                        xQueueSend(nikon->queueOledCmd, &indexExpPas, 0);

                        int oledResponseExpoOffsetValue = 18;
                        xQueueSend(nikon->queueOledCmd, &oledResponseExpoOffsetValue, 0);
                        xQueueSend(nikon->queueOledCmd, &indexExpOffset, 0);
                    } else if (oledChangeMod == 12 && nikon->queueOledCmd != nullptr) {  // TimeLapse
                        xQueueSend(nikon->queueOledCmd, &oledChangeMod, 0);

                        int oledResponseParaFixValue = 23;
                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFixValue, 0);
                        xQueueSend(nikon->queueOledCmd, &indexParaFixTimeLapse, 0);

                        int oledResponseParaInitLen = 24;
                        xQueueSend(nikon->queueOledCmd, &oledResponseParaInitLen, 0);
                        xQueueSend(nikon->queueOledCmd, &initLength, 0);

                        int oledResponseParaFinalLen = 25;
                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFinalLen, 0);
                        xQueueSend(nikon->queueOledCmd, &finalLength, 0);

                        int oledResponseParaFps = 26;
                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                        xQueueSend(nikon->queueOledCmd, &fps, 0);

                    } else {
                        if (nikon->queueOledCmd != nullptr)
                            xQueueSend(nikon->queueOledCmd, &oledChangeMod, 0);
                    }
                    break;
                }

                case 1:  // Para +
                {
                    if (indexMod == HDR_MODE) {
                        paraHdrIndex = (paraHdrIndex + 1) % 3;
                        switch (paraHdrIndex) {
                            case HDR_PARA_NBR_PHOTO: {
                                int oledResponseNbrPhoto = 13;
                                if (nikon->queueOledCmd != nullptr)
                                    xQueueSend(nikon->queueOledCmd, &oledResponseNbrPhoto, 0);
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpPas = 14;
                                if (nikon->queueOledCmd != nullptr)
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpPas, 0);
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpOffset = 15;
                                if (nikon->queueOledCmd != nullptr)
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpOffset, 0);
                                break;
                            }
                            default:
                                break;
                        }
                    } else if (indexMod == TIMELAPSE_MODE) {
                        if (indexParaFixTimeLapse == 0) {  // initial/final length
                            paraTimeLapseIndex = (paraTimeLapseIndex + 1) % 4;

                            switch (paraTimeLapseIndex) {
                                case TIMELAPSE_PARA_FIX: {
                                    int oledResponseParaFix = 19;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFix, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_INITIAL_LENGTH: {
                                    int oledResponseInitLen = 20;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseInitLen, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_FINAL_LENGTH: {
                                    int oledResponseFinalLen = 21;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseFinalLen, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_FPS: {
                                    int oledResponseFps = 22;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseFps, 0);
                                    break;
                                }
                                default:
                                    break;
                            }

                        } else if (indexParaFixTimeLapse == 1) {  // mult factor
                            paraTimeLapseIndex = (paraTimeLapseIndex + 1) % 3;

                            switch (paraTimeLapseIndex) {
                                case TIMELAPSE_PARA_FIX: {
                                    int oledResponseParaFix = 19;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFix, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_MULT_FAC: {
                                    int oledResponseMultFac = 29;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseMultFac, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_MULT_FPS: {
                                    int oledResponseMultFps = 30;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseMultFps, 0);
                                    break;
                                }
                                default:
                                    break;
                            }
                        } else if (indexParaFixTimeLapse == 2) {  // raw delay
                            paraTimeLapseIndex = (paraTimeLapseIndex + 1) % 2;

                            switch (paraTimeLapseIndex) {
                                case TIMELAPSE_PARA_FIX: {
                                    int oledResponseParaFix = 32;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFix, 0);
                                    break;
                                }
                                case TIMELAPSE_PARA_RAW_DELAY: {
                                    int oledResponseRawDelay = 31;
                                    if (nikon->queueOledCmd != nullptr)
                                        xQueueSend(nikon->queueOledCmd, &oledResponseRawDelay, 0);
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                    }
                    break;
                }
                case 2:  // Value -
                {
                    if (indexMod == HDR_MODE) {
                        switch (paraHdrIndex) {
                            case HDR_PARA_NBR_PHOTO: {
                                int oledResponseNbrPhotoValue = 16;
                                indexNbrOfPhoto = _max(indexNbrOfPhoto - 1, 0);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseNbrPhotoValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexNbrOfPhoto, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpoPasValue = 17;
                                indexExpPas = _max(indexExpPas - 1, 0);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpoPasValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexExpPas, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpoOffsetValue = 18;
                                indexExpOffset = _max(indexExpOffset - 1, 0);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpoOffsetValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexExpOffset, 0);
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    } else if (indexMod == TIMELAPSE_MODE) {
                        switch (paraTimeLapseIndex) {
                            case TIMELAPSE_PARA_FIX: {
                                //if (indexParaFixTimeLapse == 0)
                                //    break;
                                int oledResponseParaFixValue = 23;
                                indexParaFixTimeLapse = _max(indexParaFixTimeLapse - 1, 0);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseParaFixValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexParaFixTimeLapse, 0);
                                    if (indexParaFixTimeLapse == 0) {
                                        int oledResponseParaFixValue = 23;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFixValue, 0);
                                        xQueueSend(nikon->queueOledCmd, &indexParaFixTimeLapse, 0);

                                        int oledResponseParaInitLen = 24;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaInitLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &initLength, 0);

                                        int oledResponseParaFinalLen = 25;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFinalLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &finalLength, 0);

                                        int oledResponseParaFps = 26;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    } else if (indexParaFixTimeLapse == 2) {
                                        Serial.println("indexParaFixTimeLapse==2");
                                        int oledResponseParaRawDelay = 33;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaRawDelay, 0);
                                        xQueueSend(nikon->queueOledCmd, &rawDelay, 0);
                                    } else if (indexParaFixTimeLapse == 1 && nikon->queueOledCmd != nullptr) {
                                        int oledResponseParaMultFac = 34;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaMultFac, 0);
                                        xQueueSend(nikon->queueOledCmd, &multFactor, 0);

                                        int oledResponseParaFps = 35;
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    }
                                }

                                break;
                            }

                            case 1: {
                                if (indexParaFixTimeLapse == 0) {  //initial length
                                    int oledResponseParaInitLen = 24;
                                    initLength = _max(initLength - 5, 5);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaInitLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &initLength, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 2) {  // Raw delay
                                    int oledResponseParaRawDelay = 33;
                                    rawDelay = _max(rawDelay - 500, 500);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaRawDelay, 0);
                                        xQueueSend(nikon->queueOledCmd, &rawDelay, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 1) {  // Mult factor
                                    int oledResponseParaMultFac = 34;
                                    multFactor = _max(multFactor - 1, 2);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaMultFac, 0);
                                        xQueueSend(nikon->queueOledCmd, &multFactor, 0);
                                    }
                                }

                                break;
                            }

                            case 2: {
                                if (indexParaFixTimeLapse == 0) {  //final length
                                    int oledResponseParaFinalLen = 25;
                                    finalLength = _max(finalLength - 5, 5);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFinalLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &finalLength, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 1) {  // Fps
                                    int oledResponseParaFps = 35;
                                    fps = _max(fps - 1, 1);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    }
                                }

                                break;
                            }

                            case 3: {
                                if (indexParaFixTimeLapse == 0) {  //fps
                                    int oledResponseParaFps = 26;
                                    fps = _max(fps - 1, 1);
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    }
                                }

                                break;
                            }

                            default:
                                break;
                        }
                    }
                    break;
                }
                case 3:  // Value +
                {
                    if (indexMod == HDR_MODE) {
                        switch (paraHdrIndex) {
                            case HDR_PARA_NBR_PHOTO: {
                                int oledResponseNbrPhotoValue = 16;
                                indexNbrOfPhoto = _min(indexNbrOfPhoto + 1, sizeof(HdrNbrPhotoArray) - 1);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseNbrPhotoValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexNbrOfPhoto, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpoPasValue = 17;
                                indexExpPas = _min(indexExpPas + 1, sizeof(HdrExpPasArray) / sizeof(float) - 1);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpoPasValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexExpPas, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpoOffsetValue = 18;
                                indexExpOffset = _min(indexExpOffset + 1, sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[1]) - 1);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseExpoOffsetValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexExpOffset, 0);
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    } else if (indexMod == TIMELAPSE_MODE) {
                        switch (paraTimeLapseIndex) {
                            case TIMELAPSE_PARA_FIX: {
                                //if (indexParaFixTimeLapse == 2)
                                //    break;
                                int oledResponseParaFixValue = 23;
                                indexParaFixTimeLapse = _min(indexParaFixTimeLapse + 1, 2);
                                if (nikon->queueOledCmd != nullptr) {
                                    xQueueSend(nikon->queueOledCmd, &oledResponseParaFixValue, 0);
                                    xQueueSend(nikon->queueOledCmd, &indexParaFixTimeLapse, 0);
                                }
                                if (indexParaFixTimeLapse == 2 && nikon->queueOledCmd != nullptr) {
                                    Serial.println("indexParaFixTimeLapse==2");
                                    int oledResponseParaRawDelay = 33;
                                    xQueueSend(nikon->queueOledCmd, &oledResponseParaRawDelay, 0);
                                    xQueueSend(nikon->queueOledCmd, &rawDelay, 0);
                                } else if (indexParaFixTimeLapse == 1 && nikon->queueOledCmd != nullptr) {
                                    int oledResponseParaMultFac = 34;
                                    xQueueSend(nikon->queueOledCmd, &oledResponseParaMultFac, 0);
                                    xQueueSend(nikon->queueOledCmd, &multFactor, 0);

                                    int oledResponseParaFps = 35;
                                    xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                    xQueueSend(nikon->queueOledCmd, &fps, 0);
                                }
                                break;
                            }

                            case 1: {
                                if (indexParaFixTimeLapse == 0) {  //initial final length
                                    int oledResponseParaInitLen = 24;
                                    initLength += 5;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaInitLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &initLength, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 2) {  // Raw delay
                                    int oledResponseParaRawDelay = 33;
                                    rawDelay += 500;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaRawDelay, 0);
                                        xQueueSend(nikon->queueOledCmd, &rawDelay, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 1) {  // Mult factor
                                    int oledResponseParaMultFac = 34;
                                    multFactor++;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaMultFac, 0);
                                        xQueueSend(nikon->queueOledCmd, &multFactor, 0);
                                    }
                                }

                                break;
                            }

                            case 2: {
                                if (indexParaFixTimeLapse == 0) {  //final length
                                    int oledResponseParaFinalLen = 25;
                                    finalLength += 5;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFinalLen, 0);
                                        xQueueSend(nikon->queueOledCmd, &finalLength, 0);
                                    }
                                } else if (indexParaFixTimeLapse == 1) {  // Fps
                                    int oledResponseParaFps = 35;
                                    fps++;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    }
                                }

                                break;
                            }

                            case 3: {
                                Serial.println("fps ++");
                                if (indexParaFixTimeLapse == 0) {  //fps
                                    int oledResponseParaFps = 26;
                                    fps += 1;
                                    if (nikon->queueOledCmd != nullptr) {
                                        xQueueSend(nikon->queueOledCmd, &oledResponseParaFps, 0);
                                        xQueueSend(nikon->queueOledCmd, &fps, 0);
                                    }
                                }

                                break;
                            }

                            default:
                                break;
                        }
                    }
                    break;
                }
                case 4:  // Take photo !!
                {
                    switch (indexMod) {
                        case HDR_MODE: {
                            Serial.println("Take HDR!");
                            static int takingHdr = false;
                            if (!takingHdr) {
                                COMMANDE_DSLR commande;
                                commande.para1 = 0;
                                commande.para2 = HdrNbrPhotoArray[indexNbrOfPhoto];  // 3 photos
                                commande.para3 = 10 * HdrExpPasArray[indexExpPas];   //2 => 1.00*10 = 10
                                commande.para4 = indexExpOffset;

                                xQueueSend(queueCmdDslr, &commande, 0);
                            }
                            break;
                        }
                        case FOCUS_MODE: {
                            COMMANDE_DSLR commande;
                            commande.para1 = 99;
                            xQueueSend(queueCmdDslr, &commande, 0);

                            break;
                        }
                        case TIMELAPSE_MODE: {
                            if (timelapseEnable) {
                                int oledResponseStopTimeLapse = 27;
                                xQueueSend(nikon->queueOledCmd, &oledResponseStopTimeLapse, 0);

                                COMMANDE_DSLR commande;
                                commande.para1 = 1;  // Stop timelapse
                                xQueueSend(queueCmdDslr, &commande, 0);
                                timelapseEnable = false;
                            } else if (indexParaFixTimeLapse == 0) {  // initial/final length
                                int delayTimelapse = 1000 * initLength / finalLength / fps;
                                Serial.printf("timerGoal %d\n", delayTimelapse);

                                int oledResponseStartTimeLapse = 28;
                                xQueueSend(nikon->queueOledCmd, &oledResponseStartTimeLapse, 0);

                                COMMANDE_DSLR commande;
                                commande.para1 = 2;  // Start timelapse
                                commande.para2 = delayTimelapse >> 8;
                                commande.para3 = delayTimelapse & 0xFF;
                                xQueueSend(queueCmdDslr, &commande, 0);
                                timelapseEnable = true;
                            } else if (indexParaFixTimeLapse == 1) {  // Mult factor
                                int delayTimelapse = 1000 / fps * multFactor;
                                Serial.printf("timerGoal %d\n", delayTimelapse);

                                int oledResponseStartTimeLapse = 28;
                                xQueueSend(nikon->queueOledCmd, &oledResponseStartTimeLapse, 0);

                                COMMANDE_DSLR commande;
                                commande.para1 = 2;  // Start timelapse
                                commande.para2 = delayTimelapse >> 8;
                                commande.para3 = delayTimelapse & 0xFF;
                                xQueueSend(queueCmdDslr, &commande, 0);
                                timelapseEnable = true;
                            } else if (indexParaFixTimeLapse == 2) {  // raw delay
                                int delayTimelapse = rawDelay;
                                Serial.printf("timerGoal %d\n", delayTimelapse);

                                int oledResponseStartTimeLapse = 28;
                                xQueueSend(nikon->queueOledCmd, &oledResponseStartTimeLapse, 0);

                                COMMANDE_DSLR commande;
                                commande.para1 = 2;  // Start timelapse
                                commande.para2 = delayTimelapse >> 8;
                                commande.para3 = delayTimelapse & 0xFF;
                                xQueueSend(queueCmdDslr, &commande, 0);
                                timelapseEnable = true;
                            }
                            break;
                        }
                        default:
                            break;
                    }

                    break;
                }
                case 5:  // BTN 1 DEBUG
                {
                    COMMANDE_DSLR commande;
                    commande.para1 = 99;  //DEBUG cmd
                    xQueueSend(queueCmdDslr, &commande, 0);
                    break;
                }
                default: {
                    Serial.printf("invalid queueCamMod: %d\n", element);
                    break;
                }
            }
        }
    }
}