/*#define logo_ble_blue_width 7
#define logo_ble_blue_height 12
static const unsigned char logo_ble_blue[] = {
    0x1c, 0x36, 0x43, 0x57, 0x61, 0x33, 0x71, 0x25, 0x57, 0x65, 0x26, 0x1c};

*/
#if !defined(LOGO_BLE_BLUE)
#define LOGO_BLE_BLUE

#include <stdlib.h>

static const _smCharType logo_ble_blue[12]
#if defined(_FORCE_PROGMEM__)
    PROGMEM
#endif
    = {
       0xc6, 0x92, 0x18, 0x14, 0x58, 0x30, 0x30, 0x58, 0x14, 0x18, 0x92, 0xc6
};

static const tIcon ble_blue = {logo_ble_blue, 7, 12, 12, RLE_no};

#endif

