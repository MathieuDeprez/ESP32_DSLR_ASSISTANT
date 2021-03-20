#ifndef _STRUCT_LIST
#define _STRUCT_LIST
#include <Arduino.h>
typedef struct
{
    const char name[10];
    const uint32_t value;
} valueNameStructUint32;

typedef struct
{
    const char name[10];
    const uint16_t value;
} valueNameStructUint16;

typedef struct
{
    const char name[10];
    const uint8_t value;
} valueNameStructUint8;

typedef struct
{
    const char name[10];
    const uint8_t value[19];
} valueNameStructUint8_19;

#endif