/* Camera controller header */
#ifndef __CAMCONTROLLER_H__
#define __CAMCONTROLLER_H__

//#define NO_SETTINGS_SCREEN

#include <inttypes.h>
#include <avr/pgmspace.h>

#include <canoneos.h>
#include <eosvaluetitles.h>
#include "menu.h"

const char msgSetSelf[]      = "SELF";
const char msgSetFrames[]    = "FRMS";
const char msgSetBkt[]       = "BKT";
const char msgSetInterval[]  = "INT";
const char msgExit[]         = "EXIT";
const char msgSetRun[]       = "RUN ";
const char msgAbort[]        = "ABORT";
const char msgLeft[]         = "LEFT";

const char msgError[]        = "ERROR:";
const char msgCamera[]       = "CAMERA";
const char msgDisconnected[]  = "DISCONNECTED";

const char msgSelfTimer[]    = "SELF TIMER h/m/s";
const char msgBracketing[]   = "BKT(RANGE/STEP)";
const char msgSteps[]        = "steps";
const char msgCntFrames[]    = "FRAMES";
const char msgIntTimer[]     = "INT TIMER h/m/s";

#ifndef NO_SETTINGS_SCREEN
const char msgIntervalometer[]  = "INTERVAL";
const char msgSettings[]     = "SETTINGS";
const char msgClock[]        = "CLOCK";
const char msgEV[]           = "EV";
#endif

typedef KeyValuePairDataItem<VT_EXPCOMP, VT_EXPCOMP_COUNT, VT_EXPCOMP_TEXT_LEN>    EXP_COMP_DATA_ITEM;            // Exposure compensation data item type
typedef SRAMValueList<VT_EXPCOMP, 16>                                              BKT_STEP_VALUE_LIST;           // Bracketing step value list type

//--- Data Item Types ---------------------------------------------------------
#ifndef NO_SETTINGS_SCREEN
typedef KeyValuePairDataItem<VT_MODE, VT_MODE_COUNT, VT_MODE_TEXT_LEN>             DIT_MODE;
typedef KeyValuePairDataItem<VT_APERTURE, VT_APT_COUNT, VT_APT_TEXT_LEN>           DIT_APERTURE;
typedef KeyValuePairDataItem<VT_WB, VT_WB_COUNT, VT_WB_TEXT_LEN>                   DIT_WB;
typedef KeyValuePairDataItem<VT_SHSPEED, VT_SHSPEED_COUNT, VT_SHSPEED_TEXT_LEN>    DIT_SHUTTER_SPEED;
typedef KeyValuePairDataItem<VT_PSTYLE, VT_PSTYLE_COUNT, VT_PSTYLE_TEXT_LEN>       DIT_PSTYLE;
typedef KeyValuePairDataItem<VT_ISO, VT_ISO_COUNT, VT_ISO_TEXT_LEN>                DIT_ISO;
#endif

typedef KeyValuePairDataItem<VT_EXPCOMP, VT_EXPCOMP_COUNT, VT_EXPCOMP_TEXT_LEN>    DIT_EXPCOMP;
typedef IntDataItem<uint8_t, 3>                                                    DIT_TIMER_DIGIT_PAIR;


#endif //__CAMCONTROLLER_H__
