#include <struct_list.h>
#ifndef D3300_H
#define D3300_H

valueNameStructUint16 LIST_ATPERTURE[] = {
    {"F2.8", 0x0118},
    {"F3.2", 0x0140},
    {"F3.5", 0x015E},
    {"F4", 0x0190},
    {"F4.5", 0x01C2},
    {"F5", 0x01F4},
    {"F5.6", 0x0230},
    {"F6.3", 0x0276},
    {"F7.1", 0x02C6},
    {"F8", 0x0320},
    {"F9", 0x0384},
    {"F10", 0x03E8},
    {"F11", 0x044C},
    {"F13", 0x0514},
    {"F14", 0x0578},
    {"F16", 0x0640},
    {"F18", 0x0708},
    {"F20", 0x07D0},
    {"F22", 0x0898},
};

valueNameStructUint8 LIST_COMPRESSION[] = {
    {"BASIC", 0x00},
    {"NORMAL", 0x01},
    {"FINE", 0x02},
    {"RAW", 0x04},
    {"RAW_FINE", 0x07},
};

valueNameStructUint8_19 LIST_IMAGE_FORMAT[] = {
    {"6000x4000", {0x0A, 0x36, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x78, 0x00, 0x34, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00}},
    {"4496x3000", {0x0A, 0x34, 0x00, 0x34, 0x00, 0x39, 0x00, 0x36, 0x00, 0x78, 0x00, 0x33, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00}},
    {"2992x2000", {0x0A, 0x32, 0x00, 0x39, 0x00, 0x39, 0x00, 0x32, 0x00, 0x78, 0x00, 0x32, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00}},
};

valueNameStructUint16 LIST_WHITE_BALANCE[] = {
    {"AUTO", 0x0002},
    {"INCAND", 0x0004},
    {"FLUO", 0x0005},
    {"SUNNY", 0x0006},
    {"FLASH", 0x0007},
    {"CLOUDY", 0x8010},
    {"SHADOW", 0x8011},
    {"MANU", 0x8013},
};

valueNameStructUint16 LIST_FOCUS_MODE[] = {
    {"AF_AUTO", 0x8012},
    {"AF_PCT", 0x8010},
    {"AF_CONT", 0x8011},
    {"MANU", 0x0001},
};

valueNameStructUint16 LIST_EXP_MET_MODE[] = {
    {"MATRICE", 0x0003},
    {"CENTER", 0x0002},
    {"SPOT", 0x0004},
};

valueNameStructUint32 LIST_EXPOSURE_TIME[] = {
    {"1/4000", 0x00000002},
    {"1/3200", 0x00000003},
    {"1/2500", 0x00000004},
    {"1/2000", 0x00000005},
    {"1/1600", 0x00000006},
    {"1/1250", 0x00000008},
    {"1/1000", 0x0000000A},
    {"1/800", 0x0000000C},
    {"1/640", 0x0000000F},
    {"1/500", 0x00000014},
    {"1/400", 0x00000019},
    {"1/320", 0x0000001F},
    {"1/250", 0x00000028},
    {"1/200", 0x00000032},
    {"1/160", 0x0000003E},
    {"1/125", 0x00000050},
    {"1/100", 0x00000064},
    {"1/80", 0x0000007D},
    {"1/60", 0x000000A6},
    {"1/50", 0x000000C8},
    {"1/40", 0x000000FA},
    {"1/30", 0x0000014D},
    {"1/25", 0x00000190},
    {"1/20", 0x000001F4},
    {"1/15", 0x0000029A},
    {"1/13", 0x00000301},
    {"1/10", 0x000003E8},
    {"1/8", 0x000004E2},
    {"1/6", 0x00000682},
    {"1/5", 0x000007D0},
    {"1/4", 0x000009C4},
    {"1/3", 0x00000D05},
    {"1/2.5", 0x00000FA0},
    {"1/2", 0x00001388},
    {"1/1.6", 0x0000186A},
    {"1/1.3", 0x00001E0C},
    {"1", 0x00002710},
    {"1.3", 0x000032C8},
    {"1.6", 0x00003E80},
    {"2", 0x00004E20},
    {"2.5", 0x000061A8},
    {"3", 0x00007530},
    {"4", 0x00009C40},
    {"5", 0x0000C350},
    {"6", 0x0000EA60},
    {"8", 0x00013880},
    {"10", 0x000186A0},
    {"13", 0x0001FBD0},
    {"15", 0x000249F0},
    {"20", 0x00030D40},
    {"25", 0x0003D090},
    {"30", 0x000493E0},
    {"TIME", 0xFFFFFFFD},
    {"BULB", 0xFFFFFFFF},
};

valueNameStructUint16 LIST_EXP_PROG_MODE[] = {
    {"MANUEL", 0x0001},
    {"AUTO_PROG", 0x0002},
    {"PRIO_APER", 0x0003},
    {"PRIO_SPEE", 0x0004},
    {"AUTO", 0x8010},
    {"PORTRAIT", 0x8011},
    {"LANDSCP", 0x8012},
    {"MACRO", 0x8013},
    {"SPORT", 0x8014},
    {"NIGH", 0x8015},
    {"NO_FLASH", 0x8016},
    {"CHILD", 0x8017},
    {"EFFECT", 0x8019},
};

valueNameStructUint16 LIST_EXP_INDEX[] = {
    {"100", 0x0064},
    {"200", 0x00C8},
    {"400", 0x0190},
    {"800", 0x0320},
    {"1600", 0x0640},
    {"3200", 0x0C80},
    {"6400", 0x1900},
    {"12800", 0x3200},
    {"25600", 0x6400},
};

valueNameStructUint16 LIST_STILL_CAPTURE_MODE[] = {
    {"ONE_SHOT", 0x0001},
    {"BRUST", 0x0002},
    {"TIMER", 0x8011},
    {"REMOTE", 0x8014},
    {"REMOTE_T", 0x8015},
    {"QUIET", 0x8016},
};

valueNameStructUint8 LIST_COLOR_SPACE[] = {
    {"SRGB", 0x00},
    {"ADOBE", 0x01},
};

valueNameStructUint16 LIST_EXP_BIAS_COMP[] = {
    {"-5", 0xEC78},
    {"-4.67", 0xEDC6},
    {"-4.33", 0xEF13},
    {"-4", 0xF060},
    {"-3.67", 0xF1AE},
    {"-3.33", 0xF2FB},
    {"-3", 0xF448},
    {"-2.67", 0xF596},
    {"-2.33", 0xF6E3},
    {"-2", 0xF830},
    {"-1.67", 0xF97E},
    {"-1.33", 0xFACB},
    {"-1", 0xFC18},  //12
    {"-0.67", 0xFD66},
    {"-0.33", 0xFEB3},
    {"0", 0x0000},  //15
    {"+0.33", 0x014D},
    {"+0.67", 0x029A},
    {"+1", 0x03E8},  //18
    {"+1.33", 0x0535},
    {"+1.67", 0x0682},
    {"+2", 0x07D0},
    {"+2.33", 0x091D},
    {"+2.67", 0x0A6A},
    {"+3", 0x0BB8},
    {"+3.33", 0x0D05},
    {"+3.67", 0x0E52},
    {"+4", 0x0FA0},
    {"+4.33", 0x10ED},
    {"+4.67", 0x123A},
    {"+5", 0x1388},
};

valueNameStructUint16 LIST_EVENT[] = {
    {"imgSize", 0x5003},
    {"BatteryLv", 0x5001},
    {"CompSett", 0x5004},
    {"WhiteBal", 0x5005},
    {"Fnumber", 0x5007},
    {"FocalLen", 0x5008},
    {"FocusMode", 0x500A},
    {"ExpMetMod", 0x500B},
    {"FlashMod", 0x500C},
    {"ExpTime", 0x500D},
    {"ExpPrgMod", 0x500E},
    {"ExpIndex", 0x500F},
    {"ExpBiaCmp", 0x5010},
    {"DateTime", 0x5011},
    {"StlCptMod", 0x5013},
    {"BurstNbr", 0x5018},
    {"FocMetMod", 0x501C},
    {"AfMode", 0xD015},
    {"ExpComp", 0xD01F},
    {"ColorSpac", 0xD032},
    {"DispAv", 0xD037},
    {"LgExpNsRd", 0xD06B},
    {"NoSdCard", 0xD08A},
    {"IsoCtlSen", 0xD0B5},
    {"LensId", 0xD0E0},
    {"LensSort", 0xD0E1},
    {"LensType", 0xD0E2},
    {"FclLenMa", 0xD0E3},
    {"FclLenMin", 0xD0E4},
    {"ExpTimeNk", 0xD100},
    {"AcPower", 0xD101},
    {"ShtrSpeed", 0xD102},
    {"RcdMedia", 0xD10B},
    {"Dlight", 0xD14E},
    {"AfModeNk", 0xD161},
    {"AfAssist", 0xD163},
    {"IsoAuto", 0xD16A},
    {"LVStatus", 0xD1A2},
    {"LVZoomRt", 0xD1A3},
    {"LVProh", 0xD1A4},
    {"LVMovieMd", 0xD1AC},
    {"LVAfArea", 0x9205},
    {"ExpRemain", 0xD1F1},

};

uint8_t HdrNbrPhotoArray[] = {3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
float HdrExpPasArray[] = {0.33, 0.67, 1, 1.33, 1.67, 2, 2.33, 2.67, 3};
//float HdrExpOffsetArray[] = {-3, -2.67, -2.33, -2, -1.67, -1.33, -1, -0.67, -0.33, 0, 0.33, 0.67, 1, 1.33, 1.67, 2, 2.33, 2.67, 3};

#endif