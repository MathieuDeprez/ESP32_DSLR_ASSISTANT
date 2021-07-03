#include "main.h"

#define HDR_MODE 0
#define FOCUS_MODE 1
#define TIMELAPSE_MODE 2

#define HDR_PARA_NBR_PHOTO 0
#define HDR_PARA_EXP_PAS 1
#define HDR_PARA_EXP_OFFSET 2

QueueHandle_t queueCamMod = nullptr;

void modCamCode(void *pvParameters) {
    queueCamMod = xQueueCreate(10, sizeof(int));

    while (queueCamMod == NULL) {
        Serial.println("Error creating the queueCamMod");
    }

    static int indexMod = HDR_MODE;                //0: HDR, 1: Focus, 2: Timelapse
    static int paraHdrIndex = HDR_PARA_NBR_PHOTO;  //0: nbr photos, 1: exp pas, 2: exp offset
    int indexNbrOfPhoto = 0;
    int indexExpPas = 2;
    int indexExpOffset = 15;

    while (1) {
        int element = 0;
        if (xQueueReceive(queueCamMod, &element, portMAX_DELAY)) {
            switch (element) {
                case 0:  // Change Mode
                {
                    indexMod = ((indexMod + 1) % 3);    //0: HDR, 1: Focus, 2: Timelapse
                    int oledChangeMod = indexMod + 10;  //10: HDR, 11: Focus, 12: Timelapse
                    paraHdrIndex = 0;

                    if (oledChangeMod == 10 && queueOledCmd != nullptr) {
                        xQueueSend(queueOledCmd, &oledChangeMod, 0);

                        int oledResponseNbrPhotoValue = 16;
                        xQueueSend(queueOledCmd, &oledResponseNbrPhotoValue, 0);
                        xQueueSend(queueOledCmd, &indexNbrOfPhoto, 0);

                        int oledResponseExpoPasValue = 17;
                        xQueueSend(queueOledCmd, &oledResponseExpoPasValue, 0);
                        xQueueSend(queueOledCmd, &indexExpPas, 0);

                        int oledResponseExpoOffsetValue = 18;
                        xQueueSend(queueOledCmd, &oledResponseExpoOffsetValue, 0);
                        xQueueSend(queueOledCmd, &indexExpOffset, 0);
                    } else {
                        if (queueOledCmd != nullptr)
                            xQueueSend(queueOledCmd, &oledChangeMod, 0);
                    }
                    break;
                }

                case 1:  // Para +
                {
                    paraHdrIndex = (paraHdrIndex + 1) % 3;

                    if (indexMod == HDR_MODE) {
                        switch (paraHdrIndex) {
                            case HDR_PARA_NBR_PHOTO: {
                                int oledResponseNbrPhoto = 13;
                                if (queueOledCmd != nullptr)
                                    xQueueSend(queueOledCmd, &oledResponseNbrPhoto, 0);
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpPas = 14;
                                if (queueOledCmd != nullptr)
                                    xQueueSend(queueOledCmd, &oledResponseExpPas, 0);
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpOffset = 15;
                                if (queueOledCmd != nullptr)
                                    xQueueSend(queueOledCmd, &oledResponseExpOffset, 0);
                                break;
                            }
                            default:
                                break;
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
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseNbrPhotoValue, 0);
                                    xQueueSend(queueOledCmd, &indexNbrOfPhoto, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpoPasValue = 17;
                                indexExpPas = _max(indexExpPas - 1, 0);
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseExpoPasValue, 0);
                                    xQueueSend(queueOledCmd, &indexExpPas, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpoOffsetValue = 18;
                                indexExpOffset = _max(indexExpOffset - 1, 0);
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseExpoOffsetValue, 0);
                                    xQueueSend(queueOledCmd, &indexExpOffset, 0);
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
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseNbrPhotoValue, 0);
                                    xQueueSend(queueOledCmd, &indexNbrOfPhoto, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_PAS: {
                                int oledResponseExpoPasValue = 17;
                                indexExpPas = _min(indexExpPas + 1, sizeof(HdrExpPasArray) / sizeof(float) - 1);
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseExpoPasValue, 0);
                                    xQueueSend(queueOledCmd, &indexExpPas, 0);
                                }
                                break;
                            }
                            case HDR_PARA_EXP_OFFSET: {
                                int oledResponseExpoOffsetValue = 18;
                                indexExpOffset = _min(indexExpOffset + 1, sizeof(LIST_EXP_BIAS_COMP) / sizeof(LIST_EXP_BIAS_COMP[1]) - 1);
                                if (queueOledCmd != nullptr) {
                                    xQueueSend(queueOledCmd, &oledResponseExpoOffsetValue, 0);
                                    xQueueSend(queueOledCmd, &indexExpOffset, 0);
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
                            break;
                        }
                        case TIMELAPSE_MODE: {
                            break;
                        }
                        default:
                            break;
                    }

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