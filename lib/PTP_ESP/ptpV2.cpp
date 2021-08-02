#include <main.h>

#include "ptp.h"
#include "ptpconst.h"
#include "ptpdebug.h"

void uint16_to_char_V2(uint16_t integer, unsigned char *data) {
    data[0] = (integer & 0xff);
    data[1] = ((integer >> 8) & 0xff);
}

void uint32_to_char_V2(uint32_t integer, unsigned char *data) {
    data[0] = (integer & 0xff);
    data[1] = ((integer >> 8) & 0xff);
    data[2] = ((integer >> 16) & 0xff);
    data[3] = ((integer >> 24) & 0xff);
}

uint16_t PTP::TransactionV2(MyOpCode opcode, MyParamBlock myParamBlock, MyDataBlock myDataBlock, uint8_t *&response, uint32_t &responseLenght, BluetoothSerial &SerialBT) {
    Serial.printf("TransactionV2, opCode: %04x\n", opcode);
    uint8_t rcode;

    //Send command block
    {
        uint8_t cmd[PTP_USB_BULK_HDR_LEN + 12];  // header + 3 uint32_t parameters
        ZerroMemory(PTP_USB_BULK_HDR_LEN + 12, cmd);

        // Make command PTP container header
        uint16_to_char_V2(PTP_USB_CONTAINER_COMMAND, (unsigned char *)(cmd + PTP_CONTAINER_CONTYPE_OFF));     // type
        uint16_to_char_V2(static_cast<uint16_t>(opcode), (unsigned char *)(cmd + PTP_CONTAINER_OPCODE_OFF));  // code
        uint32_to_char_V2(++idTransaction, (unsigned char *)(cmd + PTP_CONTAINER_TRANSID_OFF));               // transaction id

        uint8_t numberOfParam = myParamBlock.numberOfParam;  // number of parameters
        uint8_t len;

        //S'il y a des paramètres
        if (numberOfParam) {
            *((uint8_t *)cmd) = PTP_USB_BULK_HDR_LEN + (numberOfParam << 2);
            len = PTP_USB_BULK_HDR_LEN + (numberOfParam << 2);

            for (uint32_t *p1 = (uint32_t *)(cmd + PTP_CONTAINER_PAYLOAD_OFF), *p2 = (uint32_t *)myParamBlock.params; numberOfParam--; p1++, p2++) {
                uint32_to_char_V2(*p2, (unsigned char *)p1);
            }
        } else {
            *((uint8_t *)cmd) = PTP_USB_BULK_HDR_LEN;
            len = PTP_USB_BULK_HDR_LEN;
        }

        // Envoi de la commande
        rcode = pUsb->outTransfer(devAddress, epInfo[epDataOutIndex].epAddr, len, cmd);

        if (rcode) {  // S'il y a un problème
            Serial.printf("Sending transaction failed with error: %d\n", rcode);
            //ESP.restart();
            return PTP_RC_GeneralError;
        }
    }

    {  // Send data block
        uint8_t data[PTP_MAX_RX_BUFFER_LEN];
        uint32_t *pd32 = reinterpret_cast<uint32_t *>(data);

        if (myDataBlock.txOperation) {  //Si j'ai des data à envoyé
            ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

            //si pvoid est buffer pointer => BL = 12 + flags->dataSize
            //si pvoid est PTPReadParser => BL = 12 + ((PTPDataSupplier *)pVoid)->GetDataSize()
            //Sinon => BL = 12
            uint32_t bytes_left = myDataBlock.dataSize;

            // Make data PTP container header
            *pd32 = bytes_left;
            uint16_to_char_V2(PTP_USB_CONTAINER_DATA, (unsigned char *)(data + PTP_CONTAINER_CONTYPE_OFF));        // type
            uint16_to_char_V2(static_cast<uint16_t>(opcode), (unsigned char *)(data + PTP_CONTAINER_OPCODE_OFF));  // code
            uint32_to_char_V2(idTransaction, (unsigned char *)(data + PTP_CONTAINER_TRANSID_OFF));                 // transaction id

            uint16_t len = 0;

            /*if (flags->typeOfVoid == 1) {
                len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;
            }

            if (flags->typeOfVoid == 3) {
                uint8_t *p1 = (data + PTP_USB_BULK_HDR_LEN);
                uint8_t *p2 = (uint8_t *)pVoid;
                Serial.println("flags->typeOfVoid == 3");
                for (uint8_t i = flags->dataSize; i; i--, p1++, p2++) {
                    *p1 = *p2;
                }

                len = PTP_USB_BULK_HDR_LEN + flags->dataSize;
            }*/

            uint8_t *p1 = (data + PTP_USB_BULK_HDR_LEN);
            uint8_t *p2 = (uint8_t *)myDataBlock.data;

            for (uint8_t i = myDataBlock.dataSize; i; i--, p1++, p2++) {
                *p1 = *p2;
            }
            len = PTP_USB_BULK_HDR_LEN + myDataBlock.dataSize;

            bool first_time = true;

            while (bytes_left) {
                //if (flags->typeOfVoid == 1)
                //    ((PTPDataSupplier *)pVoid)->GetData((first_time) ? len - PTP_USB_BULK_HDR_LEN : len, (first_time) ? (data + PTP_USB_BULK_HDR_LEN) : data);

                rcode = pUsb->outTransfer(devAddress, epInfo[epDataOutIndex].epAddr, len, data);

                if (rcode) {
                    Serial.printf("Transaction: Data block send error.: %d\n", rcode);
                    ESP.restart();
                    return PTP_RC_GeneralError;
                }

                bytes_left -= len;
                len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;

                first_time = false;
            }
        }  //Fin envoi data

        // Because inTransfer does not return the actual number of bytes received, it should be
        // calculated here.
        uint32_t total = 0, data_off = 0;  // Total PTP data packet size, Data offset
        uint8_t inbuffer = 0;              // Number of bytes read into buffer
        uint16_t loops = 0;                // Number of loops necessary to get all the data from device

        uint8_t nefStartIndex = 254;
        uint32_t nefIndex = 0;
        uint32_t subIfdTagIndex = 0;
        uint32_t subIfdAddress = 0;
        uint32_t ifdAddress = 0;
        uint32_t jpegStartTagIndex = 0;
        uint32_t jpegLengthTagIndex = 0;
        uint32_t jpegStartAddress = 0;
        uint32_t jpegLength = 0;
        bool jpegStartFound = false;

        //uint8_t jpegFound = 0;
        //int endJpegFound = 65;

        uint8_t jpegType = myParamBlock.params[1];  //0: JEPG, 1: JPEG HQ, 2: RAW
        while (1) {
            //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
            if (opcode == MyOpCode::GetObject) {
                if (loops == 0) {
                    Serial.println("loop 0 getObject");
                }
                //sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, data, inbuffer);
                //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
                if (data_off % 100000 == 0)
                    Serial.printf("##off: %d\n", data_off);

                //if (nefStartIndex == 254)
                //Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);

                if (jpegStartFound) {
                    if (jpegType == 0)
                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, data, inbuffer);
                    else if (jpegType == 1)
                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegHqOut, data, inbuffer);

                    //Check end of JPEG
                    for (int i = 0; i < inbuffer; i++) {
                        if (data[i] == 0xFF && data[i + 1] == 0xD9) {
                            //Envoi du dernier packet avec la longueur du JPEG
                            if (jpegType == 0)
                                sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, data, i + 2);
                            else if (jpegType == 1)
                                sendBluetoothData(SerialBT, MyBluetoothCode::JpegHqOut, data, i + 2);

                            Serial.println("Sending last JPEG packet");

                            Serial.printf("Found end jpeg at %d\n", nefIndex);
                            delay(500);
                            return PTP_RC_Undefined;
                        }
                    }
                } else {
                    for (int i = 0; i < inbuffer; i++) {
                        if (nefStartIndex == 254) {  // on commence par chercher le debut du fichier NEF
                            if (data[i] == 0x49 && data[i + 1] == 0x49 &&
                                data[i + 2] == 0x2a && data[i + 3] == 0x00) {
                                nefStartIndex = i + data_off - inbuffer;
                                Serial.printf("nefStart Index found at %d\n", nefStartIndex);
                            }
                        } else {  // Le fichier NEF est trouvé !
                            nefIndex++;
                            if (subIfdTagIndex == 0) {  //On cherche le tag qui indique les subIfds
                                //Serial.printf("subIfdTagIndex == 0 || %d | %d | %d\n", data[i], data[i + 1], nefIndex);
                                if (nefIndex > 9 && ((nefIndex - 10) % 12) == 0 && data[i] == 0x4a && data[i + 1] == 0x01) {
                                    Serial.printf("Found subIfdTagIndex at %d\n", nefIndex);
                                    subIfdTagIndex = nefIndex;
                                }
                            } else if (subIfdAddress == 0) {  // On cherche l'adresse des subIfds
                                if (nefIndex == subIfdTagIndex + 8) {
                                    subIfdAddress = (data[i]) |
                                                    (data[i + 1] << 8) |
                                                    (data[i + 2] << 16) |
                                                    (data[i + 3] << 24);
                                    Serial.printf("Found subIfdAddress at %d\n", subIfdAddress);
                                }
                            } else if (ifdAddress == 0) {  // On cherche l'adresse du subIfd qui nous interresse
                                if (jpegType == 0) {       //JPEG
                                    if (nefIndex == subIfdAddress + 8) {
                                        ifdAddress = (data[i]) |
                                                     (data[i + 1] << 8) |
                                                     (data[i + 2] << 16) |
                                                     (data[i + 3] << 24);
                                        Serial.printf("Found ifdAddress at %d\n", ifdAddress);
                                    }
                                } else if (jpegType == 1) {  // JPEG HQ
                                    if (nefIndex == subIfdAddress) {
                                        ifdAddress = (data[i]) |
                                                     (data[i + 1] << 8) |
                                                     (data[i + 2] << 16) |
                                                     (data[i + 3] << 24);
                                        Serial.printf("Found ifdAddress at %d\n", ifdAddress);
                                    }
                                }
                            } else if (jpegStartTagIndex == 0) {  // On cherche l'index du tag de l'adresse du fichier JPEG
                                if (nefIndex > ifdAddress && ((nefIndex - ifdAddress - 2) % 12) == 0 && data[i] == 0x01 && data[i + 1] == 0x02) {
                                    Serial.printf("Found jpegStartTagIndex at %d\n", nefIndex);
                                    jpegStartTagIndex = nefIndex;
                                }
                            } else if (jpegStartAddress == 0) {  // On cherche l'index de start du fichier JPEG
                                if (nefIndex == jpegStartTagIndex + 8) {
                                    jpegStartAddress = data[i] |
                                                       (data[i + 1] << 8) |
                                                       (data[i + 2] << 16) |
                                                       (data[i + 3] << 24);
                                    Serial.printf("Found jpegStartAddress at %d, address= %d\n", nefIndex, jpegStartAddress);
                                }
                            } else if (jpegLengthTagIndex == 0) {  // On cherche l'index du tag de la longueur du fichier JPEG
                                if (((nefIndex - ifdAddress - 2) % 12) == 0 && data[i] == 0x02 && data[i + 1] == 0x02) {
                                    Serial.printf("Found jpegLengthTagIndex at %d\n", nefIndex);
                                    jpegLengthTagIndex = nefIndex;
                                }
                            } else if (jpegLength == 0) {  // On relève la longueur du JPEG
                                if (nefIndex == jpegLengthTagIndex + 8) {
                                    jpegLength = data[i] |
                                                 (data[i + 1] << 8) |
                                                 (data[i + 2] << 16) |
                                                 (data[i + 3] << 24);
                                    Serial.printf("Found jpegLength at %d, length= %d\n", nefIndex, jpegLength);

                                    //Envoi du premier packet avec la longueur du JPEG
                                    if (jpegType == 0)
                                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, data + i, 4);
                                    else if (jpegType == 1)
                                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegHqOut, data + i, 4);
                                    Serial.println("Sending JPEG length");
                                }
                            } else if (jpegStartFound == false) {  // On cherche le debut du JPEG
                                if (nefIndex == jpegStartAddress) {
                                    Serial.printf("Found start jpeg => %d | %d | %d \n", data[i], data[i + 1], data[i + 2]);
                                    jpegStartFound = true;

                                    //Envoi du premier packet du JPEG
                                    if (jpegType == 0)
                                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegOut, data + i, inbuffer - i);
                                    else if (jpegType == 1)
                                        sendBluetoothData(SerialBT, MyBluetoothCode::JpegHqOut, data + i, inbuffer - i);
                                    Serial.println("Sending first packet");
                                }
                            }
                        }
                    }  //end for
                }

            } else {
                if (total != 0 && loops == 1) {
                    Serial.printf("\n##total: %d, inBuf: %d, off: %d, loop: %d\n", total, inbuffer, data_off, loops);
                    Serial.printf("creating num uint8_t[%d]\n", total);
                    response = new uint8_t[total];
                    responseLenght = total;
                    Serial.printf("response buffer created, size:%d !\n", responseLenght);
                }

                //Serial.println("Data: ");
                for (int k = 0; k < inbuffer; k++) {
                    //Serial.printf("%02x ", data[k]);
                    response[k + data_off - inbuffer] = data[k];
                }
                //Serial.println();
            }

            ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

            uint16_t read = PTP_MAX_RX_BUFFER_LEN;

            rcode = pUsb->inTransfer(devAddress, epInfo[epDataInIndex].epAddr, &read, data);

            if (rcode) {
                Serial.printf("Transaction: inTransfer receive error: %d\n", rcode);
                ESP.restart();
                return PTP_RC_GeneralError;
            }

            // This can occur in case of unsupported operation or successive response after data reception stage
            if ((!loops || total == data_off) && *((uint16_t *)(data + PTP_CONTAINER_CONTYPE_OFF)) == PTP_USB_CONTAINER_RESPONSE) {
                uint16_t response = *((uint16_t *)(data + PTP_CONTAINER_OPCODE_OFF));

                if (response == PTP_RC_OK && *pd32 > PTP_USB_BULK_HDR_LEN) {
                    Serial.println("response with parameter !!");
                    // number of params = (container length - 12) / 4
                    uint8_t n = (*pd32 - PTP_USB_BULK_HDR_LEN) >> 2;
                    // 1 paramète = 4 bytes (0xFF 0xFF 0xFF 0xFF)

                    //
                    //
                    //flags->rsParams = n;
                    //
                    //

                    for (uint32_t *p1 = (uint32_t *)(data + PTP_USB_BULK_HDR_LEN), *p2 = (uint32_t *)myParamBlock.params; n; n--, p1++, p2++) {
                        Serial.printf("%02x # ", *p1);
                        //Serial.print(" # ");
                        p2 = p1;
                    }
                    Serial.println();
                }
                if (response != PTP_RC_OK) {
                    Serial.printf("Transaction: Response receive error: %d\n", response);
                    data_off = 0;
                }
                return response;
            }

            if (loops == 0) {
                total = *pd32;
                inbuffer = (total < PTP_MAX_RX_BUFFER_LEN) ? (uint8_t)total : PTP_MAX_RX_BUFFER_LEN;
            } else {
                inbuffer = ((total - data_off) > PTP_MAX_RX_BUFFER_LEN) ? PTP_MAX_RX_BUFFER_LEN : (uint8_t)(total - data_off);
            }

            data_off += inbuffer;
            //Serial.printf("data_off %d\n", data_off);

            loops++;
        }
    }  // end of scope
}