/* Copyright (C) 2021 Kristian Sloth Lauszus. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Sloth Lauszus
 Web      :  https://lauszus.com
 e-mail   :  lauszus@gmail.com
 */

#ifndef _ps5bt_h_
#define _ps5bt_h_

#include "BTHID.h"
#include "PS5Parser.h"

/**
 * Generated from the standard Ethernet CRC-32 polynomial:
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 * Source: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c
 */
const uint32_t crc32_table[]  = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/*
 * There are multiple 16-bit CRC polynomials in common use, but this is
 * *the* standard CRC-32 polynomial, first popularized by Ethernet.
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 */
#define CRC32_POLY_LE 0xedb88320

static inline uint32_t crc32_le_generic(uint32_t crc, uint8_t const *p, size_t len, uint32_t polynomial) {
        // Source: https://github.com/torvalds/linux/blob/c4cf498dc0241fa2d758dba177634268446afb06/lib/crc32.c
        int i;
        while (len--) {
                crc ^= *p++;
                for (i = 0; i < 8; i++)
                        crc = (crc >> 1) ^ ((crc & 1) ? polynomial : 0);
        }
        return crc;
}

static inline uint32_t crc32(uint32_t crc, const void *buf, size_t size) {
#if 1 // Use a table, as it's faster, but takes up more space
        // Inspired by: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c
        const uint8_t *p = (const uint8_t*)buf;
        while (size--)
                crc = pgm_read_dword(&crc32_table[*p++ ^ (crc & 0xFF)]) ^ (crc >> 8);
        return crc;
#else // Can be used to save flash, but is slower
        return crc32_le_generic(crc, (uint8_t const*)buf, size, CRC32_POLY_LE);
#endif
};

/**
 * This class implements support for the PS5 controller via Bluetooth.
 * It uses the BTHID class for all the Bluetooth communication.
 */
class PS5BT : public BTHID, public PS5Parser {
public:
        /**
         * Constructor for the PS5BT class.
         * @param  p     Pointer to the BTD class instance.
         * @param  pair  Set this to true in order to pair with the device. If the argument is omitted then it will not pair with it. One can use ::PAIR to set it to true.
         * @param  pin   Write the pin to BTD#btdPin. If argument is omitted, then "0000" will be used.
         */
        PS5BT(BTD *p, bool pair = false, const char *pin = "0000") :
        BTHID(p, pair, pin), output_sequence_counter(0) {
                PS5Parser::Reset();
        };

        /**
         * Used to check if a PS5 controller is connected.
         * @return Returns true if it is connected.
         */
        bool connected() {
                return BTHID::connected;
        };

protected:
        /** @name BTHID implementation */
        /**
         * Used to parse Bluetooth HID data.
         * @param len The length of the incoming data.
         * @param buf Pointer to the data buffer.
         */
        virtual void ParseBTHIDData(uint8_t len, uint8_t *buf) {
                PS5Parser::Parse(len, buf);
        };

        /**
         * Called when a device is successfully initialized.
         * Use attachOnInit(void (*funcOnInit)(void)) to call your own function.
         * This is useful for instance if you want to set the LEDs in a specific way.
         */
        virtual void OnInitBTHID() {
                PS5Parser::Reset();
                enable_sixaxis(); // Make the controller send out the entire output report

                // Only call this is a user function has not been set
                if (!pFuncOnInit)
                        setLed(Red); // Set the LED to red, as the PS5 controller turns Bluetooth when searching for a device
        };

        /** Used to reset the different buffers to there default values */
        virtual void ResetBTHID() {
                PS5Parser::Reset();
        };
        /**@}*/

        /** @name PS5Parser implementation */
        virtual void sendOutputReport(PS5Output *output) {
                // See the series of patches here: https://patchwork.kernel.org/project/linux-input/cover/20201219062336.72568-1-roderick@gaikai.com/
                uint8_t buf[1 /* BT DATA Output Report */ + 1 /* report id */ + 1 /* seq_tag */ + 1 /* tag */ + 47 /* common */ + 24 /* reserved */ + 4 /* crc32 */];
                memset(buf, 0, sizeof(buf));

                // Send as a Bluetooth HID DATA output report on the interrupt channel
                buf[0] = 0xA2; // HID BT DATA (0xA0) | Report Type (Output 0x02)

                buf[0x01] = 0x31; // Report ID
                buf[0x02] = (output_sequence_counter << 4) | 0x0; // Highest 4-bit is a sequence number, which needs to be increased every report. Lowest 4-bit is tag and can be zero for now.
                if(++output_sequence_counter == 15)
                        output_sequence_counter = 0;
                buf[0x03] = 0x10; // Magic number must be set to 0x10

                buf[0x01 + 3] = 0xFF; // feature flags 1
                buf[0x02 + 3]= 0xF7;  // feature flags 2

                buf[0x03 + 3] = output->smallRumble; // Small Rumble
                buf[0x04 + 3] = output->bigRumble; // Big rumble

                // 5-7 headphone, speaker, mic volume, audio flags

                buf[0x09 + 3] = (uint8_t)output->microphoneLed;

                // 0x0A mute flags

                // Adaptive Triggers: 0x0B-0x14 right, 0x15 unknown, 0x16-0x1F left
                rightTrigger.processTrigger(&buf[0x0B + 3]); // right
                leftTrigger.processTrigger(&buf[0x16 + 3]); // left

                // 0x20-0x24 unknown
                // 0x25 trigger motor effect strengths
                // 0x26 speaker volume

                // player LEDs
                buf[0x27 + 3] = 0x03; // led brightness, pulse
                buf[0x2A + 3] = output->disableLeds ? 0x01 : 0x2; // led pulse option
                // buf[0x2B] LED brightness, 0 = full, 1= medium, 2 = low
                buf[0x2C + 3] = output->playerLeds; // 5 white player LEDs

                // lightbar
                buf[0x2D + 3] = output->r; // Red
                buf[0x2E + 3] = output->g; // Green
                buf[0x2F + 3] = output->b; // Blue

                uint32_t crc = ~crc32(0xFFFFFFFF, buf, sizeof(buf) - 4 /* Do not include the crc32 */); // Note how the report type is also included in the output report
                buf[75] = crc & 0xFF;
                buf[76] = (crc >> 8) & 0xFF;
                buf[77] = (crc >> 16);
                buf[78] = (crc >> 24);

                output->reportChanged = false;

                // Send the Bluetooth DATA output report on the interrupt channel
                pBtd->L2CAP_Command(hci_handle, buf, sizeof(buf), interrupt_scid[0], interrupt_scid[1]);
        };
        /**@}*/

private:
        void enable_sixaxis() { // Command used to make the PS5 controller send out the entire output report
                // Request the paring info. This makes the controller send out the full report - see: https://patchwork.kernel.org/project/linux-input/cover/20201219062336.72568-1-roderick@gaikai.com/
                uint8_t buf[2];
                buf[0] = 0x43; // HID BT Get_report (0x40) | Report Type (Feature 0x03)
                buf[1] = 0x09; // Report ID for paring info

                // Send the Bluetooth Get_report Feature report on the control channel
                pBtd->L2CAP_Command(hci_handle, buf, 2, control_scid[0], control_scid[1]);
        };

        uint8_t output_sequence_counter;
};
#endif
